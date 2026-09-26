extends SceneTree

# Run: Godot --headless --audio-driver WASAPI --path godot --script ../tests/audio_feed.gd
# Omit --audio-driver on systems without WASAPI (Dummy may report retired
# playbacks at exit because it does not mix).
# Exercise the real generator and feed policy without loading the game world.
class AudioGame extends "res://Scripts/FroggerGame.gd":
	func _ready() -> void:
		pass
	func _process(_delta: float) -> void:
		pass

class SoundSource extends RefCounted:
	var count: int = 0
	var requested: int = 0
	func get_sound_sample_count() -> int:
		return count
	func clear_sound_samples() -> void:
		count = 0
	func get_stereo_sound_samples(amount: int) -> PackedVector2Array:
		requested = amount
		var samples := PackedVector2Array()
		samples.resize(mini(amount, count))
		count -= samples.size()
		return samples

var failures: int = 0

func check(condition: bool, message: String) -> void:
	if not condition:
		push_error(message)
		failures += 1

func _initialize() -> void:
	call_deferred("run")

func run() -> void:
	var game := AudioGame.new()
	root.add_child(game)
	var source := SoundSource.new()
	game.simulation = source
	game.started = true
	source.count = 792
	game.feed_audio()
	check(source.count == 0, "A normal simulation frame must be drained")
	check(game.audio_player.playing, "Gameplay must start stream playback")
	check(game.audio_capacity <= 4800, "Generator capacity must stay below 100 ms")
	var previous_playback := game.audio_playback
	# Deliberately exceed both the generator capacity and the stale threshold.
	# This also covers a producer delivering more than the native 50 ms cap.
	source.count = 3000
	game.feed_audio()
	check(game.audio_playback == previous_playback, "Stale flush must reuse playback while output is suspended")
	check(source.requested == 3000 and source.count == 0, "Overflow must drain the whole producer queue")
	check(game.audio_playback.get_frames_available() >= 0, "Overflow must fit the generator")
	for frame in range(100):
		source.count = 3000
		game.feed_audio()
		check(game.audio_playback == previous_playback and source.count == 0,
			"Repeated stalls must keep one playback and drain samples (frame %d)" % frame)
	for gate in ["muted", "paused", "started"]:
		game.set(gate, gate != "started")
		source.count = 792
		game.feed_audio()
		check(source.count == 0 and not game.audio_player.playing, "%s must clear and stop audio" % gate)
		await create_timer(0.04).timeout
		game.set(gate, gate == "started")
		game.feed_audio()
		check(game.audio_player.playing, "Resuming after %s must restart audio" % gate)
		await create_timer(0.04).timeout
	game.free()
	# The audio server releases stopped playbacks during its next mix.
	await create_timer(0.1).timeout
	print("Audio feed: normal playback, stale restart, overflow drain, mute/pause/menu gates checked")
	quit(1 if failures else 0)
