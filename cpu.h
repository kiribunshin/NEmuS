#pragma once
#include <cstdint>
#include "bus.h"

struct CPU {
    uint8_t A = 0;     //accumulator
    uint8_t X = 0;     //Index register X
    uint8_t Y = 0;     //Index register Y
    uint8_t SP = 0xFD; //Stack pointer (starts here)
    uint16_t PC = 0;   //program counter
    uint8_t P = 0x34;  //status flags

    Bus* bus = nullptr;

    // Addressing Modes
    uint16_t IMP();  uint16_t IMM(); uint16_t ACC();
    uint16_t ZP0();  uint16_t ZPX();
    uint16_t ZPY();  uint16_t REL();
    uint16_t ABS();  uint16_t ABX();
    uint16_t ABY();  uint16_t IND();
    uint16_t IZX();  uint16_t IZY();

    // Opcodes (operations) === return whether it needs extra cycles (0 or 1)
    uint8_t LDA(); uint8_t LDX(); uint8_t LDY();
    uint8_t STA(); uint8_t STX(); uint8_t STY();
    uint8_t TAX(); uint8_t TAY(); uint8_t TXA(); uint8_t TYA(); uint8_t TSX(); uint8_t TXS();
    uint8_t PHA(); uint8_t PHP(); uint8_t PLA(); uint8_t PLP();
    uint8_t AND(); uint8_t EOR(); uint8_t ORA(); uint8_t BIT();
    uint8_t ADC(); uint8_t SBC(); uint8_t CMP(); uint8_t CPX(); uint8_t CPY();
    uint8_t INC(); uint8_t INX(); uint8_t INY(); uint8_t DEC(); uint8_t DEX(); uint8_t DEY();
    uint8_t ASL(); uint8_t LSR(); uint8_t ROL(); uint8_t ROR();
    uint8_t JMP(); uint8_t JSR(); uint8_t RTS(); uint8_t RTI();
    uint8_t BCC(); uint8_t BCS(); uint8_t BEQ(); uint8_t BNE();
    uint8_t BMI(); uint8_t BPL(); uint8_t BVC(); uint8_t BVS();
    uint8_t CLC(); uint8_t CLD(); uint8_t CLI(); uint8_t CLV();
    uint8_t SEC(); uint8_t SED(); uint8_t SEI();
    uint8_t BRK(); uint8_t NOP();
    uint8_t XXX(); // illegal/unimplemented opcode catch-all

    uint16_t addr_abs = 0x0000;
    uint8_t  fetched = 0x00;
    uint8_t opcode = 0x00;
    uint8_t cycles = 0; // cycles remaining for the currently executing instruction
    uint64_t totalCycles = 0; //total cycles for logging purposes
    bool useAccumulator = false;

    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t data);
    uint8_t fetch(); // reads the value at addr_abs into `fetched`, and returns it
    void setFlag(uint8_t flag, bool value);
    bool getFlag(uint8_t flag) const;
    void updateZN(uint8_t value);
    void compare(uint8_t reg);
    void reset();
    void irq();
    void nmi();
    void trace();
    void clock(); //execute one instruction
};

struct Instruction {
    const char* name;              // e.g. "LDA" - just for debugging/logging
    uint16_t (CPU::*addrmode)();   // pointer to one of CPU's addressing mode functions
    uint8_t  (CPU::*operate)();    // pointer to one of CPU's opcode functions
    uint8_t  cycles;               // base cycle count for this instruction
};