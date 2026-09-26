class_name FrogVisualState

const FrameSeconds: float = 33.0 / 2000.0
const MaximumDrownDepth: float = 1.15

var dying: bool = false
var drowning: bool = false
var carrying: bool = false
var death_frame: int = 0
var death_x: int = 0
var death_row: int = 0
var hop_direction: int = 0
var hop_start_frame: int = 0
var previous_hop: int = 0

func reset() -> void:
	dying = false
	drowning = false
	carrying = false
	death_frame = 0
	hop_direction = 0
	hop_start_frame = 0
	previous_hop = 0

static func player_on_board(s: Dictionary) -> bool:
	var x: int = BoardVisuals.at(s, 0x8044)
	var row: int = BoardVisuals.at(s, 0x8047)
	return x >= 8 and x <= 240 and row >= 26 and row <= 240

func observe(s: Dictionary) -> void:
	var riding_gator: bool = BoardVisuals.at(s, 0x8004) != 0 and BoardVisuals.river_gator_ride(s)
	var dead: bool = player_on_board(s) and BoardVisuals.at(s, 0x8004) != 0 and BoardVisuals.at(s, 0x83cd) == 0 and not riding_gator
	if dead and not dying:
		death_frame = s.get("frame", 0)
		death_x = BoardVisuals.at(s, 0x8044)
		death_row = BoardVisuals.at(s, 0x8047)
		drowning = BoardVisuals.at(s, 0x829c) != 0
	dying = dead
	carrying = not dead and player_on_board(s) and BoardVisuals.at(s, 0x8134) != 0 and BoardVisuals.at(s, 0x8135) != 0
	var hop: int = 0
	for i in range(4):
		if BoardVisuals.at(s, 0x8248 + i) != 0:
			hop = i + 1
	if hop != 0 and hop != previous_hop:
		hop_direction = hop
		hop_start_frame = s.get("frame", 0)
	previous_hop = hop

func hop_active(frame: int) -> bool:
	return not dying and hop_direction != 0 and frame - hop_start_frame < 10

func hop_seconds(frame: int, fraction: float) -> float:
	return clampf((float(frame - hop_start_frame) + fraction) / 10.0, 0.0, 1.0) * (10.0 / 60.0)

func death_seconds(frame: int, fraction: float) -> float:
	return maxf(0.0, float(frame - death_frame) + fraction) * FrameSeconds

static func ease_val(t: float) -> float:
	var p: float = clampf(t, 0.0, 1.0)
	return p * p * (3.0 - 2.0 * p)

static func squash_progress(seconds: float) -> float:
	return ease_val(seconds / 0.11)

static func drown_depth(seconds: float) -> float:
	return MaximumDrownDepth * (1.0 - exp(-maxf(0.0, seconds) / 0.25))

static func drown_height(surface_height: float, initial_height: float, seconds: float) -> float:
	var floor_val: float = surface_height - MaximumDrownDepth
	var remaining: float = maxf(0.0, initial_height - floor_val)
	return initial_height - remaining * drown_depth(seconds) / MaximumDrownDepth


class HomeArrivalVisual:
	const HoldSeconds: float = 0.25
	const HopCompletionSeconds: float = 6.0 * (33.0 / 2000.0)
	const HopClipSeconds: float = 10.0 / 60.0
	const FrameSeconds: float = 33.0 / 2000.0

	var start_frame: int = -1
	var x: int = 0
	var row: int = 0
	var target_x: int = 0
	var passenger: bool = false
	var start_hop_pose_seconds: float = 0.0

	func reset() -> void:
		start_frame = -1
		passenger = false

	func cancel() -> void:
		reset()

	func begin(time_award: Dictionary, p_passenger: bool, p_hop_pose_seconds: float = 0.0) -> void:
		start_frame = time_award.get("frame", 0)
		x = time_award.get("x", 0)
		row = time_award.get("row", 0)
		passenger = p_passenger
		var bay: int = clampi(int(roundf((float(x) - 24.0) / 48.0)), 0, 4)
		target_x = 24 + 48 * bay
		start_hop_pose_seconds = clampf(p_hop_pose_seconds, 0.0, HopClipSeconds)

	func elapsed(frame: int, fraction: float) -> float:
		return maxf(0.0, float(frame - start_frame) + fraction) * FrameSeconds

	func completion() -> float:
		return HopCompletionSeconds if row > 32 else 0.0

	func active(frame: int, fraction: float = 0.0) -> bool:
		return start_frame >= 0 and frame >= start_frame and elapsed(frame, fraction) < completion() + HoldSeconds

	func finishing_hop(frame: int, fraction: float = 0.0) -> bool:
		return active(frame, fraction) and elapsed(frame, fraction) < completion()

	func progress(frame: int, fraction: float) -> float:
		var comp: float = completion()
		if comp == 0.0:
			return 1.0
		var p: float = clampf(elapsed(frame, fraction) / comp, 0.0, 1.0)
		return p * p * (3.0 - 2.0 * p)

	func visual_row(frame: int, fraction: float = 0.0) -> float:
		return float(row) + float(32 - row) * progress(frame, fraction)

	func visual_x(frame: int, fraction: float = 0.0) -> float:
		return float(x) + float(target_x - x) * progress(frame, fraction)

	func hop_pose_seconds(frame: int, fraction: float = 0.0) -> float:
		return start_hop_pose_seconds + (HopClipSeconds - start_hop_pose_seconds) * progress(frame, fraction)
