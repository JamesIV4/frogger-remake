class_name InputPulse

const Up: int = 1
const Down: int = 2
const Left: int = 4
const Right: int = 8

var analog_mask: int = 0

func reset() -> void:
	analog_mask = 0

func from_inputs(digital_mask: int, stick_x: float, stick_y: float) -> int:
	var previous: int = analog_mask
	analog_mask = 0
	if stick_y < (-0.30 if (previous & Up) != 0 else -0.55):
		analog_mask |= Up
	if stick_y > (0.30 if (previous & Down) != 0 else 0.55):
		analog_mask |= Down
	if stick_x < (-0.30 if (previous & Left) != 0 else -0.55):
		analog_mask |= Left
	if stick_x > (0.30 if (previous & Right) != 0 else 0.55):
		analog_mask |= Right
	return from_held_mask(digital_mask | analog_mask)

func from_held_mask(mask: int) -> int:
	mask &= (Up | Down | Left | Right)
	if (mask & Up) != 0:
		return Up
	if (mask & Down) != 0:
		return Down
	if (mask & Left) != 0:
		return Left
	if (mask & Right) != 0:
		return Right
	return 0
