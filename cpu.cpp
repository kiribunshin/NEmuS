#include "cpu.h"
#include "bus.h"
#include <cstdio>

//lookup table:
static const Instruction lookup[256] = {
    {"BRK", &CPU::IMM, &CPU::BRK, 7}, {"ORA", &CPU::IZX, &CPU::ORA, 6}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"???", &CPU::IMP, &CPU::XXX, 8}, {"???", &CPU::IMP, &CPU::NOP, 3}, {"ORA", &CPU::ZP0, &CPU::ORA, 3}, {"ASL", &CPU::ZP0, &CPU::ASL, 5}, {"???", &CPU::IMP, &CPU::XXX, 5}, {"PHP", &CPU::IMP, &CPU::PHP, 3}, {"ORA", &CPU::IMM, &CPU::ORA, 2}, {"ASL", &CPU::ACC, &CPU::ASL, 2}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"???", &CPU::IMP, &CPU::NOP, 4}, {"ORA", &CPU::ABS, &CPU::ORA, 4}, {"ASL", &CPU::ABS, &CPU::ASL, 6}, {"???", &CPU::IMP, &CPU::XXX, 6},
    {"BPL", &CPU::REL, &CPU::BPL, 2}, {"ORA", &CPU::IZY, &CPU::ORA, 5}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"???", &CPU::IMP, &CPU::XXX, 8}, {"???", &CPU::IMP, &CPU::NOP, 4}, {"ORA", &CPU::ZPX, &CPU::ORA, 4}, {"ASL", &CPU::ZPX, &CPU::ASL, 6}, {"???", &CPU::IMP, &CPU::XXX, 6}, {"CLC", &CPU::IMP, &CPU::CLC, 2}, {"ORA", &CPU::ABY, &CPU::ORA, 4}, {"???", &CPU::IMP, &CPU::NOP, 2}, {"???", &CPU::IMP, &CPU::XXX, 7}, {"???", &CPU::IMP, &CPU::NOP, 4}, {"ORA", &CPU::ABX, &CPU::ORA, 4}, {"ASL", &CPU::ABX, &CPU::ASL, 7}, {"???", &CPU::IMP, &CPU::XXX, 7},
    {"JSR", &CPU::ABS, &CPU::JSR, 6}, {"AND", &CPU::IZX, &CPU::AND, 6}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"???", &CPU::IMP, &CPU::XXX, 8}, {"BIT", &CPU::ZP0, &CPU::BIT, 3}, {"AND", &CPU::ZP0, &CPU::AND, 3}, {"ROL", &CPU::ZP0, &CPU::ROL, 5}, {"???", &CPU::IMP, &CPU::XXX, 5}, {"PLP", &CPU::IMP, &CPU::PLP, 4}, {"AND", &CPU::IMM, &CPU::AND, 2}, {"ROL", &CPU::ACC, &CPU::ROL, 2}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"BIT", &CPU::ABS, &CPU::BIT, 4}, {"AND", &CPU::ABS, &CPU::AND, 4}, {"ROL", &CPU::ABS, &CPU::ROL, 6}, {"???", &CPU::IMP, &CPU::XXX, 6},
    {"BMI", &CPU::REL, &CPU::BMI, 2}, {"AND", &CPU::IZY, &CPU::AND, 5}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"???", &CPU::IMP, &CPU::XXX, 8}, {"???", &CPU::IMP, &CPU::NOP, 4}, {"AND", &CPU::ZPX, &CPU::AND, 4}, {"ROL", &CPU::ZPX, &CPU::ROL, 6}, {"???", &CPU::IMP, &CPU::XXX, 6}, {"SEC", &CPU::IMP, &CPU::SEC, 2}, {"AND", &CPU::ABY, &CPU::AND, 4}, {"???", &CPU::IMP, &CPU::NOP, 2}, {"???", &CPU::IMP, &CPU::XXX, 7}, {"???", &CPU::IMP, &CPU::NOP, 4}, {"AND", &CPU::ABX, &CPU::AND, 4}, {"ROL", &CPU::ABX, &CPU::ROL, 7}, {"???", &CPU::IMP, &CPU::XXX, 7},
    {"RTI", &CPU::IMP, &CPU::RTI, 6}, {"EOR", &CPU::IZX, &CPU::EOR, 6}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"???", &CPU::IMP, &CPU::XXX, 8}, {"???", &CPU::IMP, &CPU::NOP, 3}, {"EOR", &CPU::ZP0, &CPU::EOR, 3}, {"LSR", &CPU::ZP0, &CPU::LSR, 5}, {"???", &CPU::IMP, &CPU::XXX, 5}, {"PHA", &CPU::IMP, &CPU::PHA, 3}, {"EOR", &CPU::IMM, &CPU::EOR, 2}, {"LSR", &CPU::ACC, &CPU::LSR, 2}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"JMP", &CPU::ABS, &CPU::JMP, 3}, {"EOR", &CPU::ABS, &CPU::EOR, 4}, {"LSR", &CPU::ABS, &CPU::LSR, 6}, {"???", &CPU::IMP, &CPU::XXX, 6},
    {"BVC", &CPU::REL, &CPU::BVC, 2}, {"EOR", &CPU::IZY, &CPU::EOR, 5}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"???", &CPU::IMP, &CPU::XXX, 8}, {"???", &CPU::IMP, &CPU::NOP, 4}, {"EOR", &CPU::ZPX, &CPU::EOR, 4}, {"LSR", &CPU::ZPX, &CPU::LSR, 6}, {"???", &CPU::IMP, &CPU::XXX, 6}, {"CLI", &CPU::IMP, &CPU::CLI, 2}, {"EOR", &CPU::ABY, &CPU::EOR, 4}, {"???", &CPU::IMP, &CPU::NOP, 2}, {"???", &CPU::IMP, &CPU::XXX, 7}, {"???", &CPU::IMP, &CPU::NOP, 4}, {"EOR", &CPU::ABX, &CPU::EOR, 4}, {"LSR", &CPU::ABX, &CPU::LSR, 7}, {"???", &CPU::IMP, &CPU::XXX, 7},
    {"RTS", &CPU::IMP, &CPU::RTS, 6}, {"ADC", &CPU::IZX, &CPU::ADC, 6}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"???", &CPU::IMP, &CPU::XXX, 8}, {"???", &CPU::IMP, &CPU::NOP, 3}, {"ADC", &CPU::ZP0, &CPU::ADC, 3}, {"ROR", &CPU::ZP0, &CPU::ROR, 5}, {"???", &CPU::IMP, &CPU::XXX, 5}, {"PLA", &CPU::IMP, &CPU::PLA, 4}, {"ADC", &CPU::IMM, &CPU::ADC, 2}, {"ROR", &CPU::ACC, &CPU::ROR, 2}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"JMP", &CPU::IND, &CPU::JMP, 5}, {"ADC", &CPU::ABS, &CPU::ADC, 4}, {"ROR", &CPU::ABS, &CPU::ROR, 6}, {"???", &CPU::IMP, &CPU::XXX, 6},
    {"BVS", &CPU::REL, &CPU::BVS, 2}, {"ADC", &CPU::IZY, &CPU::ADC, 5}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"???", &CPU::IMP, &CPU::XXX, 8}, {"???", &CPU::IMP, &CPU::NOP, 4}, {"ADC", &CPU::ZPX, &CPU::ADC, 4}, {"ROR", &CPU::ZPX, &CPU::ROR, 6}, {"???", &CPU::IMP, &CPU::XXX, 6}, {"SEI", &CPU::IMP, &CPU::SEI, 2}, {"ADC", &CPU::ABY, &CPU::ADC, 4}, {"???", &CPU::IMP, &CPU::NOP, 2}, {"???", &CPU::IMP, &CPU::XXX, 7}, {"???", &CPU::IMP, &CPU::NOP, 4}, {"ADC", &CPU::ABX, &CPU::ADC, 4}, {"ROR", &CPU::ABX, &CPU::ROR, 7}, {"???", &CPU::IMP, &CPU::XXX, 7},
    {"???", &CPU::IMP, &CPU::NOP, 2}, {"STA", &CPU::IZX, &CPU::STA, 6}, {"???", &CPU::IMP, &CPU::NOP, 2}, {"???", &CPU::IMP, &CPU::XXX, 6}, {"STY", &CPU::ZP0, &CPU::STY, 3}, {"STA", &CPU::ZP0, &CPU::STA, 3}, {"STX", &CPU::ZP0, &CPU::STX, 3}, {"???", &CPU::IMP, &CPU::XXX, 3}, {"DEY", &CPU::IMP, &CPU::DEY, 2}, {"???", &CPU::IMP, &CPU::NOP, 2}, {"TXA", &CPU::IMP, &CPU::TXA, 2}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"STY", &CPU::ABS, &CPU::STY, 4}, {"STA", &CPU::ABS, &CPU::STA, 4}, {"STX", &CPU::ABS, &CPU::STX, 4}, {"???", &CPU::IMP, &CPU::XXX, 4},
    {"BCC", &CPU::REL, &CPU::BCC, 2}, {"STA", &CPU::IZY, &CPU::STA, 6}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"???", &CPU::IMP, &CPU::XXX, 6}, {"STY", &CPU::ZPX, &CPU::STY, 4}, {"STA", &CPU::ZPX, &CPU::STA, 4}, {"STX", &CPU::ZPY, &CPU::STX, 4}, {"???", &CPU::IMP, &CPU::XXX, 4}, {"TYA", &CPU::IMP, &CPU::TYA, 2}, {"STA", &CPU::ABY, &CPU::STA, 5}, {"TXS", &CPU::IMP, &CPU::TXS, 2}, {"???", &CPU::IMP, &CPU::XXX, 5}, {"???", &CPU::IMP, &CPU::NOP, 5}, {"STA", &CPU::ABX, &CPU::STA, 5}, {"???", &CPU::IMP, &CPU::XXX, 5}, {"???", &CPU::IMP, &CPU::XXX, 5},
    {"LDY", &CPU::IMM, &CPU::LDY, 2}, {"LDA", &CPU::IZX, &CPU::LDA, 6}, {"LDX", &CPU::IMM, &CPU::LDX, 2}, {"???", &CPU::IMP, &CPU::XXX, 6}, {"LDY", &CPU::ZP0, &CPU::LDY, 3}, {"LDA", &CPU::ZP0, &CPU::LDA, 3}, {"LDX", &CPU::ZP0, &CPU::LDX, 3}, {"???", &CPU::IMP, &CPU::XXX, 3}, {"TAY", &CPU::IMP, &CPU::TAY, 2}, {"LDA", &CPU::IMM, &CPU::LDA, 2}, {"TAX", &CPU::IMP, &CPU::TAX, 2}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"LDY", &CPU::ABS, &CPU::LDY, 4}, {"LDA", &CPU::ABS, &CPU::LDA, 4}, {"LDX", &CPU::ABS, &CPU::LDX, 4}, {"???", &CPU::IMP, &CPU::XXX, 4},
    {"BCS", &CPU::REL, &CPU::BCS, 2}, {"LDA", &CPU::IZY, &CPU::LDA, 5}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"???", &CPU::IMP, &CPU::XXX, 5}, {"LDY", &CPU::ZPX, &CPU::LDY, 4}, {"LDA", &CPU::ZPX, &CPU::LDA, 4}, {"LDX", &CPU::ZPY, &CPU::LDX, 4}, {"???", &CPU::IMP, &CPU::XXX, 4}, {"CLV", &CPU::IMP, &CPU::CLV, 2}, {"LDA", &CPU::ABY, &CPU::LDA, 4}, {"TSX", &CPU::IMP, &CPU::TSX, 2}, {"???", &CPU::IMP, &CPU::XXX, 4}, {"LDY", &CPU::ABX, &CPU::LDY, 4}, {"LDA", &CPU::ABX, &CPU::LDA, 4}, {"LDX", &CPU::ABY, &CPU::LDX, 4}, {"???", &CPU::IMP, &CPU::XXX, 4},
    {"CPY", &CPU::IMM, &CPU::CPY, 2}, {"CMP", &CPU::IZX, &CPU::CMP, 6}, {"???", &CPU::IMP, &CPU::NOP, 2}, {"???", &CPU::IMP, &CPU::XXX, 8}, {"CPY", &CPU::ZP0, &CPU::CPY, 3}, {"CMP", &CPU::ZP0, &CPU::CMP, 3}, {"DEC", &CPU::ZP0, &CPU::DEC, 5}, {"???", &CPU::IMP, &CPU::XXX, 5}, {"INY", &CPU::IMP, &CPU::INY, 2}, {"CMP", &CPU::IMM, &CPU::CMP, 2}, {"DEX", &CPU::IMP, &CPU::DEX, 2}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"CPY", &CPU::ABS, &CPU::CPY, 4}, {"CMP", &CPU::ABS, &CPU::CMP, 4}, {"DEC", &CPU::ABS, &CPU::DEC, 6}, {"???", &CPU::IMP, &CPU::XXX, 6},
    {"BNE", &CPU::REL, &CPU::BNE, 2}, {"CMP", &CPU::IZY, &CPU::CMP, 5}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"???", &CPU::IMP, &CPU::XXX, 8}, {"???", &CPU::IMP, &CPU::NOP, 4}, {"CMP", &CPU::ZPX, &CPU::CMP, 4}, {"DEC", &CPU::ZPX, &CPU::DEC, 6}, {"???", &CPU::IMP, &CPU::XXX, 6}, {"CLD", &CPU::IMP, &CPU::CLD, 2}, {"CMP", &CPU::ABY, &CPU::CMP, 4}, {"NOP", &CPU::IMP, &CPU::NOP, 2}, {"???", &CPU::IMP, &CPU::XXX, 7}, {"???", &CPU::IMP, &CPU::NOP, 4}, {"CMP", &CPU::ABX, &CPU::CMP, 4}, {"DEC", &CPU::ABX, &CPU::DEC, 7}, {"???", &CPU::IMP, &CPU::XXX, 7},
    {"CPX", &CPU::IMM, &CPU::CPX, 2}, {"SBC", &CPU::IZX, &CPU::SBC, 6}, {"???", &CPU::IMP, &CPU::NOP, 2}, {"???", &CPU::IMP, &CPU::XXX, 8}, {"CPX", &CPU::ZP0, &CPU::CPX, 3}, {"SBC", &CPU::ZP0, &CPU::SBC, 3}, {"INC", &CPU::ZP0, &CPU::INC, 5}, {"???", &CPU::IMP, &CPU::XXX, 5}, {"INX", &CPU::IMP, &CPU::INX, 2}, {"SBC", &CPU::IMM, &CPU::SBC, 2}, {"NOP", &CPU::IMP, &CPU::NOP, 2}, {"???", &CPU::IMM, &CPU::SBC, 2}, {"CPX", &CPU::ABS, &CPU::CPX, 4}, {"SBC", &CPU::ABS, &CPU::SBC, 4}, {"INC", &CPU::ABS, &CPU::INC, 6}, {"???", &CPU::IMP, &CPU::XXX, 6},
    {"BEQ", &CPU::REL, &CPU::BEQ, 2}, {"SBC", &CPU::IZY, &CPU::SBC, 5}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"???", &CPU::IMP, &CPU::XXX, 8}, {"???", &CPU::IMP, &CPU::NOP, 4}, {"SBC", &CPU::ZPX, &CPU::SBC, 4}, {"INC", &CPU::ZPX, &CPU::INC, 6}, {"???", &CPU::IMP, &CPU::XXX, 6}, {"SED", &CPU::IMP, &CPU::SED, 2}, {"SBC", &CPU::ABY, &CPU::SBC, 4}, {"NOP", &CPU::IMP, &CPU::NOP, 2}, {"???", &CPU::IMP, &CPU::XXX, 7}, {"???", &CPU::IMP, &CPU::NOP, 4}, {"SBC", &CPU::ABX, &CPU::SBC, 4}, {"INC", &CPU::ABX, &CPU::INC, 7}, {"???", &CPU::IMP, &CPU::XXX, 7},
};

