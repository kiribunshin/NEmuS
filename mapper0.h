#pragma once
#include <cstdint>

class mapper0 {
public:
    mapper0(uint8_t prgBanks, uint8_t chrBanks);

    uint32_t cpuMapRead(uint16_t addr);
    uint32_t ppuMapRead(uint16_t addr);

private:
    uint8_t prgBanks;
    uint8_t chrBanks;
};
