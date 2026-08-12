#pragma once
#include <array>
#include "cartridge.h"
#include "ppu.h"

class Bus {
public:
    std::array<uint8_t, 2048> ram{}; // 2KB internal RAM (0x0000–0x07FF)
    Cartridge* cartridge = nullptr;
    PPU* ppu = nullptr;
    // APU* apu = nullptr;

    Bus();

    uint8_t read(uint16_t addr, bool bReadOnly);
    void write(uint16_t addr, uint8_t data);
};