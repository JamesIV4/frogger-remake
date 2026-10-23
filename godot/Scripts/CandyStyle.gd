class_name CandyStyle
extends StyleBox

## A bubbly candy face for buttons, cards and panels: rounded corners, a thick
## ink outline, an ink ledge underneath that the face sinks into while pressed,
## a darker lower bevel and a glossy highlight on bright faces.
## Content margins already include the ledge, so text stays centered on the
## visible face in every state.

var ledge: float = 6.0
var outline: int = 3
var bevel: float = 4.0
var sink: float = 0.0
var shine: bool = true
var rim: bool = false
var ledge_box := StyleBoxFlat.new()
var body_box := StyleBoxFlat.new()
var face_box := StyleBoxFlat.new()
var shine_box := StyleBoxFlat.new()
var rim_box := StyleBoxFlat.new()

static func make(face: Color, radius: int = 14, ledge_px: float = 6.0, outline_px: int = 3, glossy: bool = true, pressed: bool = false) -> CandyStyle:
	var s := CandyStyle.new()
	s.ledge = ledge_px
	s.outline = outline_px
	# Gloss only reads as shine on a bright face.
	s.shine = glossy and face.get_luminance() > 0.3
	s.sink = 1.0 if pressed else 0.0
	s.ledge_box.bg_color = UiKit.Ink
	s.body_box.bg_color = face.darkened(0.28)
	s.body_box.border_color = UiKit.Ink
	s.body_box.set_border_width_all(outline_px)
	s.face_box.bg_color = face
	s.shine_box.bg_color = Color(1, 1, 1, 0.30)
	s.rim_box.draw_center = false
	s.rim_box.border_color = face.lightened(0.28)
	s.rim_box.set_border_width_all(2)
	for box in [s.ledge_box, s.body_box, s.face_box, s.shine_box, s.rim_box]:
		box.anti_aliasing = true
	s.ledge_box.set_corner_radius_all(radius)
	s.body_box.set_corner_radius_all(radius)
	s.face_box.set_corner_radius_all(maxi(2, radius - outline_px))
	s.shine_box.set_corner_radius_all(maxi(2, radius - outline_px - 3))
	s.rim_box.set_corner_radius_all(maxi(2, radius - outline_px - 2))
	s.set_margins(18, 10, 18, 10)
	return s

## Text margins around the visible face; the ledge is added below it, and a
## pressed face moves the text down with it.
func set_margins(left: float, top: float, right: float, bottom: float) -> CandyStyle:
	var drop: float = ledge * sink
	content_margin_left = left
	content_margin_right = right
	content_margin_top = top + drop
	content_margin_bottom = bottom + ledge - drop
	return self

func _draw(to_canvas_item: RID, rect: Rect2) -> void:
	var drop: float = ledge * sink
	var body := Rect2(rect.position + Vector2(0, drop), Vector2(rect.size.x, maxf(1.0, rect.size.y - ledge)))
	if ledge - drop > 0.5:
		ledge_box.draw(to_canvas_item, Rect2(rect.position + Vector2(0, ledge), body.size))
	body_box.draw(to_canvas_item, body)
	var inset: float = float(outline)
	var face := body.grow_individual(-inset, -inset, -inset, -inset - bevel)
	if face.size.x <= 0.0 or face.size.y <= 0.0:
		return
	face_box.draw(to_canvas_item, face)
	if rim:
		rim_box.draw(to_canvas_item, face.grow(-2.0))
	if shine and face.size.y > 14.0:
		shine_box.draw(to_canvas_item, Rect2(face.position + Vector2(8, 3), Vector2(maxf(1.0, face.size.x - 16), minf(face.size.y * 0.34, 12.0))))
