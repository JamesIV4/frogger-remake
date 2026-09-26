#include "ArcadeSimulation.h"
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>

static inline ModelFootprint GetVehicleFootprint(int lane) {
    switch (lane) {
        case 6: return {-13.09056, 13.30560, 6.07488};
        case 7: return {-7.79520, 8.29920, 6.07488};
        case 8: return {-8.29920, 7.79520, 6.07488};
        case 9: return {-6.76704, 8.66880, 6.24960};
        case 10: return {-8.87040, 6.76704, 6.07488};
        default: return {0.0, 0.0, 0.0};
    }
}

static constexpr double FrogAlongX = 8.56170;
static constexpr double FrogAcrossRow = 6.89952;

Ppi::Ppi(std::function<int(int)> in_fn, std::function<void(int, int)> out_fn)
    : input(std::move(in_fn)), output(std::move(out_fn)) {}

int Ppi::Read(int p) {
    switch (p) {
        case 0: return (control & 16) != 0 ? input(0) : latch[0];
        case 1: return (control & 2) != 0 ? input(1) : latch[1];
        case 2: return (((control & 8) != 0 ? input(2) : latch[2]) & 240) |
                       (((control & 1) != 0 ? input(2) : latch[2]) & 15);
        default: return control;
    }
}

void Ppi::Write(int p, int v) {
    if (p == 3) {
        if ((v & 128) != 0) {
            control = v;
            latch[0] = latch[1] = latch[2] = 0;
            if ((control & 16) == 0) output(0, 0);
            if ((control & 2) == 0) output(1, 0);
            output(2, 0);
        } else {
            int bit = (v >> 1) & 7;
            latch[2] = (latch[2] & ~(1 << bit)) | ((v & 1) << bit);
            output(2, latch[2]);
        }
    } else if (p == 2 || (p == 0 ? (control & 16) == 0 : (control & 2) == 0)) {
        latch[p] = v;
        output(p, v);
    }
}

FroggerBus::FroggerBus(const std::vector<uint8_t>& rom)
    : Rom(rom),
      inputPpi([this](int p) {
          switch (p) {
              case 0: return 255 & ~(((Input & 4) != 0 ? 32 : 0) | ((Input & 8) != 0 ? 16 : 0) | ((Input & 16) != 0 ? 128 : 0));
              case 1: return 252 & ~(((Input & 32) != 0 ? 128 : 0) | ((Input & 64) != 0 ? 64 : 0));
              default: return 241 & ~(((Input & 1) != 0 ? 16 : 0) | ((Input & 2) != 0 ? 64 : 0));
          }
      }, [](int, int) {}),
      soundPpi([](int p) { return p == 2 ? 0 : 255; }, [this](int p, int v) {
          if (p == 0) SoundData = v;
          if (p == 1) {
              if ((SoundControl & 8) != 0 && (v & 8) == 0) {
                  Sounds.push_back(SoundData);
                  if (SoundCommand) SoundCommand(SoundData);
              }
              SoundControl = v;
          }
      }) {}

int FroggerBus::Read(int a) {
    a &= 65535;
    if (a < 0x4000) return a < (int)Rom.size() ? Rom[a] : 255;
    if (a >= 0x8000 && a < 0x8800) return Ram[a - 0x8000];
    if ((a & 0xf800) == 0xa800) return Video[a & 1023];
    if ((a & 0xf800) == 0xb000) return Objects[a & 255];
    if (a >= 0xc000) {
        int v = 255, p = (a >> 1) & 3;
        if ((a & 0x1000) != 0) v &= soundPpi.Read(p);
        if ((a & 0x2000) != 0) v &= inputPpi.Read(p);
        return v;
    }
    return 255;
}

void FroggerBus::Write(int a, int value) {
    a &= 65535;
    uint8_t v = (uint8_t)value;
    if (a < 0x4000) return; // ROM
    if (a >= 0x8000 && a < 0x8800) { Ram[a - 0x8000] = v; return; }
    if ((a & 0xf800) == 0xa800) { Video[a & 1023] = v; return; }
    if ((a & 0xf800) == 0xb000) { Objects[a & 255] = v; return; }
    switch (a & 0xf81c) {
        case 0xb808: NmiEnabled = (v & 1) != 0; return;
        case 0xb80c: FlipY = (v & 1) != 0; return;
        case 0xb810: FlipX = (v & 1) != 0; return;
    }
    if (a >= 0xc000) {
        int p = (a >> 1) & 3;
        if ((a & 0x1000) != 0) soundPpi.Write(p, v);
        if ((a & 0x2000) != 0) inputPpi.Write(p, v);
    }
}

