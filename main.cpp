#include <cstdio>

#include "cpu.h"

int main() {
    CPU cpu;
    Bus bus;
    cpu.bus = &bus;

    //test program
    bus.ram[0x0000] = 0xA9; //LDA IMM
    bus.ram[0x0001] = 0x42; //Byte to load

    cpu.PC = 0x0000;
    do { cpu.clock(); } while (cpu.cycles > 0);

    printf("A = 0x%02X\n", cpu.A);
    printf("Z Flag = %d\n", cpu.getFlag(0x02));
    printf("N Flag = %d\n", cpu.getFlag(0x80));

    return 0;
}