class_name PresentationMotion

class Track:
	var native_x: float
	var display_x: float
	var velocity: float = 0.0
	var frame: int
	var history: Array = []

	func _init(pos: float, f: int):
		native_x = pos
		display_x = pos
		frame = f
		history.append({"frame": f, "x": pos})

var tracks: Dictionary = {}
var history_frames: int
var contact_tolerance: float
var correction_rate: float
const FRAME_SECONDS: float = 33.0 / 2000.0

func _init(p_history_frames: int = 32, p_contact_tolerance: float = 1.0, p_correction_rate: float = 24.0):
	history_frames = maxi(2, p_history_frames)
	contact_tolerance = clampf(p_contact_tolerance, 0.25, 2.0)
	correction_rate = clampf(p_correction_rate, 1.0, 120.0)

func reset() -> void:
	tracks.clear()

func display_x(id: int) -> Variant:
	if tracks.has(id):
		return wrap_val(tracks[id].display_x)
	return null

func velocity_x(id: int) -> float:
	if tracks.has(id):
		return tracks[id].velocity
	return 0.0

func step(id: int, native_pos: float, native_frame: int, render_seconds: float, paused: bool = false) -> float:
	if not tracks.has(id):
		tracks[id] = Track.new(native_pos, native_frame)
		return wrap_val(native_pos)
	var track: Track = tracks[id]
	var elapsed: int = native_frame - track.frame
	if elapsed < 0 or elapsed > 24:
		tracks[id] = Track.new(native_pos, native_frame)
		return wrap_val(native_pos)
	if elapsed > 0:
		var advance: float = native_pos - track.native_x
		advance -= 256.0 * roundf(advance / 256.0)
		if absf(advance) > 12.0:
			tracks[id] = Track.new(native_pos, native_frame)
			return wrap_val(native_pos)
		track.native_x += advance
		track.frame = native_frame
		track.history.append({"frame": native_frame, "x": track.native_x})
		while track.history.size() > history_frames:
			track.history.pop_front()
		var oldest: Dictionary = track.history[0]
		if native_frame > oldest.frame:
			var desired: float = (track.native_x - oldest.x) / float(native_frame - oldest.frame)
			track.velocity += (desired - track.velocity) * (1.0 - exp(-0.55 * elapsed))
	if not paused:
		track.display_x += track.velocity * clampf(render_seconds / FRAME_SECONDS, 0.0, 1.5)
	var error: float = track.native_x - track.display_x
	if absf(error) > 4.0:
		track.display_x = track.native_x
	elif absf(error) > contact_tolerance:
		var correction: float = error - signf(error) * contact_tolerance
		track.display_x += correction * clampf(render_seconds * correction_rate, 0.0, 1.0)
	if track.display_x >= 512.0 or track.display_x < -256.0:
		var shift: float = 256.0 * floorf(track.display_x / 256.0)
		track.display_x -= shift
		track.native_x -= shift
		for item in track.history:
			item.x -= shift
	return wrap_val(track.display_x)

static func wrap_val(x: float) -> float:
	x = fmod(x, 256.0)
	return x + 256.0 if x < 0.0 else x
