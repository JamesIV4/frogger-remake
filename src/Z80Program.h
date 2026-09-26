#pragma once
#include <cstdint>
#include <set>
#include <stdexcept>
#include <string>
#include <cstdio>
#include <algorithm>
#include <utility>

class IArcadeBus {
public:
    virtual ~IArcadeBus() = default;
    virtual int Read(int address) = 0;
    virtual void Write(int address, int value) = 0;
    virtual int In(int port) = 0;
    virtual void Out(int port, int value) = 0;
};

struct Reg8 {
    int val = 0;
    constexpr Reg8() : val(0) {}
    constexpr Reg8(int v) : val(v & 255) {}
    operator int() const { return val; }
    Reg8& operator=(int v) { val = v & 255; return *this; }
    Reg8& operator+=(int v) { val = (val + v) & 255; return *this; }
    Reg8& operator-=(int v) { val = (val - v) & 255; return *this; }
    Reg8& operator&=(int v) { val = (val & v) & 255; return *this; }
    Reg8& operator|=(int v) { val = (val | v) & 255; return *this; }
    Reg8& operator^=(int v) { val = (val ^ v) & 255; return *this; }
    Reg8& operator++() { val = (val + 1) & 255; return *this; }
    int operator++(int) { int old = val; val = (val + 1) & 255; return old; }
    Reg8& operator--() { val = (val - 1) & 255; return *this; }
    int operator--(int) { int old = val; val = (val - 1) & 255; return old; }
};

struct Reg16 {
    int val = 0;
    constexpr Reg16() : val(0) {}
    constexpr Reg16(int v) : val(v & 65535) {}
    operator int() const { return val; }
    Reg16& operator=(int v) { val = v & 65535; return *this; }
    Reg16& operator+=(int v) { val = (val + v) & 65535; return *this; }
    Reg16& operator-=(int v) { val = (val - v) & 65535; return *this; }
    Reg16& operator&=(int v) { val = (val & v) & 65535; return *this; }
    Reg16& operator|=(int v) { val = (val | v) & 65535; return *this; }
    Reg16& operator^=(int v) { val = (val ^ v) & 65535; return *this; }
    Reg16& operator++() { val = (val + 1) & 65535; return *this; }
    int operator++(int) { int old = val; val = (val + 1) & 65535; return old; }
    Reg16& operator--() { val = (val - 1) & 65535; return *this; }
    int operator--(int) { int old = val; val = (val - 1) & 65535; return old; }
};

struct PairReg {
    Reg8& hi;
    Reg8& lo;
    PairReg(Reg8& h, Reg8& l) : hi(h), lo(l) {}
    operator int() const { return (hi.val << 8) | lo.val; }
    PairReg& operator=(int v) { hi = v >> 8; lo = v; return *this; }
    PairReg& operator+=(int v) { return *this = (*this + v) & 65535; }
    PairReg& operator-=(int v) { return *this = (*this - v) & 65535; }
    PairReg& operator++() { return *this = (*this + 1) & 65535; }
    int operator++(int) { int old = *this; *this = (*this + 1) & 65535; return old; }
    PairReg& operator--() { return *this = (*this - 1) & 65535; }
    int operator--(int) { int old = *this; *this = (*this - 1) & 65535; return old; }
};

namespace std {
    inline void swap(PairReg a, PairReg b) {
        int temp = a;
        a = (int)b;
        b = temp;
    }
    inline void swap(PairReg a, int& b) {
        int temp = a;
        a = b;
        b = temp;
    }
    inline void swap(int& b, PairReg a) {
        int temp = b;
        b = a;
        a = temp;
    }
}

struct HalfRegH {
    Reg16& reg;
    HalfRegH(Reg16& r) : reg(r) {}
    operator int() const { return reg.val >> 8; }
    HalfRegH& operator=(int v) { reg.val = (reg.val & 255) | ((v & 255) << 8); return *this; }
    HalfRegH& operator+=(int v) { return *this = (*this + v) & 255; }
    HalfRegH& operator-=(int v) { return *this = (*this - v) & 255; }
    HalfRegH& operator++() { return *this = (*this + 1) & 255; }
    int operator++(int) { int old = *this; *this = (*this + 1) & 255; return old; }
    HalfRegH& operator--() { return *this = (*this - 1) & 255; }
    int operator--(int) { int old = *this; *this = (*this - 1) & 255; return old; }
};

