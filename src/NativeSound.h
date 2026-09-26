#pragma once
#include "Z80Program.h"
#include "Generated/SoundProgram.h"
#include <vector>
#include <deque>
#include <cmath>
#include <algorithm>
#include <cstdint>

struct SoundWrite {
    int64_t cycle;
    int reg;
    int value;
};

class NativeSound : public IArcadeBus {
public:
    static constexpr double Clock = 14318181.0 / 8.0;

    SoundProgram Cpu;
    std::vector<uint8_t> Rom;
    uint8_t Ram[1024] = {};
    int Registers[16] = {};
    std::deque<float> Samples;
    std::vector<SoundWrite> Writes;
    bool RecordWrites = false;

private:
    double tone[3] = {};
    int64_t sampleNumber = 0;
    int selected = 0;
    int latch = 0;
    int lfsr = 1;
    int envelopeStep = 15;
    int envelopeDirection = -1;
    double noisePhase = 0.0;
    double envelopePhase = 0.0;
    double filtered = 0.0;
    double previous = 0.0;
    bool irq = false;
    bool envelopeHeld = false;

public:
    explicit NativeSound(const std::vector<uint8_t>& data);
    ~NativeSound() override = default;

    void Command(int value);
    void AdvanceTo(double cycles);

    int Read(int address) override;
    void Write(int address, int value) override;
    int In(int port) override;
    void Out(int port, int value) override;

    float Synthesize();
};
