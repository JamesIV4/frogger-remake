extends SceneTree
func _init():
	for prop in ProjectSettings.get_property_list():
		if prop.name.begins_with("audio/"):
			print(prop.name, " = ", ProjectSettings.get_setting(prop.name), " (hint: ", prop.hint_string, ")")
	quit()
