#include "ppu.h"
#include "cartridge.h"

// === PPU Flags ===

//PPUCTRL
constexpr uint8_t CTRL_NAMETABLE_X      = (1 << 0); // base nametable select, bit 0
constexpr uint8_t CTRL_NAMETABLE_Y      = (1 << 1); // base nametable select, bit 1
constexpr uint8_t CTRL_INCREMENT_MODE   = (1 << 2); // 0: add 1 (going across), 1: add 32 (going down)
constexpr uint8_t CTRL_SPRITE_PATTERN   = (1 << 3); // sprite pattern table address (0: $0000, 1: $1000), ignored in 8x16 mode
constexpr uint8_t CTRL_BG_PATTERN       = (1 << 4); // background pattern table address (0: $0000, 1: $1000)
constexpr uint8_t CTRL_SPRITE_SIZE      = (1 << 5); // 0: 8x8, 1: 8x16
constexpr uint8_t CTRL_MASTER_SLAVE     = (1 << 6); // PPU master/slave select: irrelevant on NES, but real games read it
constexpr uint8_t CTRL_NMI_ENABLE       = (1 << 7); // generate NMI at start of vblank
//PPUMASK
constexpr uint8_t MASK_GREYSCALE          = (1 << 0); // 0: normal color, 1: greyscale
constexpr uint8_t MASK_SHOW_BG_LEFT       = (1 << 1); // show background in leftmost 8 pixels
constexpr uint8_t MASK_SHOW_SPRITES_LEFT  = (1 << 2); // show sprites in leftmost 8 pixels
constexpr uint8_t MASK_SHOW_BACKGROUND    = (1 << 3); // enable background rendering
constexpr uint8_t MASK_SHOW_SPRITES       = (1 << 4); // enable sprite rendering
constexpr uint8_t MASK_EMPHASIZE_RED      = (1 << 5); // color emphasis, red
constexpr uint8_t MASK_EMPHASIZE_GREEN    = (1 << 6); // color emphasis, green
constexpr uint8_t MASK_EMPHASIZE_BLUE     = (1 << 7); // color emphasis, blue

uint8_t PPU::ppuRead(uint16_t addr) {
    addr &= 0x3FFF;
    if (addr <= 0x1FFF) {
        return cartridge->ppuRead(addr);
    }
    else if (addr >= 0x2000 && addr <= 0x3EFF) {
        uint16_t relative = addr & 0x0FFF;          // 0 - 4095
        uint16_t nametableIndex = relative / 0x400; // 0, 1, 2, 3
        uint16_t offset = relative & 0x3FF;
        if (cartridge->verticalMirroring) {
            uint8_t physicalNametable = nametableIndex & 0x1;
            return nameTable[physicalNametable * 0x400 + offset];
        }
        uint8_t physicalNametable = (nametableIndex >> 1) & 0x1;
        return nameTable[physicalNametable * 0x400 + offset];
    }
    else {
        uint8_t relative = addr & 0x1F;
        return paletteRAM[relative];
    }
}

void PPU::ppuWrite(uint16_t addr, uint8_t data) {
    addr &= 0x3FFF;
    if (addr <= 0x1FFF) {
        //CHR-RAM SUPPORT
        return;
    }
    else if (addr >= 0x2000 && addr <= 0x3EFF) {
        uint16_t relative = addr & 0x0FFF;          // 0 - 4095
        uint16_t nametableIndex = relative / 0x400; // 0, 1, 2, 3
        uint16_t offset = relative & 0x3FF;
        if (cartridge->verticalMirroring) {
            uint8_t physicalNametable = nametableIndex & 0x1;
            nameTable[physicalNametable * 0x400 + offset] = data;
        }
        else {
            uint8_t physicalNametable = (nametableIndex >> 1) & 0x1;
            nameTable[physicalNametable * 0x400 + offset] = data;
        }
    }
    else {
        uint8_t relative = addr & 0x1F;
        paletteRAM[relative] = data;
    }
}

uint8_t PPU::readRegister(uint8_t reg, bool readOnly) {
    switch (reg) {
        case 0x00:
            return 0;
        default:
            return 0;
    }
}

void PPU::writeRegister(uint8_t reg, uint8_t data) {
    switch (reg) {
        case 0x00:
            ctrl = data;
            break;
        default:
            break;
    }
}

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