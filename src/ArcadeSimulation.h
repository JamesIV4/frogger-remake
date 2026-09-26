#pragma once
#include "Z80Program.h"
#include "Generated/MainProgram.h"
#include "NativeSound.h"
#include <vector>
#include <queue>
#include <string>
#include <memory>
#include <functional>

enum class BonusKind { Bug = 0, Rescue = 1, Time = 2 };
enum class RiverGatorZone { Outside = 0, Back = 1, Snout = 2 };

struct BonusAward {
    int Frame;
    int X;
    int Row;
    int Amount;
    BonusKind Kind;
};

struct ModelFootprint {
    double MinAlongX;
    double MaxAlongX;
    double AcrossRow;
};

class Ppi {
public:
    int control = 0x9b;
    int latch[3] = {};
    std::function<int(int)> input;
    std::function<void(int, int)> output;

    Ppi(std::function<int(int)> in_fn, std::function<void(int, int)> out_fn);
    int Read(int p);
    void Write(int p, int v);
};

class FroggerBus : public IArcadeBus {
public:
    std::vector<uint8_t> Rom;
    uint8_t Ram[2048] = {};
    uint8_t Video[1024] = {};
    uint8_t Objects[256] = {};
    std::vector<int> Sounds;
    bool NmiEnabled = false, FlipX = false, FlipY = false;
    int Input = 0, SoundData = 0, SoundControl = 0;
    std::function<void(int)> SoundCommand;
    Ppi inputPpi;
    Ppi soundPpi;

    explicit FroggerBus(const std::vector<uint8_t>& rom);
    int Read(int a) override;
    void Write(int a, int value) override;
    int In(int port) override;
    void Out(int port, int value) override;
};

struct FrameState {
    int frame = 0;
    std::vector<int> ram = std::vector<int>(2048, 0);
    std::vector<int> video = std::vector<int>(1024, 0);
    std::vector<int> objects = std::vector<int>(256, 0);
    std::vector<int> sounds;
    int At(int a) const {
        return a >= 0xa800 ? video[(a - 0xa800) & 1023] : ram[(a - 0x8000) & 2047];
    }
    int BcdScore(int a) const {
        return 10 * ((At(a) & 15) + 10 * (At(a) >> 4) + 100 * (At(a + 1) & 15) + 1000 * (At(a + 1) >> 4));
    }
};

class ArcadeSimulationCore {
public:
    static constexpr double FrameSeconds = 33.0 / 2000.0;
    static constexpr int CyclesPerFrame = 50688;

    FroggerBus Bus;
    MainProgram Cpu;
    std::unique_ptr<NativeSound> Sound;
    bool ModernCollision = false;
    std::vector<BonusAward> BonusAwards;
    int Frame = 0;

private:
    int64_t nextNmi = 0;
    int previousX = 0, previousY = 0;
    uint8_t previousVehicles[2048] = {};
    int previousVehicleCounts[5] = {};
    int homeAwardX = 0, homeAwardRow = 0;
    bool homeAwardOrigin = false;

public:
    ArcadeSimulationCore(const std::vector<uint8_t>& rom, bool modern = false, const std::vector<uint8_t>& soundRom = {});
    ~ArcadeSimulationCore() = default;

    void Step(int buttons = 0);
    void Modern(bool enabled) { ModernCollision = enabled; }
    int Peek(int address) { return Bus.Read(address); }
    void Poke(int address, int value) { Bus.Write(address, value); }

    FrameState Snapshot();
    std::vector<uint8_t> StateArray() const;
    std::string StateBytes() const;

    static bool ModelContact(double frogFromX, double frogFromRow, double frogToX, double frogToRow,
                             double vehicleFromX, double vehicleToX, double vehicleRow, ModelFootprint vehicle);
    static bool SweptBox(double x0, double y0, double x1, double y1, double cx, double cy, double rx, double ry);
    static RiverGatorZone RiverGatorContact(int frogX, int tipX);

    bool ResolveModernRiverGator();

private:
    void ObserveBonusAward();
    bool ResolveModernRoad();
};
