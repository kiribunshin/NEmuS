#pragma once
#include <cstdint>
#include <array>
#include <vector>

class Bus {
public:
    std::array<uint8_t, 2048> ram{}; // 2KB internal RAM (0x0000–0x07FF)
    std::vector<uint8_t> prgROM;
    // Placeholders for future subsystems:
    // PPU* ppu = nullptr;
    // APU* apu = nullptr;

    Bus();

    uint8_t read(uint16_t addr, bool bReadOnly);
    void write(uint16_t addr, uint8_t data);
};