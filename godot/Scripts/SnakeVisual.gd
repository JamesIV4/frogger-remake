class_name SnakeVisual

# Follow the head's trail in log-local coordinates. A reversal travels down
# the body instead of teleporting the tail to the other end of the sprite pair.
const Scale: float = 0.75
const HeadZ: float = 0.75
const TrailLength: float = 1.8
const TurnSeconds: float = 0.36
const StraightOffset: float = 0.085
const TurnRadius: float = StraightOffset * 1.5
var trail: Array[Vector2] = []
var heading: int = 0
var yaw: float = 0.0
var turn_start: float = -100.0
var last_time: float = -1.0
var lateral: float = 0.0

func reset() -> void:
	trail.clear()
	heading = 0
	last_time = -1.0
	turn_start = -100.0

func sample_trail(distance: float) -> Vector2:
	for i in range(1, trail.size()):
		var length: float = trail[i - 1].distance_to(trail[i])
		if length >= distance:
			return trail[i - 1].lerp(trail[i], distance / maxf(length, 0.00001))
		distance -= length
	return trail.back()

func update(actor: ModelActor, head_x: float, direction: int, origin: Vector3,
		left: float, right: float, time: float, on_log: bool = false) -> void:
	# Pausing removes the fractional render frame. Hold that tiny rollback
	# rather than mistaking it for a new game and straightening a curled tail.
	if time < last_time and last_time - time <= 0.017:
		time = last_time
	# The head is the first sprite, not the midpoint of the two-sprite snake.
	# A two-pixel inset keeps the broad 3D nose supported at a log-end turn.
	head_x = clampf(head_x, left + 0.135, right - 0.135)
	var end_clearance: float = right - head_x if direction > 0 else head_x - left
	var approach_width: float = lerpf(StraightOffset, TurnRadius, 1.0 - smoothstep(0.2, 0.8, end_clearance))
	if trail.is_empty() or time < last_time or time - last_time > 0.4:
		reset()
		heading = direction
		yaw = float(direction) * PI / 2.0
		lateral = float(direction) * approach_width
		for i in range(91):
			trail.append(Vector2(clampf(head_x - direction * float(i) * TrailLength / 90.0,
				left + 0.025, right - 0.025), lateral))
	var dt: float = maxf(0.0, time - last_time) if last_time >= 0.0 else 0.0
	# Smooth the ROM's crawl relative to its log, not two independently
	# smoothed world positions (their jitter would fold the trail repeatedly).
	if dt > 0.0:
		head_x = lerpf(trail[0].x, head_x, 1.0 - exp(-dt * 24.0))
	else:
		head_x = trail[0].x
	if direction != heading:
		turn_start = time
		heading = direction
	# Open out before the end, keep the broad U until the tail has followed,
	# then ease back toward the lane center. This widens the old .085 curl
	# radius without extending the curl beyond either end of the log.
	var width: float = maxf(approach_width, TurnRadius if time - turn_start < 1.8 else StraightOffset)
	# Keep the same bend when the ROM crawls faster (the bank snake). Let
	# distance advance the shared turn too, so the head cannot outrun the
	# sideways part of its curve and fold into its own neck.
	var turn_step: float = maxf(dt * TurnRadius * 2.0 / TurnSeconds, absf(head_x - trail[0].x) * 2.0 / PI)
	lateral = move_toward(lateral, float(heading) * width, turn_step)
	var head := Vector2(head_x, lateral)
	if head.distance_to(trail[0]) > 0.002:
		trail.push_front(head)
		var length: float = 0.0
		for i in range(1, trail.size()):
			length += trail[i - 1].distance_to(trail[i])
			if length > TrailLength:
				trail.resize(i + 1)
				break
	last_time = time
	# Aim the rigid head away from the first neck section. A separate yaw
	# tween can turn the snout into the returning body, especially on land
	# where the crawl is faster than on a log.
	var neck: Vector2 = body_point((HeadZ - 0.505) * Scale, time, left, right)
	var forward: Vector2 = head - neck
	if forward.length_squared() > 0.0001:
		yaw = atan2(forward.x, forward.y)
	else:
		yaw = float(heading) * PI / 2.0
	actor.root.rotation = Vector3(0.0, yaw, 0.0)
	actor.root.scale = Vector3.ONE * Scale
	actor.root.position = origin + Vector3(head.x, 0.0, head.y) - actor.root.basis * Vector3(0.0, 0.0, HeadZ)
	if on_log:
		actor.root.position.y += log_surface_offset(head, left, right)
	actor.pose("Move", fmod(time, 1.2))
	var skeleton: Skeleton3D = actor.skeleton
	# Global bone poses let each section follow the curved trail while keeping
	# its original skin weights and the steady head. Everything remains planar.
	for segment in range(1, 15):
		var distance: float = (HeadZ - 0.505 + float(segment - 1) * 0.095) * Scale
		var point: Vector2 = body_point(distance, time, left, right)
		var tangent: Vector2 = (body_point(distance + 0.025, time, left, right) - body_point(distance - 0.025, time, left, right)).normalized()
		if tangent.length_squared() < 0.5:
			tangent = Vector2(-heading, 0.0)
		var along := Vector3(tangent.x, 0.0, tangent.y)
		var basis := Basis(along.cross(Vector3.UP), along, Vector3.UP)
		var world_point: Vector3 = origin + Vector3(point.x, 0.13 * Scale, point.y)
		world_point.y -= (body_bottom(distance) - ModelFootprints.SnakeBottomTiles) * Scale
		if on_log:
			world_point.y += log_surface_offset(point, left, right)
		var pose := Transform3D((skeleton.global_basis.inverse() * basis).orthonormalized(), skeleton.to_local(world_point))
		skeleton.set_bone_global_pose_override(skeleton.find_bone("Segment%d" % segment), pose, 1.0, true)

