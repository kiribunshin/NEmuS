#pragma once
#include <array>
#include "cartridge.h"

class PPU {

public:
    Cartridge* cartridge = nullptr;

    std::array<uint8_t, 2048> nameTable{};
    std::array<uint8_t, 32> paletteRAM{};

    uint16_t currentScanline = 0;
    uint16_t currentCycle = -1;

    uint8_t ctrl = 0x00;
    uint8_t mask = 0x00;

    bool frameComplete = false;

    uint8_t ppuRead(uint16_t addr);
    void ppuWrite(uint16_t addr, uint8_t data);
    uint8_t readRegister(uint8_t reg, bool readOnly = false);
    void writeRegister(uint8_t reg, uint8_t data);
    void clock();
    void reset();
};