//6502 flags === the famous NV-BDIZC
constexpr uint8_t FLAG_C = (1 << 0); // set on unsigned carry/borrow
constexpr uint8_t FLAG_Z = (1 << 1); // Set if result == 0
constexpr uint8_t FLAG_I = (1 << 2); // Set to block IRQs
constexpr uint8_t FLAG_D = (1 << 3); // Selects BCD mode for ADC/SBC, but on the 2A03 specifically, this bit exists but does nothing, since Nintendo disabled decimal mode in hardware. Still worth tracking correctly since SED/CLD/PHP still read/write it.
constexpr uint8_t FLAG_B = (1 << 4); // Set when BRK causes an interrupt (only meaningful in the byte pushed to the stack, not a "real" persistent flag)
constexpr uint8_t FLAG_U = (1 << 5); // unused, always 1
constexpr uint8_t FLAG_V = (1 << 6); // signed arithmetic overflow
constexpr uint8_t FLAG_N = (1 << 7); // negative

//Addressing modes:
uint16_t CPU::ACC() {
    fetched = A;
    useAccumulator = true;
    return 0;
}
uint16_t CPU::IMP() {
    return 0;
}
uint16_t CPU::IMM() {
    addr_abs = PC++;
    return 0;
}
uint16_t CPU::ZP0() {
    uint8_t lo = read(PC++);
    addr_abs = (0x00 << 8) | lo;
    return 0;
}
uint16_t CPU::ZPX() {
    uint8_t base_address = read(PC++);
    addr_abs = (base_address + X) & 0x00FF;
    return 0;
}
uint16_t CPU::ZPY() {
    uint8_t base_address = read(PC++);
    addr_abs = (base_address + Y) & 0x00FF;
    return 0;
}
uint16_t CPU::ABS() {
    uint8_t lo = read(PC++);
    uint8_t hi = read(PC++);
    addr_abs = (hi << 8) | lo;
    return 0;
}
uint16_t CPU::ABX() {
    uint8_t lo = read(PC++);
    uint8_t hi = read(PC++);
    addr_abs = ((hi << 8) | lo) + X;
    if ((addr_abs >> 8) != hi) { return 1; }
    return 0;
}
uint16_t CPU::ABY() {
    uint8_t lo = read(PC++);
    uint8_t hi = read(PC++);
    addr_abs = ((hi << 8) | lo) + Y;
    if ((addr_abs >> 8) != hi) { return 1; }
    return 0;
}
uint16_t CPU::REL() {
    int8_t offset = read(PC++);
    addr_abs = PC + offset;
    return 0;
}
uint16_t CPU::IND() {
    uint8_t lo = read(PC++);
    uint8_t hi = read(PC++);
    uint16_t ptr = (hi << 8) | lo;
    uint8_t low = read(ptr);
    uint8_t high = read((ptr & 0xFF00) | ((ptr + 1) & 0x00FF)); //a bit odd and counter-intuitive, but here I'm replicating a real and documented bug in the 6502 == games EXPECT this bug, thus its presence
    addr_abs = (high << 8) | low;
    return 0;
}
uint16_t CPU::IZX() {
    uint16_t t = read(PC++);
    t += X;
    t &= 0x00FF;
    uint8_t lo = read(t);
    uint8_t hi = read((t+1) & 0x00FF);
    addr_abs = (hi << 8) | lo;
    return 0;
}
uint16_t CPU::IZY() {
    uint16_t t = read(PC++);
    uint8_t lo = read(t);
    uint8_t hi = read((t+1) & 0x00FF);
    addr_abs = ((hi << 8) | lo) + Y;
    if ((addr_abs >> 8) != hi) { return 1; }
    return 0;
}

