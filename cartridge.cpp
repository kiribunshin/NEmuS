#include "cartridge.h"
#include "mapper0.h"
#include <fstream>
#include <cstdio>

// Layout of the 16-byte iNES header, by index:
//   0-3: magic number 'N' 'E' 'S' 0x1A
//   4:   PRG-ROM size, in 16384-byte units
//   5:   CHR-ROM size, in 8192-byte units
//   6:   flags6 (mapper low nibble, trainer-present bit, etc.)
//   7:   flags7 (mapper high nibble, etc.)
//   8-15: other flags, not needed yet

bool Cartridge::loadFromFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        printf("Cartridge: failed to open file: %s\n", path.c_str());
        return false;
    }

    // Read the 16-byte header
    std::vector<uint8_t> header(16);
    file.read(reinterpret_cast<char*>(header.data()), header.size());
    if (!file) {
        printf("Cartridge: failed to read header (file too short?)\n");
        return false;
    }

    // Validate magic number: 'N' 'E' 'S' 0x1A
    if (header[0] != 'N' || header[1] != 'E' || header[2] != 'S' || header[3] != 0x1A) {
        printf("Cartridge: invalid iNES header (bad magic number)\n");
        return false;
    }

    uint8_t prgSizeUnits = header[4]; // number of 16384-byte PRG-ROM chunks
    uint8_t chrSizeUnits = header[5]; // number of 8192-byte CHR-ROM chunks

    printf("Cartridge: PRG-ROM = %d x 16KB, CHR-ROM = %d x 8KB\n", prgSizeUnits, chrSizeUnits);

    // Mapper number: low nibble from header[6]'s upper bits, high nibble from header[7]'s upper bits
    mapperID = (header[7] & 0xF0) | (header[6] >> 4);
    printf("Cartridge: mapper = %d\n", mapperID);

    // Trainer: if flags6 bit 2 is set, a 512-byte trainer sits before PRG-ROM: skip it
    bool hasTrainer = header[6] & 0x04;
    if (hasTrainer) {
        file.seekg(512, std::ios::cur);
    }

    //vertical mirroring
    verticalMirroring = header[6] & 0x01;

    // Read PRG-ROM
    prgROM.resize(prgSizeUnits * 16384);
    file.read(reinterpret_cast<char*>(prgROM.data()), prgROM.size());
    if (!file) {
        printf("Cartridge: failed to read PRG-ROM (file too short?)\n");
        return false;
    }

    // Read CHR-ROM
    chrROM.resize(chrSizeUnits * 8192);
    file.read(reinterpret_cast<char*>(chrROM.data()), chrROM.size());
    if (!file) {
        printf("Cartridge: failed to read CHR-ROM (file too short?)\n");
        return false;
    }

    mapper = std::make_unique<mapper0>(prgSizeUnits, chrSizeUnits);

    return true;
}

uint8_t Cartridge::cpuRead(uint16_t addr) {
    uint32_t mapped = mapper->cpuMapRead(addr);
    return prgROM[mapped];
}

uint8_t Cartridge::ppuRead(uint16_t addr) {
    uint32_t mapped = mapper->ppuMapRead(addr);
    return chrROM[mapped];
}