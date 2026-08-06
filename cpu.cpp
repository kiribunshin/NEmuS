#include "cpu.h"
#include "bus.h"
#include <cstdio>

//lookup table:
static const Instruction lookup[256] = {
    {"BRK", &CPU::IMM, &CPU::BRK, 7}, {"ORA", &CPU::IZX, &CPU::ORA, 6}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"???", &CPU::IMP, &CPU::XXX, 8}, {"???", &CPU::IMP, &CPU::NOP, 3}, {"ORA", &CPU::ZP0, &CPU::ORA, 3}, {"ASL", &CPU::ZP0, &CPU::ASL, 5}, {"???", &CPU::IMP, &CPU::XXX, 5}, {"PHP", &CPU::IMP, &CPU::PHP, 3}, {"ORA", &CPU::IMM, &CPU::ORA, 2}, {"ASL", &CPU::IMP, &CPU::ASL, 2}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"???", &CPU::IMP, &CPU::NOP, 4}, {"ORA", &CPU::ABS, &CPU::ORA, 4}, {"ASL", &CPU::ABS, &CPU::ASL, 6}, {"???", &CPU::IMP, &CPU::XXX, 6},
    {"BPL", &CPU::REL, &CPU::BPL, 2}, {"ORA", &CPU::IZY, &CPU::ORA, 5}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"???", &CPU::IMP, &CPU::XXX, 8}, {"???", &CPU::IMP, &CPU::NOP, 4}, {"ORA", &CPU::ZPX, &CPU::ORA, 4}, {"ASL", &CPU::ZPX, &CPU::ASL, 6}, {"???", &CPU::IMP, &CPU::XXX, 6}, {"CLC", &CPU::IMP, &CPU::CLC, 2}, {"ORA", &CPU::ABY, &CPU::ORA, 4}, {"???", &CPU::IMP, &CPU::NOP, 2}, {"???", &CPU::IMP, &CPU::XXX, 7}, {"???", &CPU::IMP, &CPU::NOP, 4}, {"ORA", &CPU::ABX, &CPU::ORA, 4}, {"ASL", &CPU::ABX, &CPU::ASL, 7}, {"???", &CPU::IMP, &CPU::XXX, 7},
    {"JSR", &CPU::ABS, &CPU::JSR, 6}, {"AND", &CPU::IZX, &CPU::AND, 6}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"???", &CPU::IMP, &CPU::XXX, 8}, {"BIT", &CPU::ZP0, &CPU::BIT, 3}, {"AND", &CPU::ZP0, &CPU::AND, 3}, {"ROL", &CPU::ZP0, &CPU::ROL, 5}, {"???", &CPU::IMP, &CPU::XXX, 5}, {"PLP", &CPU::IMP, &CPU::PLP, 4}, {"AND", &CPU::IMM, &CPU::AND, 2}, {"ROL", &CPU::IMP, &CPU::ROL, 2}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"BIT", &CPU::ABS, &CPU::BIT, 4}, {"AND", &CPU::ABS, &CPU::AND, 4}, {"ROL", &CPU::ABS, &CPU::ROL, 6}, {"???", &CPU::IMP, &CPU::XXX, 6},
    {"BMI", &CPU::REL, &CPU::BMI, 2}, {"AND", &CPU::IZY, &CPU::AND, 5}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"???", &CPU::IMP, &CPU::XXX, 8}, {"???", &CPU::IMP, &CPU::NOP, 4}, {"AND", &CPU::ZPX, &CPU::AND, 4}, {"ROL", &CPU::ZPX, &CPU::ROL, 6}, {"???", &CPU::IMP, &CPU::XXX, 6}, {"SEC", &CPU::IMP, &CPU::SEC, 2}, {"AND", &CPU::ABY, &CPU::AND, 4}, {"???", &CPU::IMP, &CPU::NOP, 2}, {"???", &CPU::IMP, &CPU::XXX, 7}, {"???", &CPU::IMP, &CPU::NOP, 4}, {"AND", &CPU::ABX, &CPU::AND, 4}, {"ROL", &CPU::ABX, &CPU::ROL, 7}, {"???", &CPU::IMP, &CPU::XXX, 7},
    {"RTI", &CPU::IMP, &CPU::RTI, 6}, {"EOR", &CPU::IZX, &CPU::EOR, 6}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"???", &CPU::IMP, &CPU::XXX, 8}, {"???", &CPU::IMP, &CPU::NOP, 3}, {"EOR", &CPU::ZP0, &CPU::EOR, 3}, {"LSR", &CPU::ZP0, &CPU::LSR, 5}, {"???", &CPU::IMP, &CPU::XXX, 5}, {"PHA", &CPU::IMP, &CPU::PHA, 3}, {"EOR", &CPU::IMM, &CPU::EOR, 2}, {"LSR", &CPU::IMP, &CPU::LSR, 2}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"JMP", &CPU::ABS, &CPU::JMP, 3}, {"EOR", &CPU::ABS, &CPU::EOR, 4}, {"LSR", &CPU::ABS, &CPU::LSR, 6}, {"???", &CPU::IMP, &CPU::XXX, 6},
    {"BVC", &CPU::REL, &CPU::BVC, 2}, {"EOR", &CPU::IZY, &CPU::EOR, 5}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"???", &CPU::IMP, &CPU::XXX, 8}, {"???", &CPU::IMP, &CPU::NOP, 4}, {"EOR", &CPU::ZPX, &CPU::EOR, 4}, {"LSR", &CPU::ZPX, &CPU::LSR, 6}, {"???", &CPU::IMP, &CPU::XXX, 6}, {"CLI", &CPU::IMP, &CPU::CLI, 2}, {"EOR", &CPU::ABY, &CPU::EOR, 4}, {"???", &CPU::IMP, &CPU::NOP, 2}, {"???", &CPU::IMP, &CPU::XXX, 7}, {"???", &CPU::IMP, &CPU::NOP, 4}, {"EOR", &CPU::ABX, &CPU::EOR, 4}, {"LSR", &CPU::ABX, &CPU::LSR, 7}, {"???", &CPU::IMP, &CPU::XXX, 7},
    {"RTS", &CPU::IMP, &CPU::RTS, 6}, {"ADC", &CPU::IZX, &CPU::ADC, 6}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"???", &CPU::IMP, &CPU::XXX, 8}, {"???", &CPU::IMP, &CPU::NOP, 3}, {"ADC", &CPU::ZP0, &CPU::ADC, 3}, {"ROR", &CPU::ZP0, &CPU::ROR, 5}, {"???", &CPU::IMP, &CPU::XXX, 5}, {"PLA", &CPU::IMP, &CPU::PLA, 4}, {"ADC", &CPU::IMM, &CPU::ADC, 2}, {"ROR", &CPU::IMP, &CPU::ROR, 2}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"JMP", &CPU::IND, &CPU::JMP, 5}, {"ADC", &CPU::ABS, &CPU::ADC, 4}, {"ROR", &CPU::ABS, &CPU::ROR, 6}, {"???", &CPU::IMP, &CPU::XXX, 6},
    {"BVS", &CPU::REL, &CPU::BVS, 2}, {"ADC", &CPU::IZY, &CPU::ADC, 5}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"???", &CPU::IMP, &CPU::XXX, 8}, {"???", &CPU::IMP, &CPU::NOP, 4}, {"ADC", &CPU::ZPX, &CPU::ADC, 4}, {"ROR", &CPU::ZPX, &CPU::ROR, 6}, {"???", &CPU::IMP, &CPU::XXX, 6}, {"SEI", &CPU::IMP, &CPU::SEI, 2}, {"ADC", &CPU::ABY, &CPU::ADC, 4}, {"???", &CPU::IMP, &CPU::NOP, 2}, {"???", &CPU::IMP, &CPU::XXX, 7}, {"???", &CPU::IMP, &CPU::NOP, 4}, {"ADC", &CPU::ABX, &CPU::ADC, 4}, {"ROR", &CPU::ABX, &CPU::ROR, 7}, {"???", &CPU::IMP, &CPU::XXX, 7},
    {"???", &CPU::IMP, &CPU::NOP, 2}, {"STA", &CPU::IZX, &CPU::STA, 6}, {"???", &CPU::IMP, &CPU::NOP, 2}, {"???", &CPU::IMP, &CPU::XXX, 6}, {"STY", &CPU::ZP0, &CPU::STY, 3}, {"STA", &CPU::ZP0, &CPU::STA, 3}, {"STX", &CPU::ZP0, &CPU::STX, 3}, {"???", &CPU::IMP, &CPU::XXX, 3}, {"DEY", &CPU::IMP, &CPU::DEY, 2}, {"???", &CPU::IMP, &CPU::NOP, 2}, {"TXA", &CPU::IMP, &CPU::TXA, 2}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"STY", &CPU::ABS, &CPU::STY, 4}, {"STA", &CPU::ABS, &CPU::STA, 4}, {"STX", &CPU::ABS, &CPU::STX, 4}, {"???", &CPU::IMP, &CPU::XXX, 4},
    {"BCC", &CPU::REL, &CPU::BCC, 2}, {"STA", &CPU::IZY, &CPU::STA, 6}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"???", &CPU::IMP, &CPU::XXX, 6}, {"STY", &CPU::ZPX, &CPU::STY, 4}, {"STA", &CPU::ZPX, &CPU::STA, 4}, {"STX", &CPU::ZPY, &CPU::STX, 4}, {"???", &CPU::IMP, &CPU::XXX, 4}, {"TYA", &CPU::IMP, &CPU::TYA, 2}, {"STA", &CPU::ABY, &CPU::STA, 5}, {"TXS", &CPU::IMP, &CPU::TXS, 2}, {"???", &CPU::IMP, &CPU::XXX, 5}, {"???", &CPU::IMP, &CPU::NOP, 5}, {"STA", &CPU::ABX, &CPU::STA, 5}, {"???", &CPU::IMP, &CPU::XXX, 5}, {"???", &CPU::IMP, &CPU::XXX, 5},
    {"LDY", &CPU::IMM, &CPU::LDY, 2}, {"LDA", &CPU::IZX, &CPU::LDA, 6}, {"LDX", &CPU::IMM, &CPU::LDX, 2}, {"???", &CPU::IMP, &CPU::XXX, 6}, {"LDY", &CPU::ZP0, &CPU::LDY, 3}, {"LDA", &CPU::ZP0, &CPU::LDA, 3}, {"LDX", &CPU::ZP0, &CPU::LDX, 3}, {"???", &CPU::IMP, &CPU::XXX, 3}, {"TAY", &CPU::IMP, &CPU::TAY, 2}, {"LDA", &CPU::IMM, &CPU::LDA, 2}, {"TAX", &CPU::IMP, &CPU::TAX, 2}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"LDY", &CPU::ABS, &CPU::LDY, 4}, {"LDA", &CPU::ABS, &CPU::LDA, 4}, {"LDX", &CPU::ABS, &CPU::LDX, 4}, {"???", &CPU::IMP, &CPU::XXX, 4},
    {"BCS", &CPU::REL, &CPU::BCS, 2}, {"LDA", &CPU::IZY, &CPU::LDA, 5}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"???", &CPU::IMP, &CPU::XXX, 5}, {"LDY", &CPU::ZPX, &CPU::LDY, 4}, {"LDA", &CPU::ZPX, &CPU::LDA, 4}, {"LDX", &CPU::ZPY, &CPU::LDX, 4}, {"???", &CPU::IMP, &CPU::XXX, 4}, {"CLV", &CPU::IMP, &CPU::CLV, 2}, {"LDA", &CPU::ABY, &CPU::LDA, 4}, {"TSX", &CPU::IMP, &CPU::TSX, 2}, {"???", &CPU::IMP, &CPU::XXX, 4}, {"LDY", &CPU::ABX, &CPU::LDY, 4}, {"LDA", &CPU::ABX, &CPU::LDA, 4}, {"LDX", &CPU::ABY, &CPU::LDX, 4}, {"???", &CPU::IMP, &CPU::XXX, 4},
    {"CPY", &CPU::IMM, &CPU::CPY, 2}, {"CMP", &CPU::IZX, &CPU::CMP, 6}, {"???", &CPU::IMP, &CPU::NOP, 2}, {"???", &CPU::IMP, &CPU::XXX, 8}, {"CPY", &CPU::ZP0, &CPU::CPY, 3}, {"CMP", &CPU::ZP0, &CPU::CMP, 3}, {"DEC", &CPU::ZP0, &CPU::DEC, 5}, {"???", &CPU::IMP, &CPU::XXX, 5}, {"INY", &CPU::IMP, &CPU::INY, 2}, {"CMP", &CPU::IMM, &CPU::CMP, 2}, {"DEX", &CPU::IMP, &CPU::DEX, 2}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"CPY", &CPU::ABS, &CPU::CPY, 4}, {"CMP", &CPU::ABS, &CPU::CMP, 4}, {"DEC", &CPU::ABS, &CPU::DEC, 6}, {"???", &CPU::IMP, &CPU::XXX, 6},
    {"BNE", &CPU::REL, &CPU::BNE, 2}, {"CMP", &CPU::IZY, &CPU::CMP, 5}, {"???", &CPU::IMP, &CPU::XXX, 2}, {"???", &CPU::IMP, &CPU::XXX, 8}, {"???", &CPU::IMP, &CPU::NOP, 4}, {"CMP", &CPU::ZPX, &CPU::CMP, 4}, {"DEC", &CPU::ZPX, &CPU::DEC, 6}, {"???", &CPU::IMP, &CPU::XXX, 6}, {"CLD", &CPU::IMP, &CPU::CLD, 2}, {"CMP", &CPU::ABY, &CPU::CMP, 4}, {"NOP", &CPU::IMP, &CPU::NOP, 2}, {"???", &CPU::IMP, &CPU::XXX, 7}, {"???", &CPU::IMP, &CPU::NOP, 4}, {"CMP", &CPU::ABX, &CPU::CMP, 4}, {"DEC", &CPU::ABX, &CPU::DEC, 7}, {"???", &CPU::IMP, &CPU::XXX, 7},
    {"CPX", &CPU::IMM, &CPU::CPX, 2}, {"SBC", &CPU::IZX, &CPU::SBC, 6}, {"???", &CPU::IMP, &CPU::NOP, 2}, {"???", &CPU::IMP, &CPU::XXX, 8}, {"CPX", &CPU::ZP0, &CPU::CPX, 3}, {"SBC", &CPU::ZP0, &CPU::SBC, 3}, {"INC", &CPU::ZP0, &CPU::INC, 5}, {"???", &CPU::IMP, &CPU::XXX, 5}, {"INX", &CPU::IMP, &CPU::INX, 2}, {"SBC", &CPU::IMM, &CPU::SBC, 2}, {"NOP", &CPU::IMP, &CPU::NOP, 2}, {"???", &CPU::IMP, &CPU::SBC, 2}, {"CPX", &CPU::ABS, &CPU::CPX, 4}, {"SBC", &CPU::ABS, &CPU::SBC, 4}, {"INC", &CPU::ABS, &CPU::INC, 6}, {"???", &CPU::IMP, &CPU::XXX, 6},
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
uint16_t CPU::IMM() {
    addr_abs = PC++;
    return 0;
}
uint16_t CPU::ZP0() {
    uint8_t lo = read(PC++);
    addr_abs = (0x00 << 8) | lo;
    return 0;
}
uint16_t CPU::ABS() {
    uint8_t lo = read(PC++);
    uint8_t hi = read(PC++);
    addr_abs = (hi << 8) | lo;
    return 0;
}

