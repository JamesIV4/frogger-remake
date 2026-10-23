class_name UiKit

## Shared look for the HUD and menus. Colors come from the board palette in
## art/scripts/build_assets.py; type stays Press Start 2P / Silkscreen with
## chunky ink outlines; controls are CandyStyle faces with a little bounce.

const Ink: Color = Color("101b32")
const Cream: Color = Color("fff3bd")
const Yellow: Color = Color("ffdd00")
const Lime: Color = Color("b0ff39")
const Green: Color = Color("47d61b")
const DarkGreen: Color = Color("08732e")
const Blue: Color = Color("00aafa")
# Secondary buttons: bright aqua, distinct from the blue panels.
const Secondary: Color = Color("12bfe8")
const Red: Color = Color("ed1237")
const Orange: Color = Color("ff7a16")
const Pink: Color = Color("ff269c")
# Bubbly candy style: friendly mid-blue panels, rounded corners, glossy
# highlights and soft ledges, with the game's bright green, orange and yellow.
## Corner radius for every rounded UI box.
const Radius: int = 12
const PanelFace: Color = Color("2f74e0")
const PanelDeep: Color = Color("1d58b8")
const PanelRim: Color = Color("8cc8ff")
const SoftText: Color = Color("e2f2ff")

const ToggleOn: String = """<svg xmlns="http://www.w3.org/2000/svg" width="60" height="34" viewBox="0 0 60 34">
<rect x="2" y="6" width="56" height="26" rx="13" fill="#101b32"/>
<rect x="2.5" y="2.5" width="55" height="25" rx="12.5" fill="#47d61b" stroke="#101b32" stroke-width="3"/>
<rect x="8" y="6" width="24" height="5" rx="2.5" fill="#ffffff" fill-opacity=".45"/>
<circle cx="43" cy="15" r="10" fill="#fff3bd" stroke="#101b32" stroke-width="3"/>
</svg>"""
const ToggleOff: String = """<svg xmlns="http://www.w3.org/2000/svg" width="60" height="34" viewBox="0 0 60 34">
<rect x="2" y="6" width="56" height="26" rx="13" fill="#101b32"/>
<rect x="2.5" y="2.5" width="55" height="25" rx="12.5" fill="#164a9e" stroke="#101b32" stroke-width="3"/>
<circle cx="17" cy="15" r="10" fill="#cfe6ff" stroke="#101b32" stroke-width="3"/>
</svg>"""
const Knob: String = """<svg xmlns="http://www.w3.org/2000/svg" width="30" height="30" viewBox="0 0 30 30">
<circle cx="15" cy="17" r="11" fill="#101b32"/>
<circle cx="15" cy="14" r="11" fill="%s" stroke="#101b32" stroke-width="3"/>
<circle cx="12" cy="10.5" r="3" fill="#ffffff" fill-opacity=".6"/>
</svg>"""
# The app icon's frog without its tile, outlined so it reads on any panel.
const FrogLife: String = """<svg xmlns="http://www.w3.org/2000/svg" width="128" height="128" viewBox="0 0 128 128">
<path d="M16 77C16 62 25 51 26 39C27 25 34 18 43 18C53 18 59 26 64 32C69 26 75 18 85 18C95 18 102 26 103 39C104 51 113 62 113 77C113 97 95 108 64 108C33 108 16 97 16 77Z" fill="#72b438" stroke="#101b32" stroke-width="9" stroke-linejoin="round"/>
<ellipse cx="64" cy="79" rx="47" ry="25" fill="#97ce43"/>
<path d="M18 86C28 98 45 100 64 100C83 100 101 98 111 86C106 102 88 108 64 108C40 108 23 102 18 86Z" fill="#38823a"/>
<g fill="#8bc741"><ellipse cx="43" cy="42" rx="22" ry="26"/><ellipse cx="85" cy="42" rx="22" ry="26"/></g>
<g fill="#f0d472"><ellipse cx="43" cy="41" rx="15" ry="18"/><ellipse cx="85" cy="41" rx="15" ry="18"/></g>
<g fill="#162334"><ellipse cx="45" cy="42" rx="7" ry="12"/><ellipse cx="83" cy="42" rx="7" ry="12"/></g>
<g fill="#fff7dc"><circle cx="42" cy="36" r="4"/><circle cx="80" cy="36" r="4"/></g>
<path d="M37 80C48 90 80 90 91 80" stroke="#255332" stroke-width="5" fill="none" stroke-linecap="round"/>
</svg>"""

