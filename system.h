#pragma once
#include "bus.h"
#include "cpu.h"
#include "ppu.h"

class System {
public:
    CPU cpu;
    PPU ppu;
    Bus bus;

    System(){
        cpu.bus = &bus;
        bus.ppu = &ppu;
    }

    void clock();
    void reset();
};