uint16_t CPU::IMP() { return 0; }
uint16_t CPU::ZPX() { return 0; }
uint16_t CPU::ZPY() { return 0; }
uint16_t CPU::REL() { return 0; }
uint16_t CPU::ABX() { return 0; }
uint16_t CPU::ABY() { return 0; }
uint16_t CPU::IND() { return 0; }
uint16_t CPU::IZX() { return 0; }
uint16_t CPU::IZY() { return 0; }

//Opcodes:
uint8_t CPU::LDA() {
    fetch();
    A = fetched;
    updateZN(A);
    return 0;
}

uint8_t CPU::LDX() { return 0; }
uint8_t CPU::LDY() { return 0; }
uint8_t CPU::STA() { return 0; }
uint8_t CPU::STX() { return 0; }
uint8_t CPU::STY() { return 0; }
uint8_t CPU::TAX() { return 0; }
uint8_t CPU::TAY() { return 0; }
uint8_t CPU::TXA() { return 0; }
uint8_t CPU::TYA() { return 0; }
uint8_t CPU::TSX() { return 0; }
uint8_t CPU::TXS() { return 0; }
uint8_t CPU::PHA() { return 0; }
uint8_t CPU::PHP() { return 0; }
uint8_t CPU::PLA() { return 0; }
uint8_t CPU::PLP() { return 0; }
uint8_t CPU::AND() { return 0; }
uint8_t CPU::EOR() { return 0; }
uint8_t CPU::ORA() { return 0; }
uint8_t CPU::BIT() { return 0; }
uint8_t CPU::ADC() { return 0; }
uint8_t CPU::SBC() { return 0; }
uint8_t CPU::CMP() { return 0; }
uint8_t CPU::CPX() { return 0; }
uint8_t CPU::CPY() { return 0; }
uint8_t CPU::INC() { return 0; }
uint8_t CPU::INX() { return 0; }
uint8_t CPU::INY() { return 0; }
uint8_t CPU::DEC() { return 0; }
uint8_t CPU::DEX() { return 0; }
uint8_t CPU::DEY() { return 0; }
uint8_t CPU::ASL() { return 0; }
uint8_t CPU::LSR() { return 0; }
uint8_t CPU::ROL() { return 0; }
uint8_t CPU::ROR() { return 0; }
uint8_t CPU::JMP() { return 0; }
uint8_t CPU::JSR() { return 0; }
uint8_t CPU::RTS() { return 0; }
uint8_t CPU::RTI() { return 0; }
uint8_t CPU::BCC() { return 0; }
uint8_t CPU::BCS() { return 0; }
uint8_t CPU::BEQ() { return 0; }
uint8_t CPU::BNE() { return 0; }
uint8_t CPU::BMI() { return 0; }
uint8_t CPU::BPL() { return 0; }
uint8_t CPU::BVC() { return 0; }
uint8_t CPU::BVS() { return 0; }
uint8_t CPU::CLC() { return 0; }
uint8_t CPU::CLD() { return 0; }
uint8_t CPU::CLI() { return 0; }
uint8_t CPU::CLV() { return 0; }
uint8_t CPU::SEC() { return 0; }
uint8_t CPU::SED() { return 0; }
uint8_t CPU::SEI() { return 0; }
uint8_t CPU::BRK() { return 0; }
uint8_t CPU::NOP() { return 0; }
uint8_t CPU::XXX() { return 0; }

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

void CPU::clock() {
    if (cycles == 0) {
        uint8_t opcode = read(PC++);
        const Instruction& instr = lookup[opcode];

        cycles = instr.cycles;

        uint8_t extra1 = (this->*instr.addrmode)();
        uint8_t extra2 = (this->*instr.operate)();

        cycles += (extra1 & extra2);
    }

    cycles--;
}