//Opcodes:
uint8_t CPU::LDA() {
    fetch();
    A = fetched;
    updateZN(A);
    return 1;
}
uint8_t CPU::LDX() {
    fetch();
    X = fetched;
    updateZN(X);
    return 1;
}
uint8_t CPU::LDY() {
    fetch();
    Y = fetched;
    updateZN(Y);
    return 1;
}
uint8_t CPU::STA() {
    write(addr_abs, A);
    return 0;
}
uint8_t CPU::STX() {
    write(addr_abs, X);
    return 0;
}
uint8_t CPU::STY() {
    write(addr_abs, Y);
    return 0;
}
uint8_t CPU::TAX() {
    X = A;
    updateZN(X);
    return 0;
}
uint8_t CPU::TAY() {
    Y = A;
    updateZN(Y);
    return 0;
}
uint8_t CPU::TXA() {
    A = X;
    updateZN(A);
    return 0;
}
uint8_t CPU::TYA() {
    A = Y;
    updateZN(A);
    return 0;
}
uint8_t CPU::TSX() {
    X = SP;
    updateZN(X);
    return 0;
}
uint8_t CPU::TXS() {
    SP = X;
    return 0;
}
uint8_t CPU::CLC() {
    setFlag(FLAG_C, false);
    return 0;
}
uint8_t CPU::CLD() {
    setFlag(FLAG_D, false);
    return 0;
}
uint8_t CPU::CLI() {
    setFlag(FLAG_I, false);
    return 0;
}
uint8_t CPU::CLV() {
    setFlag(FLAG_V, false);
    return 0;
}
uint8_t CPU::SEC() {
    setFlag(FLAG_C, true);
    return 0;
}
uint8_t CPU::SED() {
    setFlag(FLAG_D, true);
    return 0;
}
uint8_t CPU::SEI() {
    setFlag(FLAG_I, true);
    return 0;
}
uint8_t CPU::INC() {
    fetch();
    uint8_t res = fetched + 1;
    write(addr_abs, res);
    updateZN(res);
    return 0;
}
uint8_t CPU::DEC() {
    fetch();
    uint8_t res = fetched - 1;
    write(addr_abs, res);
    updateZN(res);
    return 0;
}
uint8_t CPU::INX() {
    X++;
    updateZN(X);
    return 0;
}
uint8_t CPU::INY() {
    Y++;
    updateZN(Y);
    return 0;
}
uint8_t CPU::DEX() {
    X--;
    updateZN(X);
    return 0;
}
uint8_t CPU::DEY() {
    Y--;
    updateZN(Y);
    return 0;
}
uint8_t CPU::AND() {
    fetch();
    A &= fetched;
    updateZN(A);
    return 1;
}
uint8_t CPU::EOR() {
    fetch();
    A ^= fetched;
    updateZN(A);
    return 1;
}
uint8_t CPU::ORA() {
    fetch();
    A |= fetched;
    updateZN(A);
    return 1;
}
uint8_t CPU::BIT() {
    fetch();
    uint8_t test = A & fetched;
    setFlag(FLAG_Z, test == 0);
    setFlag(FLAG_N, fetched & FLAG_N);
    setFlag(FLAG_V, fetched & FLAG_V);
    return 1;
}
uint8_t CPU::CMP() {
    compare(A);
    return 1;
}
uint8_t CPU::CPX() {
    compare(X);
    return 1;
}
uint8_t CPU::CPY() {
    compare(Y);
    return 1;
}
uint8_t CPU::ASL() {
    fetch();
    uint16_t shift = fetched << 1;
    setFlag(FLAG_C, shift & (1 << 8));
    setFlag(FLAG_Z, (shift & 0xFF) == 0);
    setFlag(FLAG_N, shift & FLAG_N);
    if (useAccumulator) {
        A = shift & 0xFF;
    }
    else {
        write(addr_abs, shift & 0xFF);
    }
    return 0;
}
uint8_t CPU::LSR() {
    fetch();
    setFlag(FLAG_C, fetched & 0x1);
    uint8_t shift = fetched >> 1;
    setFlag(FLAG_Z, (shift & 0xFF) == 0);
    setFlag(FLAG_N, shift & FLAG_N);
    if (useAccumulator) {
        A = shift & 0xFF;
    }
    else {
        write(addr_abs, shift & 0xFF);
    }
    return 0;
}
uint8_t CPU::ROL() {
    fetch();
    bool old_carry = getFlag(FLAG_C);
    setFlag(FLAG_C, (fetched >> 7) & 1);
    uint16_t shift = (fetched << 1) | old_carry;
    setFlag(FLAG_Z, (shift & 0xFF) == 0);
    setFlag(FLAG_N, shift & FLAG_N);
    if (useAccumulator) {
        A = shift & 0xFF;
    }
    else {
        write(addr_abs, shift & 0xFF);
    }
    return 0;
}
uint8_t CPU::ROR() {
    fetch();
    bool old_carry = getFlag(FLAG_C);
    setFlag(FLAG_C, fetched & 0x1);
    uint16_t shift = fetched >> 1 | old_carry << 7;
    setFlag(FLAG_Z, (shift & 0xFF) == 0);
    setFlag(FLAG_N, shift & FLAG_N);
    if (useAccumulator) {
        A = shift & 0xFF;
    }
    else {
        write(addr_abs, shift & 0xFF);
    }
    return 0;
}
uint8_t CPU::BCC() {
    if (!getFlag(FLAG_C)) {
        if ((addr_abs & 0xFF00) != (PC & 0xFF00)) {
            cycles++;
        }
        PC = addr_abs;
        cycles++;
    }
    return 0;
}
uint8_t CPU::BCS() {
    if (getFlag(FLAG_C)) {
        if ((addr_abs & 0xFF00) != (PC & 0xFF00)) {
            cycles++;
        }
        PC = addr_abs;
        cycles++;
    }
    return 0;
}
uint8_t CPU::BEQ() {
    if (getFlag(FLAG_Z)) {
        if ((addr_abs & 0xFF00) != (PC & 0xFF00)) {
            cycles++;
        }
        PC = addr_abs;
        cycles++;
    }
    return 0;
}
uint8_t CPU::BNE() {
    if (!getFlag(FLAG_Z)) {
        if ((addr_abs & 0xFF00) != (PC & 0xFF00)) {
            cycles++;
        }
        PC = addr_abs;
        cycles++;
    }
    return 0;
}
uint8_t CPU::BMI() {
    if (getFlag(FLAG_N)) {
        if ((addr_abs & 0xFF00) != (PC & 0xFF00)) {
            cycles++;
        }
        PC = addr_abs;
        cycles++;
    }
    return 0;
}
uint8_t CPU::BPL() {
    if (!getFlag(FLAG_N)) {
        if ((addr_abs & 0xFF00) != (PC & 0xFF00)) {
            cycles++;
        }
        PC = addr_abs;
        cycles++;
    }
    return 0;
}
uint8_t CPU::BVC() {
    if (!getFlag(FLAG_V)) {
        if ((addr_abs & 0xFF00) != (PC & 0xFF00)) {
            cycles++;
        }
        PC = addr_abs;
        cycles++;
    }
    return 0;
}
uint8_t CPU::BVS() {
    if (getFlag(FLAG_V)) {
        if ((addr_abs & 0xFF00) != (PC & 0xFF00)) {
            cycles++;
        }
        PC = addr_abs;
        cycles++;
    }
    return 0;
}
uint8_t CPU::PHA() {
    write(0x0100 | SP, A);
    SP--;
    return 0;
}
uint8_t CPU::PHP() {
    write(0x0100 | SP, P | FLAG_B | FLAG_U);
    SP--;
    return 0;
}
uint8_t CPU::PLA() {
    SP++;
    A = read(0x0100 | SP);
    updateZN(A);
    return 0;
}
uint8_t CPU::PLP() {
    SP++;
    P = read(0x0100 | SP);
    P &= ~FLAG_B;
    P |= FLAG_U;
    return 0;
}
uint8_t CPU::JMP() {
    PC = addr_abs;
    return 0;
}
uint8_t CPU::JSR() {
    uint16_t comeback = PC - 1;
    write(0x0100 | SP, comeback >> 8);
    SP--;
    write(0x0100 | SP, comeback & 0xFF);
    SP--;
    PC = addr_abs;
    return 0;
}
uint8_t CPU::RTS() {
    SP++;
    uint8_t lo = read(0x0100 | SP);
    SP++;
    uint8_t hi = read(0x0100 | SP);
    PC = ((hi << 8) | lo) + 1;
    return 0;
}
uint8_t CPU::RTI() {
    SP++;
    P = read(0x0100 | SP);
    P &= ~FLAG_B;
    P |= FLAG_U;
    SP++;
    uint8_t lo = read(0x0100 | SP);
    SP++;
    uint8_t hi = read(0x0100 | SP);
    PC = (hi << 8) | lo;
    return 0;
}
uint8_t CPU::BRK() {
    PC++;
    write(0x0100 | SP, (PC >> 8) & 0xFF);
    SP--;
    write(0x0100 | SP, PC & 0xFF);
    SP--;
    write(0x0100 | SP, P | FLAG_B | FLAG_U);
    SP--;
    setFlag(FLAG_I, true);
    uint8_t lo = read(0xFFFE);
    uint8_t hi = read(0xFFFF);
    PC = (hi << 8) | lo;
    return 0;
}
uint8_t CPU::ADC() {
    fetch();
    uint16_t sum = (uint16_t)A + (uint16_t)fetched + (uint16_t)getFlag(FLAG_C);
    setFlag(FLAG_C, sum > 0xFF);
    setFlag(FLAG_Z, (sum & 0xFF) == 0);
    setFlag(FLAG_N, (sum & 0xFF) & FLAG_N);
    setFlag(FLAG_V, (~(A ^ fetched) & (A ^ sum) & 0x80));
    A = sum & 0xFF;
    return 1;
}
uint8_t CPU::SBC() {
    fetch();
    uint16_t value = ((uint16_t)fetched) ^ 0x00FF;
    uint16_t sum = (uint16_t)A + value + (uint16_t)getFlag(FLAG_C);
    setFlag(FLAG_C, sum > 0xFF);
    setFlag(FLAG_Z, (sum & 0xFF) == 0);
    setFlag(FLAG_N, (sum & 0xFF) & FLAG_N);
    setFlag(FLAG_V, (~(A ^ value) & (A ^ sum) & 0x80));
    A = sum & 0xFF;
    return 1;
}
uint8_t CPU::NOP() {
    return 1;
}
uint8_t CPU::XXX() {
    printf("ILLEGAL OPCODE EXECUTED: $%02X\n", opcode);
    return 0;
}

