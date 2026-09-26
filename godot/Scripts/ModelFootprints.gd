class_name ModelFootprints

const FrogAlongX: float = 8.56170
const FrogAcrossRow: float = 6.89952
const RiverGatorLengthTiles: float = 2.153474
const RiverGatorWidthTiles: float = 1.100000
const RiverGatorFrontTiles: float = 1.030000
const RiverGatorSnoutTiles: float = 0.605000
const LogTopTiles: float = 0.436645
const SnakeBottomTiles: float = 0.021363
const LadyBottomTiles: float = -0.006061

static func vehicle(lane: int) -> Dictionary:
	match lane:
		6: return {"min_along_x": -13.09056, "max_along_x": 13.30560, "across_row": 6.07488}
		7: return {"min_along_x": -7.79520, "max_along_x": 8.29920, "across_row": 6.07488}
		8: return {"min_along_x": -8.29920, "max_along_x": 7.79520, "across_row": 6.07488}
		9: return {"min_along_x": -6.76704, "max_along_x": 8.66880, "across_row": 6.24960}
		10: return {"min_along_x": -8.87040, "max_along_x": 6.76704, "across_row": 6.07488}
		_: push_error("Invalid lane: %d" % lane); return {}
