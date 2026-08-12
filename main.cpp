#include <cstdio>
#include "cartridge.h"
#include "system.h"

int main() {
    System nes;
    Cartridge cart;
    if (!cart.loadFromFile("nestest.nes")) {
        printf("Failed to load ROM\n");
        return -1;
    }
    nes.bus.cartridge = &cart;

    nes.reset();

    // burn off reset()'s initial cycle count before real execution begins
    while (nes.cpu.cycles > 0) nes.clock();

    for (int i = 0; i < 10000; i++) {
        nes.clock();
    }

    printf("CPU total cycles: %llu\n", nes.cpu.totalCycles);
    printf("PPU scanline: %d, cycle: %d\n", nes.ppu.currentScanline, nes.ppu.currentCycle);

    return 0;
}