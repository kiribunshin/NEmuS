#include "mapper0.h"

mapper0::mapper0(uint8_t prgBanks, uint8_t chrBanks) : prgBanks(prgBanks), chrBanks(chrBanks) {}

uint32_t mapper0::cpuMapRead(uint16_t addr) {
    if (prgBanks == 1) {
        return (addr & 0x7FFF) & 0x3FFF;
    }
    return addr & 0x7FFF;
}

uint32_t mapper0::ppuMapRead(uint16_t addr) {
    return addr & 0x1FFF;
}