uint8_t CPU::read(uint16_t addr) {
    return bus->read(addr, false);
}

void CPU::write(uint16_t addr, uint8_t data) {
    bus->write(addr, data);
}

uint8_t CPU::fetch() {
    fetched = read(addr_abs);
    return fetched;
}

void CPU::setFlag(uint8_t flag, bool value) {
    if (value) {
        P |= flag;
    } else {
        P &= ~flag;
    }
}

bool CPU::getFlag(uint8_t flag) const {
    return (P & flag) == flag;
}

void CPU::updateZN(uint8_t value) {
    setFlag(FLAG_Z, value == 0);
    setFlag(FLAG_N, (value & FLAG_N) == FLAG_N);
}

void CPU::compare(uint8_t reg) {
    fetch();
    uint8_t res = reg - fetched;
    setFlag(FLAG_C, reg >= fetched);
    setFlag(FLAG_Z, reg == fetched);
    setFlag(FLAG_N, res & FLAG_N);
}

void CPU::reset() {
    A = X = Y = 0;
    SP = 0xFD;
    P = 0x00 | FLAG_U | FLAG_I;
    uint8_t lo = read(0xFFFC);
    uint8_t hi = read(0xFFFD);
    PC = (hi << 8) | lo;
    addr_abs = fetched = 0;
    cycles = 8;
}

