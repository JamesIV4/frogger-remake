#include "ArcadeSimulationExtension.h"

namespace godot {

void ArcadeSimulation::_bind_methods() {
    ClassDB::bind_method(D_METHOD("setup", "rom", "modern", "sound_rom"), &ArcadeSimulation::setup, DEFVAL(false), DEFVAL(PackedByteArray()));
    ClassDB::bind_method(D_METHOD("step", "buttons"), &ArcadeSimulation::step, DEFVAL(0));
    ClassDB::bind_method(D_METHOD("peek", "address"), &ArcadeSimulation::peek);
    ClassDB::bind_method(D_METHOD("poke", "address", "value"), &ArcadeSimulation::poke);
    ClassDB::bind_method(D_METHOD("modern", "enabled"), &ArcadeSimulation::modern);
    ClassDB::bind_method(D_METHOD("get_frame"), &ArcadeSimulation::get_frame);
    ClassDB::bind_method(D_METHOD("snapshot"), &ArcadeSimulation::snapshot);
    ClassDB::bind_method(D_METHOD("get_sound_sample_count"), &ArcadeSimulation::get_sound_sample_count);
    ClassDB::bind_method(D_METHOD("get_sound_samples", "count"), &ArcadeSimulation::get_sound_samples);
    ClassDB::bind_method(D_METHOD("get_stereo_sound_samples", "count"), &ArcadeSimulation::get_stereo_sound_samples);
    ClassDB::bind_method(D_METHOD("clear_sound_samples"), &ArcadeSimulation::clear_sound_samples);
    ClassDB::bind_method(D_METHOD("get_bonus_awards"), &ArcadeSimulation::get_bonus_awards);
    ClassDB::bind_method(D_METHOD("state_array"), &ArcadeSimulation::state_array);
    ClassDB::bind_method(D_METHOD("state_bytes"), &ArcadeSimulation::state_bytes);
}

void ArcadeSimulation::setup(const PackedByteArray& rom, bool modern, const PackedByteArray& soundRom) {
    std::vector<uint8_t> rom_vec(rom.ptr(), rom.ptr() + rom.size());
    std::vector<uint8_t> sound_rom_vec;
    if (soundRom.size() > 0) {
        sound_rom_vec.assign(soundRom.ptr(), soundRom.ptr() + soundRom.size());
    }
    core = std::make_unique<ArcadeSimulationCore>(rom_vec, modern, sound_rom_vec);
}

void ArcadeSimulation::step(int buttons) {
    if (core) core->Step(buttons);
}

int ArcadeSimulation::peek(int address) {
    return core ? core->Peek(address) : 255;
}

void ArcadeSimulation::poke(int address, int value) {
    if (core) core->Poke(address, value);
}

void ArcadeSimulation::modern(bool enabled) {
    if (core) core->Modern(enabled);
}

int ArcadeSimulation::get_frame() const {
    return core ? core->Frame : 0;
}

Dictionary ArcadeSimulation::snapshot() const {
    Dictionary dict;
    if (!core) return dict;
    dict["frame"] = core->Frame;

    PackedByteArray ram_arr;
    ram_arr.resize(2048);
    memcpy(ram_arr.ptrw(), core->Bus.Ram, 2048);
    dict["ram"] = ram_arr;

    PackedByteArray video_arr;
    video_arr.resize(1024);
    memcpy(video_arr.ptrw(), core->Bus.Video, 1024);
    dict["video"] = video_arr;

    PackedByteArray obj_arr;
    obj_arr.resize(256);
    memcpy(obj_arr.ptrw(), core->Bus.Objects, 256);
    dict["objects"] = obj_arr;

    Array sounds_arr;
    for (int s : core->Bus.Sounds) {
        sounds_arr.append(s);
    }
    const_cast<ArcadeSimulationCore*>(core.get())->Bus.Sounds.clear();
    dict["sounds"] = sounds_arr;
    return dict;
}

int ArcadeSimulation::get_sound_sample_count() const {
    return (core && core->Sound) ? (int)core->Sound->Samples.size() : 0;
}

PackedFloat32Array ArcadeSimulation::get_sound_samples(int count) {
    PackedFloat32Array arr;
    if (!core || !core->Sound) return arr;
    if (core->Sound->Samples.size() > 2400) {
        size_t excess = core->Sound->Samples.size() - 2400;
        core->Sound->Samples.erase(core->Sound->Samples.begin(), core->Sound->Samples.begin() + excess);
    }
    int n = std::min(count, (int)core->Sound->Samples.size());
    if (n <= 0) return arr;
    arr.resize(n);
    float* w = arr.ptrw();
    for (int i = 0; i < n; i++) {
        w[i] = core->Sound->Samples.front();
        core->Sound->Samples.pop_front();
    }
    return arr;
}

PackedVector2Array ArcadeSimulation::get_stereo_sound_samples(int count) {
    PackedVector2Array arr;
    if (!core || !core->Sound) return arr;
    if (core->Sound->Samples.size() > 2400) {
        size_t excess = core->Sound->Samples.size() - 2400;
        core->Sound->Samples.erase(core->Sound->Samples.begin(), core->Sound->Samples.begin() + excess);
    }
    int n = std::min(count, (int)core->Sound->Samples.size());
    if (n <= 0) return arr;
    arr.resize(n);
    Vector2* w = (Vector2*)arr.ptrw();
    for (int i = 0; i < n; i++) {
        float v = core->Sound->Samples.front();
        core->Sound->Samples.pop_front();
        w[i] = Vector2(v, v);
    }
    return arr;
}

void ArcadeSimulation::clear_sound_samples() {
    if (core && core->Sound) {
        core->Sound->Samples.clear();
    }
}

Array ArcadeSimulation::get_bonus_awards() {
    Array arr;
    if (!core) return arr;
    for (const auto& a : core->BonusAwards) {
        Dictionary d;
        d["frame"] = a.Frame;
        d["x"] = a.X;
        d["row"] = a.Row;
        d["amount"] = a.Amount;
        d["kind"] = (int)a.Kind;
        arr.append(d);
    }
    core->BonusAwards.clear();
    return arr;
}

PackedByteArray ArcadeSimulation::state_array() const {
    PackedByteArray arr;
    if (!core) return arr;
    auto vec = core->StateArray();
    arr.resize(vec.size());
    memcpy(arr.ptrw(), vec.data(), vec.size());
    return arr;
}

String ArcadeSimulation::state_bytes() const {
    if (!core) return "[]";
    return String(core->StateBytes().c_str());
}

} // namespace godot