## Rasterizes inline SVG at twice its display size, then draws it at the
## display size, so icons stay crisp under the phone UI's canvas scaling.
static func svg_texture(svg: String, intrinsic_width: float, display: Vector2i, oversample: float = 2.0) -> ImageTexture:
	var image := Image.new()
	image.load_svg_from_string(svg, oversample * float(display.x) / intrinsic_width)
	var texture := ImageTexture.create_from_image(image)
	texture.set_size_override(display)
	return texture

static func outline_label(label: Label, outline: int, extrude: int = 0) -> void:
	label.add_theme_color_override("font_outline_color", Ink)
	label.add_theme_constant_override("outline_size", outline)
	if extrude > 0:
		# A solid ink shadow with the same outline reads as the key art's
		# extruded pixel lettering.
		label.add_theme_color_override("font_shadow_color", Ink)
		label.add_theme_constant_override("shadow_offset_x", 0)
		label.add_theme_constant_override("shadow_offset_y", extrude)
		label.add_theme_constant_override("shadow_outline_size", outline)

static func focus_ring(radius: int) -> StyleBoxFlat:
	var ring := StyleBoxFlat.new()
	ring.draw_center = false
	ring.border_color = Yellow
	ring.set_border_width_all(4)
	ring.set_expand_margin_all(5)
	ring.set_corner_radius_all(Radius + 3)
	ring.anti_aliasing = true
	return ring

## Candy normal/hover/pressed/disabled faces plus a yellow focus ring.
static func style_button(button: BaseButton, face: Color, radius: int = 14, margins: Vector4 = Vector4(18, 10, 18, 10), ledge: float = 6.0) -> void:
	var states: Dictionary = {
		"normal": CandyStyle.make(face, radius, ledge),
		"hover": CandyStyle.make(face.lightened(0.14), radius, ledge),
		"pressed": CandyStyle.make(face.darkened(0.06), radius, ledge, 3, true, true),
		"hover_pressed": CandyStyle.make(face.darkened(0.06), radius, ledge, 3, true, true),
		"disabled": CandyStyle.make(face.lerp(PanelDeep, 0.75), radius, ledge, 3, false),
	}
	for state in states:
		(states[state] as CandyStyle).set_margins(margins.x, margins.y, margins.z, margins.w)
		button.add_theme_stylebox_override(state, states[state])
	button.add_theme_stylebox_override("focus", focus_ring(radius))

static func bounce(control: Control, target: float, seconds: float = 0.16) -> void:
	if control.has_meta("ui_bounce"):
		var previous: Tween = control.get_meta("ui_bounce")
		if previous != null and previous.is_valid():
			previous.kill()
	var tween := control.create_tween()
	tween.tween_property(control, "scale", Vector2.ONE * target, seconds).set_trans(Tween.TRANS_BACK).set_ease(Tween.EASE_OUT)
	control.set_meta("ui_bounce", tween)

## Hover pop and press squash about the control's center. Scale never changes
## container layout, so this is purely visual.
static func juice(button: BaseButton) -> void:
	button.pivot_offset = button.size * 0.5
	button.resized.connect(func(): button.pivot_offset = button.size * 0.5)
	button.mouse_entered.connect(func(): if not button.disabled: bounce(button, 1.04))
	button.mouse_exited.connect(func(): bounce(button, 1.0))
	button.button_down.connect(func(): bounce(button, 0.94, 0.08))
	button.button_up.connect(func(): bounce(button, 1.04 if button.is_hovered() else 1.0))

static func pop_in(control: Control, from: float = 0.8, seconds: float = 0.3) -> void:
	if control.has_meta("ui_bounce"):
		var previous: Tween = control.get_meta("ui_bounce")
		if previous != null and previous.is_valid():
			previous.kill()
	control.pivot_offset = control.size * 0.5
	control.scale = Vector2.ONE * from
	control.modulate.a = 0.0
	var tween := control.create_tween().set_parallel(true)
	tween.tween_property(control, "scale", Vector2.ONE, seconds).set_trans(Tween.TRANS_BACK).set_ease(Tween.EASE_OUT)
	tween.tween_property(control, "modulate:a", 1.0, seconds * 0.4)
	control.set_meta("ui_bounce", tween)

