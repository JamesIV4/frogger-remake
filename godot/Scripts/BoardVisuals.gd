class_name BoardVisuals

enum RiverGatorZone { Outside, Back, Snout }

const LeftEdge: float = 8.0
const RightEdge: float = 232.0
const RiverGatorVisiblePixels: int = 57
const RiverGatorDangerPixels: int = 16
const LadyFrogRow: int = 96

static func river_gator_contact(frog_x: int, tip_x: int) -> RiverGatorZone:
	var behind: int = (tip_x - frog_x + 256) & 255
	if behind < RiverGatorDangerPixels:
		return RiverGatorZone.Snout
	if behind <= RiverGatorVisiblePixels:
		return RiverGatorZone.Back
	return RiverGatorZone.Outside

static func river_gator_active(state: Dictionary) -> bool:
	return at(state, 0x83b7) >= 2 and (at(state, 0x8150) & 1) != 0 and at(state, 0x8101) != 0

static func river_gator_ride(state: Dictionary) -> bool:
	if not river_gator_active(state) or at(state, 0x829c) != 0:
		return false
	var biased: int = (at(state, 0x8047) + 8) & 255
	if biased < 42 or biased >= 59:
		return false
	return river_gator_contact(at(state, 0x8044), at(state, 0x8101)) == RiverGatorZone.Back

static func lady_frog_height(scale_val: float) -> float:
	return -0.18 + ModelFootprints.LogTopTiles - scale_val * ModelFootprints.LadyBottomTiles + 0.012

static func fit_river_gator(native_width: int) -> Dictionary:
	var length_scale: float = (float(native_width) - 3.0) / (16.0 * ModelFootprints.RiverGatorLengthTiles)
	var width_scale: float = 14.0 / (16.0 * ModelFootprints.RiverGatorWidthTiles)
	var center_offset: float = float(native_width) / 2.0 + 12.0 - 16.0 * ModelFootprints.RiverGatorFrontTiles * length_scale
	return {
		"center_offset_pixels": center_offset,
		"width_scale": width_scale,
		"length_scale": length_scale
	}

static func intersects_playfield(center: float, half_width: float) -> bool:
	return center + half_width >= LeftEdge and center - half_width <= RightEdge

static func interpolate_byte(previous: int, current: int, alpha: float) -> float:
	var d: int = ((current - previous + 128) & 255) - 128
	if absi(d) > 8:
		return float(current)
	return float(previous) + float(d) * clampf(alpha, 0.0, 1.0)

static func surface_height(row: float) -> float:
	if row >= 216.0:
		return mix(0.02, 0.075, (row - 216.0) / 8.0)
	if row >= 136.0:
		return 0.02
	if row >= 128.0:
		return mix(0.075, 0.02, (row - 128.0) / 8.0)
	if row >= 112.0:
		return mix(0.20, 0.075, (row - 112.0) / 16.0)
	if row >= 48.0:
		return 0.20
	return mix(0.08, 0.20, (row - 32.0) / 16.0)

static func mix(a: float, b: float, t: float) -> float:
	return a + (b - a) * clampf(t, 0.0, 1.0)

static func smooth_val(t: float) -> float:
	t = clampf(t, 0.0, 1.0)
	return t * t * (3.0 - 2.0 * t)

static func home_gator_reveal(state: Dictionary, full: bool, fraction: float = 0.0) -> float:
	if full:
		return 1.0
	return smooth_val((float(at(state, 0x8122)) + clampf(fraction, 0.0, 1.0)) / 80.0)

static func turtle_depth(state: Dictionary, x: float, row: int, alpha: float = 0.0, known_diver: bool = false) -> float:
	if not known_diver and turtle_phase(state, x, row) == 0:
		return 0.0
	var p: float = float(at(state, 0x8110 if row == 64 else 0x8111)) + (1.0 if row == 64 else 2.0) * alpha
	if row == 64:
		if p < 80.0:
			return 0.0
		if p < 160.0:
			return 0.70 * smooth_val((p - 80.0) / 80.0)
		if p < 176.0:
			return 0.70
		return 0.70 * (1.0 - smooth_val((p - 176.0) / 80.0))
	if p < 80.0:
		return 0.70 * smooth_val(p / 80.0)
	if p < 96.0:
		return 0.70
	if p < 176.0:
		return 0.70 * (1.0 - smooth_val((p - 96.0) / 80.0))
	return 0.0

