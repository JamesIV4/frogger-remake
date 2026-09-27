class_name ModelFootprints

const FrogAlongX: float = 8.56170
const FrogAcrossRow: float = 6.89952
const RiverGatorLengthTiles: float = 2.275091
const RiverGatorWidthTiles: float = 1.100000
const RiverGatorFrontTiles: float = 1.150847
const RiverGatorSnoutTiles: float = 0.725847
const LogTopTiles: float = 0.436645
const BeaverTopTiles: float = 0.402000
const BeaverFrontTiles: float = 0.728000
const SnakeBottomTiles: float = 0.021363
const LadyBottomTiles: float = -0.006061

# Authored support/underside profiles for the snake to follow the faceted log.
const LogProfile: Array[Vector4] = [Vector4(-0.5, -0.006, 0.213, 0.22), Vector4(-0.39, 0.008, 0.235, 0.25), Vector4(-0.2, -0.012, 0.25, 0.268), Vector4(0.02, 0.014, 0.245, 0.263), Vector4(0.23, -0.005, 0.24, 0.255), Vector4(0.4, 0.004, 0.228, 0.242), Vector4(0.5, -0.008, 0.208, 0.218)]
const SnakeProfile: Array[Vector3] = [Vector3(-0.92, 0.072, 0.055), Vector3(-0.85, 0.13, 0.095), Vector3(-0.76, 0.17, 0.13), Vector3(-0.67, 0.16, 0.12), Vector3(-0.58, 0.125, 0.1), Vector3(-0.43, 0.12, 0.095), Vector3(-0.25, 0.118, 0.09), Vector3(-0.06, 0.113, 0.085), Vector3(0.13, 0.105, 0.08), Vector3(0.32, 0.095, 0.075), Vector3(0.51, 0.078, 0.068), Vector3(0.7, 0.055, 0.052), Vector3(0.88, 0.02, 0.025)]

static func vehicle(lane: int) -> Dictionary:
	match lane:
		6: return {"min_along_x": -13.09056, "max_along_x": 13.30560, "across_row": 6.07488}
		7: return {"min_along_x": -7.79520, "max_along_x": 8.29920, "across_row": 6.07488}
		8: return {"min_along_x": -8.29920, "max_along_x": 7.79520, "across_row": 6.07488}
		9: return {"min_along_x": -6.76704, "max_along_x": 8.66880, "across_row": 6.24960}
		10: return {"min_along_x": -8.87040, "max_along_x": 6.76704, "across_row": 6.07488}
		_: push_error("Invalid lane: %d" % lane); return {}
