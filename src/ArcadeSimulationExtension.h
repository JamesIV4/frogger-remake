#pragma once
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>
#include "ArcadeSimulation.h"
#include <memory>

namespace godot {

class ArcadeSimulation : public RefCounted {
    GDCLASS(ArcadeSimulation, RefCounted)

private:
    std::unique_ptr<ArcadeSimulationCore> core;

protected:
    static void _bind_methods();

public:
    ArcadeSimulation() = default;
    ~ArcadeSimulation() override = default;

    void setup(const PackedByteArray& rom, bool modern = false, const PackedByteArray& soundRom = PackedByteArray());
    void step(int buttons = 0);
    int peek(int address);
    void poke(int address, int value);
    void modern(bool enabled);
    int get_frame() const;
    Dictionary snapshot() const;
    int get_sound_sample_count() const;
    PackedFloat32Array get_sound_samples(int count);
    PackedVector2Array get_stereo_sound_samples(int count);
    void clear_sound_samples();
    Array get_bonus_awards();
    PackedByteArray state_array() const;
    String state_bytes() const;
};

} // namespace godot
