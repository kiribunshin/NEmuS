#include "ppu.h"

void PPU::clock() {
    currentCycle++;
    frameComplete = false;
    if (currentCycle == 341) {
        currentScanline++;
        currentCycle = 0;
    }
    if (currentScanline == 262) {
        frameComplete = true;
        currentScanline = 0;
    }
}

void PPU::reset() {
    currentScanline = 0;
    currentCycle = 0;
    frameComplete = false;
}