struct HalfRegL {
    Reg16& reg;
    HalfRegL(Reg16& r) : reg(r) {}
    operator int() const { return reg.val & 255; }
    HalfRegL& operator=(int v) { reg.val = (reg.val & 65280) | (v & 255); return *this; }
    HalfRegL& operator+=(int v) { return *this = (*this + v) & 255; }
    HalfRegL& operator-=(int v) { return *this = (*this - v) & 255; }
    HalfRegL& operator++() { return *this = (*this + 1) & 255; }
    int operator++(int) { int old = *this; *this = (*this + 1) & 255; return old; }
    HalfRegL& operator--() { return *this = (*this - 1) & 255; }
    int operator--(int) { int old = *this; *this = (*this - 1) & 255; return old; }
};

class Z80Program {
public:
    IArcadeBus* Bus;
    int PC = 0, AFAlt = 0, BCAlt = 0, DEAlt = 0, HLAlt = 0, InterruptMode = 0, EiDelay = 0;
    bool Iff1 = false, Iff2 = false, Halted = false;
    int64_t Cycles = 0;
    bool Executed[65536] = {};
    std::set<int> IndirectTargets;

    Reg8 A, F{0x40}, B, C, D, E, H, L, I, R;
    Reg16 IX{65535}, IY{65535}, SP;
    PairReg AF{A, F}, BC{B, C}, DE{D, E}, HL{H, L};
    HalfRegH IXH{IX}, IYH{IY};
    HalfRegL IXL{IX}, IYL{IY};

    explicit Z80Program(IArcadeBus* bus) : Bus(bus) {}
    virtual ~Z80Program() = default;

    virtual int ExecuteInstruction() = 0;

    void StepInstruction() {
        if (Halted) { Cycles += 4; return; }
        Executed[PC] = true;
        R = (R & 128) | ((R + 1) & 127);
        Cycles += ExecuteInstruction();
        if (EiDelay > 0) EiDelay--;
    }

    void Nmi() {
        Halted = false;
        Iff2 = Iff1;
        Iff1 = false;
        Push(PC);
        PC = 0x66;
        Cycles += 11;
    }

    bool Irq() {
        if (!Iff1 || EiDelay > 0) return false;
        Halted = false;
        Iff1 = Iff2 = false;
        Push(PC);
        PC = InterruptMode == 2 ? Read16((I << 8) | 255) : 0x38;
        Cycles += 13;
        return true;
    }

    std::runtime_error Missing() {
        char buf[64];
        snprintf(buf, sizeof(buf), "Unrecovered execution address 0x%04x", PC);
        return std::runtime_error(buf);
    }

    void ReturnFromHook() {
        PC = Pop();
        Cycles += 10;
    }

    int Read8(int address) { return Bus->Read(address & 65535); }
    void Write8(int address, int value) { Bus->Write(address & 65535, value & 255); }
    int Read16(int address) { return Read8(address) | (Read8(address + 1) << 8); }
    void Write16(int address, int value) { Write8(address, value); Write8(address + 1, value >> 8); }
    int PortRead(int port) { return Bus->In(port & 65535); }
    void PortWrite(int port, int value) { Bus->Out(port & 65535, value & 255); }
    void Push(int value) { SP -= 2; Write16(SP, value); }
    int Pop() { int v = Read16(SP); SP += 2; return v; }

    static int Parity(int v) { v ^= v >> 4; v ^= v >> 2; v ^= v >> 1; return (v & 1) == 0 ? 4 : 0; }
    static int Sz(int v) { return (v & 0xa8) | (v == 0 ? 64 : 0); }

