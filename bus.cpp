#include "bus.h"

Bus::Bus()
{
    //clear ram
    for (auto &i : ram) i = 0x00;
}

uint8_t Bus::read(uint16_t addr, bool bReadOnly) {
    if (addr <= 0x1FFF) {
        // RAM, mirrored every 0x0800 (X & N-1 is identical to X % N and more easily calculated on hardware)
        return ram[addr & 0x07FF];
    }
    else if (addr <= 0x3FFF) {
        return ppu->readRegister(addr & 0x0007, bReadOnly);
    }
    else if (addr <= 0x4017) {
        // APU + I/O registers
        return 0; // stub until APU/controller input exists
    }
    else if (addr <= 0x401F) {
        // APU/IO test mode
        return 0;
    }
    else if (addr <= 0x5FFF) {
        // Expansion ROM / mapper-specific
        return 0;
    }
    else if (addr <= 0x7FFF) {
        // Cartridge RAM (PRG-RAM)
        return 0;
    }
    else {
        return cartridge->cpuRead(addr);
    }
}

void Bus::write(uint16_t addr, uint8_t value) {
    if (addr <= 0x1FFF) {
        ram[addr & 0x07FF] = value;
    }
    else if (addr <= 0x3FFF) {
        ppu->writeRegister(addr & 0x0007, value);
    }
    else if (addr <= 0x4017) {
        // APU + I/O registers
    }
    else if (addr <= 0x401F) {
        // APU/IO test mode
    }
    else if (addr <= 0x5FFF) {
        // Expansion ROM / mapper-specific
    }
    else if (addr <= 0x7FFF) {
        // Cartridge PRG-RAM
    }
    // else: 0x8000-0xFFFF is PRG-ROM — read-only, writes are ignored
}