## Theme shared by every HUD and menu control under the UI root.
static func build_theme(body_font: Font, font_size: int) -> Theme:
	var th := Theme.new()
	th.default_font = body_font
	th.default_font_size = font_size
	th.set_color("font_color", "Label", Cream)
	th.set_color("font_outline_color", "Label", Ink)
	th.set_constant("outline_size", "Label", 5)
	for type in ["Button", "CheckButton"]:
		for key in ["font_color", "font_hover_color", "font_pressed_color", "font_hover_pressed_color", "font_focus_color"]:
			th.set_color(key, type, Cream)
		th.set_color("font_disabled_color", type, Cream.darkened(0.45))
		th.set_color("font_outline_color", type, Ink)
		th.set_constant("outline_size", type, 6)
	var candy: Dictionary = {
		"normal": CandyStyle.make(Secondary, 12, 5),
		"hover": CandyStyle.make(Secondary.lightened(0.14), 12, 5),
		"pressed": CandyStyle.make(Secondary.darkened(0.06), 12, 5, 3, true, true),
		"hover_pressed": CandyStyle.make(Secondary.darkened(0.06), 12, 5, 3, true, true),
		"disabled": CandyStyle.make(Secondary.lerp(PanelDeep, 0.75), 12, 5, 3, false),
	}
	for state in candy:
		th.set_stylebox(state, "Button", (candy[state] as CandyStyle).set_margins(14, 8, 14, 8))
	th.set_stylebox("focus", "Button", focus_ring(12))
	# Option rows: dark wells with a candy switch on the right.
	var row := StyleBoxFlat.new()
	row.bg_color = PanelDeep
	row.border_color = PanelRim.darkened(0.35)
	row.set_border_width_all(2)
	row.set_corner_radius_all(Radius)
	row.content_margin_left = 14
	row.content_margin_right = 10
	row.content_margin_top = 6
	row.content_margin_bottom = 6
	var row_hover := row.duplicate() as StyleBoxFlat
	row_hover.bg_color = PanelDeep.lightened(0.10)
	for state in ["normal", "pressed", "disabled"]:
		th.set_stylebox(state, "CheckButton", row)
	for state in ["hover", "hover_pressed"]:
		th.set_stylebox(state, "CheckButton", row_hover)
	th.set_stylebox("focus", "CheckButton", focus_ring(10))
	var on := svg_texture(ToggleOn, 60.0, Vector2i(60, 34))
	var off := svg_texture(ToggleOff, 60.0, Vector2i(60, 34))
	th.set_icon("checked", "CheckButton", on)
	th.set_icon("unchecked", "CheckButton", off)
	th.set_icon("checked_disabled", "CheckButton", on)
	th.set_icon("unchecked_disabled", "CheckButton", off)
	th.set_icon("checked_mirrored", "CheckButton", on)
	th.set_icon("unchecked_mirrored", "CheckButton", off)
	# Slider: a dark rounded track filling with lime behind a cream knob.
	var track := StyleBoxFlat.new()
	track.bg_color = Color("123a80")
	track.border_color = Ink
	track.set_corner_radius_all(9)
	track.set_border_width_all(3)
	track.content_margin_top = 7
	track.content_margin_bottom = 7
	var fill := StyleBoxFlat.new()
	fill.bg_color = Lime
	fill.set_corner_radius_all(9)
	fill.border_color = Ink
	fill.set_border_width_all(3)
	fill.content_margin_top = 7
	fill.content_margin_bottom = 7
	th.set_stylebox("slider", "HSlider", track)
	th.set_stylebox("grabber_area", "HSlider", fill)
	th.set_stylebox("grabber_area_highlight", "HSlider", fill)
	th.set_icon("grabber", "HSlider", svg_texture(Knob % "#fff3bd", 30.0, Vector2i(30, 30)))
	th.set_icon("grabber_highlight", "HSlider", svg_texture(Knob % "#ffdd00", 30.0, Vector2i(30, 30)))
	th.set_icon("grabber_disabled", "HSlider", svg_texture(Knob % "#7e9ccc", 30.0, Vector2i(30, 30)))
	# Scroll bar: slim rounded lane with a bright thumb.
	var lane := StyleBoxFlat.new()
	lane.bg_color = PanelDeep
	lane.content_margin_left = 4
	lane.content_margin_right = 4
	var thumb := StyleBoxFlat.new()
	thumb.bg_color = PanelRim
	thumb.border_color = Ink
	thumb.set_border_width_all(2)
	var thumb_hot := thumb.duplicate() as StyleBoxFlat
	thumb_hot.bg_color = Yellow
	th.set_stylebox("scroll", "VScrollBar", lane)
	th.set_stylebox("scroll_focus", "VScrollBar", lane)
	th.set_stylebox("grabber", "VScrollBar", thumb)
	th.set_stylebox("grabber_highlight", "VScrollBar", thumb_hot)
	th.set_stylebox("grabber_pressed", "VScrollBar", thumb_hot)
	var tip := StyleBoxFlat.new()
	tip.bg_color = Ink
	tip.border_color = PanelRim
	tip.set_border_width_all(2)
	tip.set_content_margin_all(8)
	th.set_stylebox("panel", "TooltipPanel", tip)
	th.set_color("font_color", "TooltipLabel", Cream)
	return th
