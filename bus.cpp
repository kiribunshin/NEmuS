#include "bus.h"

Bus::Bus()
{
    //clear ram
    for (auto &i : ram) i = 0x00;
}

uint8_t Bus::read(uint16_t addr, bool bReadOnly) {
    if (addr < 0x0800) {
        return ram[addr];
    }
    return 0;
}

void Bus::write(uint16_t addr, uint8_t value) {
    if (addr < 0x0800) {
        ram[addr] = value;
    }
}