func body_point(distance: float, time: float, left: float, right: float) -> Vector2:
	var point: Vector2 = sample_trail(distance)
	# The trail supplies the U-turn, with a traveling S-wave along the tail.
	# Fade in behind the fixed neck, retaining the broader requested turn.
	var strength: float = smoothstep(0.15, 0.9, distance)
	point.y += 0.065 * strength * sin(time * TAU / 1.2 - distance * 7.5)
	point.x = clampf(point.x, left + 0.04, right - 0.04)
	return point

static func body_bottom(distance: float) -> float:
	var y: float = distance / Scale - HeadZ
	var profile: Array[Vector3] = ModelFootprints.SnakeProfile
	for i in range(1, profile.size()):
		if y <= profile[i].x:
			var t: float = clampf(inverse_lerp(profile[i - 1].x, profile[i].x, y), 0.0, 1.0)
			return 0.145 - lerpf(profile[i - 1].z, profile[i].z, t) * sin(TAU * 0.3)
	return 0.145 - profile.back().z * sin(TAU * 0.3)

static func log_surface_offset(point: Vector2, left: float, right: float) -> float:
	var x: float = point.x / (right - left)
	var profile: Array[Vector4] = ModelFootprints.LogProfile
	var section: Vector4 = profile.back()
	for i in range(1, profile.size()):
		if x <= profile[i].x:
			section = profile[i - 1].lerp(profile[i], clampf(inverse_lerp(profile[i - 1].x, profile[i].x, x), 0.0, 1.0))
			break
	# The ten-sided bark has a flat top and two sloping upper facets. Sample
	# those exact facets, including the taper toward the cut ends, instead of
	# suspending every part at the model's single highest bark knot.
	var across: float = absf(-point.y - section.y) / section.z
	var height: float = sin(TAU * 0.2)
	if across > cos(TAU * 0.2):
		height = lerpf(sin(TAU * 0.2), sin(TAU * 0.1), clampf(inverse_lerp(cos(TAU * 0.2), cos(TAU * 0.1), across), 0.0, 1.0))
	if across > cos(TAU * 0.1):
		height = lerpf(sin(TAU * 0.1), 0.0, clampf(inverse_lerp(cos(TAU * 0.1), 1.0, across), 0.0, 1.0))
	return 0.16 + section.w * height - ModelFootprints.LogTopTiles
