#ifndef CHIP_8
#define CHIP_8


class Chip8{
public:

private:
	unsigned short opcode;//stores current opcode
	unsigned char memory[4096];//4K memory emulated
	unsigned char V[16];//V0-14 is registers, V15 is carry flag
	unsigned short I;//index register
	unsigned short PC;//program counter
	unsigned char gfx[64*32];//pixels of chip8, black/white is 0/1
	unsigned char delay_timer;//timer for delay
	unsigned char sound_timer;//both timers count down at 60hz
	unsigned short stack[16];//stack of 16 bytes
	unsigned short sp;//stack pointer
	unsigned char key[16];//keypad

};


#endif