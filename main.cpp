#include <cstdio>
#include "cartridge.h"
#include "cpu.h"

int main() {
    Cartridge cart;
    if (!cart.loadFromFile("nestest.nes")) {
        printf("Failed to load nestest.nes\n");
        return 1;
    }
    freopen("test.log", "w", stdout);

    CPU cpu;
    Bus bus;
    cpu.bus = &bus;

    bus.prgROM = cart.prgROM;

    cpu.reset();
    cpu.PC = 0xC000; // nestest automation entry point

    // burn off reset()'s initial cycle count before real execution begins
    while (cpu.cycles > 0) cpu.clock();

    cpu.totalCycles = 7; // nestest's logging convention starts at 7, set AFTER the burn-off

    for (int i = 0; i < 10000; i++) {
        do { cpu.clock(); } while (cpu.cycles > 0);
    }

    return 0;
}