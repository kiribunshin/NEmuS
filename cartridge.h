#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include "mapper0.h"

class Cartridge {
public:
    std::vector<uint8_t> prgROM;
    std::vector<uint8_t> chrROM;
    uint8_t mapperID = 0;
    std::unique_ptr<mapper0> mapper;

    bool loadFromFile(const std::string& path);
    bool verticalMirroring = false;

    uint8_t cpuRead(uint16_t addr);
    uint8_t ppuRead(uint16_t addr);
};