static func at(state: Dictionary, a: int) -> int:
	if a >= 0xa800:
		var vid = state.get("video")
		return vid[(a - 0xa800) & 1023] if vid != null and vid.size() > ((a - 0xa800) & 1023) else 0
	var ram = state.get("ram")
	return ram[(a - 0x8000) & 2047] if ram != null and ram.size() > ((a - 0x8000) & 2047) else 0

static func tile_at(state: Dictionary, frog_space_x: float, frog_space_row: int, column_offset: int = 0) -> int:
	var col: int = (frog_space_row / 8 + column_offset) & 31
	var objects = state.get("objects", [])
	if objects.size() <= col * 2:
		return 0
	var raw: int = objects[col * 2]
	var scroll: int = ((raw >> 4) | (raw << 4)) & 255
	var native_y: int = (248 - int(roundf(frog_space_x)) + scroll) & 255
	var video = state.get("video", [])
	var idx: int = (native_y >> 3) * 32 + col
	return video[idx] if idx < video.size() else 0

static func turtle_phase(state: Dictionary, x: float, row: int) -> int:
	var surface: bool = false
	var bubbles: bool = false
	var blank: bool = true
	for offset in [-4, 0, 4]:
		for col in [0, 1]:
			var tile: int = tile_at(state, x + offset, row, col)
			surface = surface or (tile >= 0x70 and tile <= 0x87)
			bubbles = bubbles or (tile >= 0x94 and tile <= 0x9b)
			blank = blank and (tile == 0x10)
	if surface:
		return 0
	if bubbles:
		return 1
	if blank:
		return 2
	return 0


class LadyFrogPresentation:
	var last_x: Variant = null
	var active: bool = false
	var x: float = 0.0

	func visible(half_width: float) -> bool:
		return active and BoardVisuals.intersects_playfield(x, half_width)

	func reset() -> void:
		active = false
		last_x = null
		x = 0.0

	func observe(state: Dictionary, read_rom: Callable) -> void:
		active = BoardVisuals.at(state, 0x83fe) != 0 and BoardVisuals.at(state, 0x8135) != 0 and BoardVisuals.at(state, 0x8134) == 0
		if not active:
			last_x = null
			return
		var code: int = BoardVisuals.at(state, 0x8041)
		var color: int = BoardVisuals.at(state, 0x8042)
		var row: int = BoardVisuals.at(state, 0x8043)
		var patrol_sprite: bool = (code == 0x1e or code == 0x21 or code == 0xa1) and color == 4 and row == BoardVisuals.LadyFrogRow
		var cur_x: int
		if patrol_sprite:
			cur_x = BoardVisuals.at(state, 0x8040)
		else:
			var index: int = (BoardVisuals.at(state, 0x833d) & 0x7f) + 1
			var offset: int = read_rom.call(0x279f + index)
			if offset >= 2:
				cur_x = (offset + BoardVisuals.at(state, 0x811c)) & 255
			elif last_x != null:
				cur_x = int(last_x)
			else:
				cur_x = BoardVisuals.at(state, 0x8040)
		x = float(cur_x)
		last_x = cur_x


class HomeGatorVisual:
	const RetreatFrames: float = 12.0
	var retreat_start: float = -1.0
	var last_reveal: float = 0.0
	var was_active: bool = false

	func retreating() -> bool:
		return retreat_start >= 0.0

	func reset() -> void:
		retreat_start = -1.0
		last_reveal = 0.0
		was_active = false

	func reveal(frame: float, is_active: bool, native_reveal: float) -> Variant:
		if is_active:
			was_active = true
			retreat_start = -1.0
			last_reveal = native_reveal
			return native_reveal
		if was_active:
			was_active = false
			retreat_start = frame
		if retreat_start < 0.0:
			return null
		var t: float = clampf((frame - retreat_start) / RetreatFrames, 0.0, 1.0)
		if t >= 1.0:
			retreat_start = -1.0
			return null
		var smooth_val: float = t * t * (3.0 - 2.0 * t)
		return last_reveal * (1.0 - smooth_val)
