#pragma once
#include <cstdint>
#include <vector>
#include <string>

class Cartridge {
public:
    std::vector<uint8_t> prgROM;
    std::vector<uint8_t> chrROM;
    uint8_t mapperID = 0;

    // Returns true if the file loaded and parsed successfully
    bool loadFromFile(const std::string& path);
};