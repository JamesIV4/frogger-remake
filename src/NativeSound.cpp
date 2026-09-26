#include "NativeSound.h"

NativeSound::NativeSound(const std::vector<uint8_t>& data) : Cpu(this), Rom(data) {
    lfsr = 1;
    envelopeStep = 15;
    envelopeDirection = -1;
}

void NativeSound::Command(int value) {
    latch = value;
    irq = true;
}

static const double VolumeTable[16] = {
    7.4989420933245579e-02, // 0
    8.9125093813374551e-02, // 1
    1.0592537251772889e-01, // 2
    1.2589254117941673e-01, // 3
    1.4962356560944334e-01, // 4
    1.7782794100389229e-01, // 5
    2.1134890398366465e-01, // 6
    2.5118864315095801e-01, // 7
    2.9853826189179594e-01, // 8
    3.5481338923357547e-01, // 9
    4.2169650342858223e-01, // 10
    5.0118723362727224e-01, // 11
    5.9566214352901048e-01, // 12
    7.0794578438413791e-01, // 13
    8.4139514164519513e-01, // 14
    1.0000000000000000e+00  // 15
};

void NativeSound::AdvanceTo(double cycles) {
    int64_t end = (int64_t)(cycles * 48000.0 / Clock);
    while (sampleNumber < end) {
        double target = (double)(sampleNumber + 1) * Clock / 48000.0;
        while (Cpu.Cycles < target) {
            if (irq && Cpu.Irq()) {
                irq = false;
            }
            Cpu.StepInstruction();
        }
        Samples.push_back(Synthesize());
        sampleNumber++;
    }
    if (Samples.size() > 2400) {
        Samples.erase(Samples.begin(), Samples.end() - 2400);
    }
}

int NativeSound::Read(int address) {
    int a = address & 0x7fff;
    if (a < 0x2000) return a < (int)Rom.size() ? Rom[a] : 0;
    if (a >= 0x4000 && a < 0x6000) return Ram[a & 1023];
    return 255;
}

void NativeSound::Write(int address, int value) {
    int a = address & 0x7fff;
    if (a >= 0x4000 && a < 0x6000) Ram[a & 1023] = (uint8_t)value;
}

int NativeSound::In(int port) {
    if ((port & 64) == 0) return 255;
    if (selected == 14) return latch;
    if (selected == 15) {
        int64_t c = (Cpu.Cycles * 8) % 40960;
        int hi = 0;
        if (c >= 20480) { hi = 128; c -= 20480; }
        int v = hi | (int)((c >> 8) & 64) | (int)((c >> 8) & 32) | (int)((c >> 7) & 16) | 14;
        return (v & ~40) | ((v & 8) << 2) | ((v & 32) >> 2);
    }
    return Registers[selected];
}

void NativeSound::Out(int port, int value) {
    if ((port & 64) != 0) {
        Registers[selected] = value;
        if (RecordWrites) {
            Writes.push_back({Cpu.Cycles, selected, value});
        }
        if (selected == 13) {
            envelopeDirection = (value & 4) != 0 ? 1 : -1;
            envelopeStep = envelopeDirection == 1 ? 0 : 15;
            envelopePhase = 0;
            envelopeHeld = false;
        }
    } else if ((port & 128) != 0) {
        selected = value & 15;
    }
}

float NativeSound::Synthesize() {
    noisePhase += Clock / (16.0 * 48000.0 * (double)std::max(1, Registers[6] & 31));
    while (noisePhase >= 1.0) {
        noisePhase -= 1.0;
        int bit = (lfsr ^ (lfsr >> 3)) & 1;
        lfsr = (lfsr >> 1) | (bit << 16);
    }
    int period = Registers[11] | (Registers[12] << 8);
    envelopePhase += Clock / (256.0 * 48000.0 * (double)std::max(1, period));
    while (envelopePhase >= 1.0 && !envelopeHeld) {
        envelopePhase -= 1.0;
        envelopeStep += envelopeDirection;
        if (envelopeStep < 0 || envelopeStep > 15) {
            int shape = Registers[13];
            if ((shape & 8) == 0) {
                envelopeStep = 0;
                envelopeHeld = true;
            } else if ((shape & 1) != 0) {
                envelopeStep = (shape & 2) != 0 ? (envelopeDirection > 0 ? 0 : 15) : (envelopeDirection > 0 ? 15 : 0);
                envelopeHeld = true;
            } else {
                if ((shape & 2) != 0) envelopeDirection = -envelopeDirection;
                envelopeStep = envelopeDirection > 0 ? 0 : 15;
            }
        }
    }
    double sum = 0.0;
    for (int ch = 0; ch < 3; ch++) {
        int p = Registers[2 * ch] | ((Registers[2 * ch + 1] & 15) << 8);
        tone[ch] += Clock / (16.0 * 48000.0 * (double)std::max(1, p));
        if (tone[ch] >= 1.0) tone[ch] -= (double)(int64_t)tone[ch];
        bool gate = ((Registers[7] & (1 << ch)) != 0 || tone[ch] < 0.5) &&
                    ((Registers[7] & (8 << ch)) != 0 || (lfsr & 1) != 0);
        int volume = (Registers[8 + ch] & 16) != 0 ? envelopeStep : (Registers[8 + ch] & 15);
        if (gate && volume > 0) {
            sum += VolumeTable[std::clamp(volume, 0, 15)];
        }
    }
    double raw = sum * 0.23;
    double high = raw - previous + 0.995 * filtered;
    previous = raw;
    filtered = high;
    return (float)std::clamp(high, -1.0, 1.0);
}
