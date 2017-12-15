#include "chip.h"
#include <iostream>
#include <fstream>

int main(int argc, char **argv){

	Chip8 chip8;
	chip8.initialise(argv[1]); //probably add some safety incase called without arg
	chip8.debugInfo();
	chip8.emulate();
	chip8.debugMem();
	return 0;
}


//print debugging information
//(except for memory cause too big)
void Chip8::debugInfo(){
	std::cout << "opcode = " << opcode << "\n";
	std::cout << "V Registers: ";
	for(int x = 0;x < 16;++x){
		std::cout << (int)V[x] << " "; 
	}
	std::cout << "\n";
	std::cout << "I = " << I << "\n";
	std::cout << "PC = " << PC << "\n";
	std::cout << "delay timer = " << (int)delay_timer << "\n";
	std::cout << "sound timer = " << (int)sound_timer << "\n";
	std::cout << "stack: ";
	for(int x = 0;x < 16;++x){
		std::cout << stack[x] << " ";
	}
	std::cout << "\n";
	std::cout << "SP = " << SP << "\n";

}

//only the memory since it covers whole screen
void Chip8::debugMem(){
	for(int x = 0; x < 4096; ++x){
		std::cout << std::hex << (int)memory[x];
	}
	std::cout << "\n";
}
//initial system setup
void Chip8::initialise(char* game){
	//first clear all memory
	opcode = 0;
	for(int x = 0;x < 4096;++x){
		memory[x] = 0;
	}
	for(int x = 0;x < 16;++x){
		V[x] = 0;
	}
	I = 0;
	PC = 0x200;//program counter stars at mem 0x200
	for(int y = 0;y < 32;++y){
		for(int x = 0;x < 64;++x){
			gfx[x][y] = 0;
		}
	}
	delay_timer = 0;
	sound_timer = 0;
	for(int x = 0;x < 16;++x){
		stack[x] = 0;
	}
	SP = 0;
	for(int x = 0;x < 16;++x){
		keypad[x] = 0;
	}

	//load chip8 fontset into memory
	for(int x = 0;x < 80;++x){
		memory[x] = chip8_fontset[x];
	}

	//concatenate string with .rom
	std::string file = std::string(game) + ".c8";
	//now load game
	std::ifstream fs;
	fs.open(file);
	char c;
	int index = 0;
	while(fs >> c){
		memory[index+512] = c;
		index++;
	}
	fs.close();
}

//print display
void Chip8::printScreen(){
	for(int x = 0;x < 50;++x) std::cout << "\n";
	for(int y = 0;y < 32;++y){
		for(int x = 0;x < 64;++x){
			if(gfx[x][y]) std::cout << "*";
			else std::cout << " ";
		}
		std::cout << "\n";
	}
}

//emulate a cycle
void Chip8::emulate(){
	//Fetch
	//Decode
	//Execute

	//Fetch:
	//break into 2 parts, since its a 2 byte code
	//first byte is all we need to determine opcode function too
	opcode1 = memory[PC];
	opcode2 = memory[PC+1];
	//Decode and Execute done in a switch statement
}