#include "system.h"

void System::clock() {
    cpu.clock();
    ppu.clock();
    ppu.clock();
    ppu.clock();
}

void System::reset() {
    cpu.reset();
    ppu.reset();
}