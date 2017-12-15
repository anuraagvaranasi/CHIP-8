#ifndef CHIP_8
#define CHIP_8


class Chip8{
public:
	void initialise(char* game);
	void printScreen();
	void debugInfo();
	void debugMem();
	void emulate();
private:
	unsigned short opcode;//current opcode
	unsigned char memory[4096];//4K memory emulated
	unsigned char V[16];//V0-14 is registers, V15 is carry flag
	unsigned short I;//index register
	unsigned short PC;//program counter
	unsigned char gfx[64][32];//pixels of chip8, black/white is 0/1
	unsigned char delay_timer;
	unsigned char sound_timer;//both timers count down at 60hz
	unsigned short stack[16];//only 16 bytes, anything more overflows
	unsigned short SP;//stack pointer
	unsigned char keypad[16];

	unsigned char chip8_fontset[80] =
{ 
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

};


#endif