int FroggerBus::In(int port) { return 255; }
void FroggerBus::Out(int port, int value) {}

ArcadeSimulationCore::ArcadeSimulationCore(const std::vector<uint8_t>& rom, bool modern, const std::vector<uint8_t>& soundRom)
    : Bus(rom), Cpu(&Bus), ModernCollision(modern) {
    if (!soundRom.empty()) {
        Sound = std::make_unique<NativeSound>(soundRom);
        Bus.SoundCommand = [this](int value) {
            if (Sound) {
                Sound->AdvanceTo((double)Cpu.Cycles * NativeSound::Clock / 3072000.0);
                Sound->Command(value);
            }
        };
    }
}

void ArcadeSimulationCore::Step(int buttons) {
    Bus.Input = buttons;
    int64_t target = (Frame + 1LL) * CyclesPerFrame;
    while (Cpu.Cycles < target) {
        if (Cpu.Cycles >= nextNmi) {
            nextNmi += CyclesPerFrame;
            previousX = Peek(0x8044);
            previousY = Peek(0x8047);
            for (int lane = 6; lane <= 10; lane++) {
                int table = 0x8100 + lane * 9;
                int count = std::min(8, Peek(table));
                previousVehicleCounts[lane - 6] = count;
                for (int index = 0; index < count; index++) {
                    previousVehicles[table + index + 1 - 0x8000] = (uint8_t)Peek(table + index + 1);
                }
            }
            if (Bus.NmiEnabled) Cpu.Nmi();
        }
        if (Cpu.PC == 0x1f1c) {
            homeAwardX = Peek(0x8044);
            homeAwardRow = Peek(0x8047);
            homeAwardOrigin = true;
        }
        if (Cpu.PC == 0x08e0) ObserveBonusAward();
        if (ModernCollision && Cpu.PC == 0x28bb && ResolveModernRiverGator()) {
            Cpu.ReturnFromHook();
        } else if (ModernCollision && Cpu.PC == 0x11bf && ResolveModernRoad()) {
            Cpu.ReturnFromHook();
        } else {
            Cpu.StepInstruction();
        }
    }
    Frame++;
    if (Sound) {
        Sound->AdvanceTo((double)Cpu.Cycles * NativeSound::Clock / 3072000.0);
    }
}

void ArcadeSimulationCore::ObserveBonusAward() {
    if (Peek(0x83fe) == 0) return;
    int caller = Peek(Cpu.SP) | (Peek((Cpu.SP + 1) & 65535) << 8);
    if (caller == 0x1f44) {
        int bcd = Cpu.DE;
        int points = 10 * ((bcd & 15) + 10 * ((bcd >> 4) & 15) + 100 * ((bcd >> 8) & 15) + 1000 * ((bcd >> 12) & 15));
        BonusAwards.push_back({Frame + 1, homeAwardOrigin ? homeAwardX : Peek(0x8044), homeAwardOrigin ? homeAwardRow : Peek(0x8047), points, BonusKind::Time});
        homeAwardOrigin = false;
        return;
    }
    if (Cpu.DE != 0x20) return;
    if (caller != 0x2692 && caller != 0x1f28) return;
    BonusAwards.push_back({Frame + 1, Peek(0x8044), Peek(0x8047), 200, caller == 0x2692 ? BonusKind::Bug : BonusKind::Rescue});
}

RiverGatorZone ArcadeSimulationCore::RiverGatorContact(int frogX, int tipX) {
    int behind = (tipX - frogX + 256) & 255;
    return behind < 16 ? RiverGatorZone::Snout :
           (behind <= 57 ? RiverGatorZone::Back : RiverGatorZone::Outside);
}

bool ArcadeSimulationCore::ResolveModernRiverGator() {
    if (Peek(0x83b7) < 2 || (Peek(0x8150) & 1) == 0 || Peek(0x8101) == 0) return false;
    int row = Peek(0x8047), biased = (row + 8) & 255;
    if (biased < 42 || biased >= 59) return false;
    if (Peek(0x8004) != 0 && Peek(0x829c) != 0) return true;
    auto zone = RiverGatorContact(Peek(0x8044), Peek(0x8101));
    if (zone == RiverGatorZone::Snout) {
        Poke(0x8004, 1);
        if (row >= 48 && row < 128) Poke(0x829c, 1);
    } else if (zone == RiverGatorZone::Back) {
        Poke(0x8004, 1);
        Poke(0xa846, 0x68); Poke(0xa847, 0x69);
        Poke(0xa866, 0x6a); Poke(0xa867, 0x6b);
    }
    return true;
}