    void Add(int v, int carry = 0) {
        int old = A, res = old + v + carry;
        A = res;
        F = Sz(A) | (res > 255 ? 1 : 0) | ((old ^ v ^ A) & 16) | (((~(old ^ v) & (old ^ A) & 128) != 0) ? 4 : 0);
    }
    void Adc(int v) { Add(v, F & 1); }
    void Sub(int v, int carry = 0) {
        int old = A, res = old - v - carry;
        A = res;
        F = Sz(A) | 2 | (res < 0 ? 1 : 0) | ((old ^ v ^ A) & 16) | ((((old ^ v) & (old ^ A) & 128) != 0) ? 4 : 0);
    }
    void Sbc(int v) { Sub(v, F & 1); }
    void Cp(int v) { int old = A; Sub(v); F = (F & ~0x28) | (v & 0x28); A = old; }
    void And(int v) { A &= v; F = Sz(A) | 16 | Parity(A); }
    void Or(int v) { A |= v; F = Sz(A) | Parity(A); }
    void Xor(int v) { A ^= v; F = Sz(A) | Parity(A); }
    int Inc8(int v) { int res = (v + 1) & 255; F = (F & 1) | Sz(res) | ((res & 15) == 0 ? 16 : 0) | (res == 128 ? 4 : 0); return res; }
    int Dec8(int v) { int res = (v - 1) & 255; F = (F & 1) | Sz(res) | 2 | ((res & 15) == 15 ? 16 : 0) | (res == 127 ? 4 : 0); return res; }
    void Bit(int n, int v, int yx) { bool set = (v & (1 << n)) != 0; F = (F & 1) | 16 | (set ? 0 : 68) | (n == 7 && set ? 128 : 0) | (yx & 40); }
    int Add16(int old, int v) { int res = old + v; F = (F & 196) | (res > 65535 ? 1 : 0) | (((old ^ v ^ res) >> 8) & 16) | ((res >> 8) & 40); return res & 65535; }
    void AdcHl(int v) {
        int old = HL, res = old + v + (F & 1);
        HL = res;
        F = ((HL >> 8) & 168) | (HL == 0 ? 64 : 0) | (((old ^ HL ^ v) >> 8) & 16) | ((((old ^ HL) & (v ^ HL) & 32768) != 0) ? 4 : 0) | (res > 65535 ? 1 : 0);
    }
    void SbcHl(int v) {
        int old = HL, res = old - v - (F & 1);
        HL = res;
        F = ((HL >> 8) & 168) | (HL == 0 ? 64 : 0) | (((old ^ HL ^ v) >> 8) & 16) | ((((old ^ v) & (old ^ HL) & 32768) != 0) ? 4 : 0) | 2 | (res < 0 ? 1 : 0);
    }
    int Rotate(int v, int mode) {
        int carry = (mode % 2 == 0 ? v >> 7 : v & 1) & 1;
        int res;
        switch (mode) {
            case 0: res = (v << 1) | (v >> 7); break;
            case 1: res = (v >> 1) | (v << 7); break;
            case 2: res = (v << 1) | (F & 1); break;
            case 3: res = (v >> 1) | ((F & 1) << 7); break;
            case 4: res = v << 1; break;
            case 5: res = (v >> 1) | (v & 128); break;
            case 6: res = (v << 1) | 1; break;
            default: res = v >> 1; break;
        }
        res &= 255;
        F = Sz(res) | Parity(res) | carry;
        return res;
    }
    void RotateA(int mode) { int keep = F & 196; A = Rotate(A, mode); F = keep | (F & 1) | (A & 40); }
    void Daa() {
        int old = A, correction = 0, carry = F & 1;
        if ((F & 16) != 0 || (A & 15) > 9) correction |= 6;
        if (carry != 0 || A > 153) { correction |= 96; carry = 1; }
        int res = (((F & 2) != 0 ? A - correction : A + correction)) & 255;
        F = Sz(res) | Parity(res) | (F & 2) | carry | (((F & 2) != 0 ? ((F & 16) != 0 && (old & 15) < 6) : (old & 15) > 9) ? 16 : 0);
        A = res;
    }
    void Block(bool down, bool compare) {
        int v = Read8(HL), delta = down ? -1 : 1;
        HL += delta;
        BC--;
        if (compare) {
            int res = (A - v) & 255, half = (A ^ v ^ res) & 16, yx = (res - (half != 0 ? 1 : 0)) & 255;
            F = (F & 1) | (res & 128) | (res == 0 ? 64 : 0) | half | (BC != 0 ? 4 : 0) | 2 | (((yx << 4) | (yx & 15)) & 40);
        } else {
            Write8(DE, v);
            DE += delta;
            int sum = (A + v) & 255;
            F = (F & 193) | (BC != 0 ? 4 : 0) | (sum & 8) | ((sum & 2) != 0 ? 32 : 0);
        }
    }
    void NibbleRotate(bool left) {
        int v = Read8(HL), old = A;
        if (left) {
            A = (A & 240) | (v >> 4);
            Write8(HL, (v << 4) | (old & 15));
        } else {
            A = (A & 240) | (v & 15);
            Write8(HL, ((old & 15) << 4) | (v >> 4));
        }
        F = (F & 1) | Sz(A) | Parity(A);
    }
    void BlockIo(bool input, bool down) {
        int delta = down ? -1 : 1, v;
        if (input) {
            v = PortRead(BC);
            Write8(HL, v);
            B--;
        } else {
            v = Read8(HL);
            B--;
            PortWrite(BC, v);
        }
        HL += delta;
        int k = v + (input ? ((C + delta) & 255) : (int)L);
        F = Sz(B) | ((v & 128) != 0 ? 2 : 0) | (k > 255 ? 17 : 0) | Parity((k & 7) ^ B);
    }
};
