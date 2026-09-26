class_name GameDefaults

const PreferencesVersion: int = 2

static func from_stored(version: int, modern: bool, perspective: bool, follow: bool, fullscreen: bool) -> Dictionary:
	if version >= PreferencesVersion:
		return {
			"modern": modern,
			"perspective": perspective,
			"follow": follow,
			"fullscreen": fullscreen
		}
	return {
		"modern": true,
		"perspective": true,
		"follow": true,
		"fullscreen": true
	}