bool ArcadeSimulationCore::ResolveModernRoad() {
    int y = Peek(0x8047), x = Peek(0x8044);
    if (y < 136 || y > 216) return false;
    if (Peek(0x83cd) != 0 || Peek(0x8004) != 0) return true;
    for (int lane = 6; lane <= 10; lane++) {
        int width = lane == 6 ? 34 : 18;
        int table = 0x8100 + lane * 9;
        int row = (lane + 3) * 16;
        auto body = GetVehicleFootprint(lane);
        int count = std::min(8, Peek(table));
        for (int i = 0; i < count; i++) {
            int address = table + i + 1;
            int raw = Peek(address);
            double current = (double)raw - 3.0 - (double)width / 2.0;
            double prior = current;
            if (i < previousVehicleCounts[lane - 6]) {
                int old = previousVehicles[address - 0x8000];
                int delta = ((raw - old + 128) & 255) - 128;
                prior = current - (double)delta;
            }
            for (int wrap = -256; wrap <= 256; wrap += 256) {
                if (ModelContact((double)previousX, (double)previousY, (double)x, (double)y,
                                 prior + (double)wrap, current + (double)wrap, (double)row, body)) {
                    Poke(0x8004, 1);
                    return true;
                }
            }
        }
    }
    return true;
}

bool ArcadeSimulationCore::ModelContact(double frogFromX, double frogFromRow, double frogToX, double frogToRow,
                                        double vehicleFromX, double vehicleToX, double vehicleRow, ModelFootprint vehicle) {
    double center = (vehicle.MinAlongX + vehicle.MaxAlongX) / 2.0;
    double halfWidth = (vehicle.MaxAlongX - vehicle.MinAlongX) / 2.0 + FrogAlongX;
    return SweptBox(frogFromX - vehicleFromX, frogFromRow - vehicleRow,
                    frogToX - vehicleToX, frogToRow - vehicleRow, center, 0.0,
                    halfWidth, FrogAcrossRow + vehicle.AcrossRow);
}

bool ArcadeSimulationCore::SweptBox(double x0, double y0, double x1, double y1, double cx, double cy, double rx, double ry) {
    double near_t = 0.0, far_t = 1.0;
    struct Axis { double p, d, c, r; };
    Axis axes[2] = { {x0, x1 - x0, cx, rx}, {y0, y1 - y0, cy, ry} };
    for (int i = 0; i < 2; i++) {
        double p = axes[i].p, d = axes[i].d, c = axes[i].c, r = axes[i].r;
        if (std::abs(d) < 1e-9) {
            if (p < c - r || p > c + r) return false;
        } else {
            double a = (c - r - p) / d;
            double b = (c + r - p) / d;
            if (a > b) std::swap(a, b);
            near_t = std::max(near_t, a);
            far_t = std::min(far_t, b);
            if (near_t > far_t) return false;
        }
    }
    return true;
}

std::vector<uint8_t> ArcadeSimulationCore::StateArray() const {
    std::vector<uint8_t> state;
    state.reserve(2048 + 1024 + 256);
    state.insert(state.end(), Bus.Ram, Bus.Ram + 2048);
    state.insert(state.end(), Bus.Video, Bus.Video + 1024);
    state.insert(state.end(), Bus.Objects, Bus.Objects + 256);
    return state;
}

FrameState ArcadeSimulationCore::Snapshot() {
    FrameState s;
    s.frame = Frame;
    for (int i = 0; i < 2048; i++) s.ram[i] = Bus.Ram[i];
    for (int i = 0; i < 1024; i++) s.video[i] = Bus.Video[i];
    for (int i = 0; i < 256; i++) s.objects[i] = Bus.Objects[i];
    s.sounds = Bus.Sounds;
    Bus.Sounds.clear();
    return s;
}

std::string ArcadeSimulationCore::StateBytes() const {
    auto state = StateArray();
    std::ostringstream ss;
    ss << "[";
    for (size_t i = 0; i < state.size(); i++) {
        if (i > 0) ss << ",";
        ss << (int)state[i];
    }
    ss << "]";
    return ss.str();
}

