#include <cstdint>

class PPU {

public:
    uint16_t currentScanline = 0;
    uint16_t currentCycle = -1;

    bool frameComplete = false;


    void clock();
    void reset();
};