void CPU::irq() {
    if (getFlag(FLAG_I)) return;
    write(0x0100 | SP, (PC >> 8) & 0xFF);
    SP--;
    write(0x0100 | SP, PC & 0xFF);
    SP--;
    write(0x0100 | SP, (P & ~FLAG_B) | FLAG_U);
    SP--;
    setFlag(FLAG_I, true);
    uint8_t lo = read(0xFFFE);
    uint8_t hi = read(0xFFFF);
    PC = (hi << 8) | lo;
    cycles = 7;
}

void CPU::nmi() {
    write(0x0100 | SP, (PC >> 8) & 0xFF);
    SP--;
    write(0x0100 | SP, PC & 0xFF);
    SP--;
    write(0x0100 | SP, (P & ~FLAG_B) | FLAG_U);
    SP--;
    setFlag(FLAG_I, true);
    uint8_t lo = read(0xFFFA);
    uint8_t hi = read(0xFFFB);
    PC = (hi << 8) | lo;
    cycles = 8;
}

void CPU::trace() {
    printf("%04X  A:%02X X:%02X Y:%02X P:%02X SP:%02X CYC:%llu\n",
           PC, A, X, Y, P, SP, totalCycles);
}

void CPU::clock() {
    if (cycles == 0) {
        trace();  // logs current PC/registers, before this instruction consumes anything
        opcode = read(PC++);
        const Instruction& instr = lookup[opcode];

        cycles = instr.cycles;

        useAccumulator = false;
        uint8_t extra1 = (this->*instr.addrmode)();
        uint8_t extra2 = (this->*instr.operate)();

        cycles += (extra1 & extra2);
    }

    cycles--;
    totalCycles++;
}