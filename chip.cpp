#include "chip.h"
#include <iostream>
#include <fstream>

int main(int argc, char **argv){

	Chip8 chip8;
	chip8.initialise(argv[1]); //probably add some safety incase called without arg
	chip8.debugInfo();
	chip8.emulate();
	chip8.debugInfo();
	return 0;
}


//print debugging information
//(except for memory cause too big)
void Chip8::debugInfo(){
	std::cout << "opcode = " << std::hex << opcode << "\n";
	std::cout << "V Registers: ";
	for(int x = 0;x < 16;++x){
		std::cout << std::hex << (int)V[x] << " "; 
	}
	std::cout << "\n";
	std::cout << "I = " << I << "\n";
	std::cout << "PC = " << std::hex << PC << "\n";
	std::cout << "delay timer = " << (int)delay_timer << "\n";
	std::cout << "sound timer = " << (int)sound_timer << "\n";
	std::cout << "stack: ";
	for(int x = 0;x < 16;++x){
		std::cout << std::hex << stack[x] << " ";
	}
	std::cout << "\n";
	std::cout << "SP = " << SP << "\n\n";

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
			screen[x][y] = 0;
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
			if(screen[x][y]) std::cout << "*";
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
	//first byte is usually to determine opcode function 
	memory[PC] = 0x3A;
	memory[PC+1] = 0xEE;
	opcode = memory[PC] << 8 | memory[PC+1];
	char opcode_byte = (opcode & 0xF000) >> 12;//shift it 3 bytes
	//Decode and Execute done in a switch statement
	switch(opcode_byte){
		case 0x0://3 different opcodes for 0
			//00E0 = clear screen
			if((opcode & 0xFF) == 0xE0){
				for(int y = 0;y < 32;++y){
					for(int x = 0;x < 64;++x){
						screen[x][y] = 0;
					}
				}
				PC+=2;
			}
			//00EE = return
			else if((opcode & 0x00FF) == 0xEE){
				--SP;
				PC = stack[SP];
				stack[SP] = 0;//clear stack (shouldnt be needed but incase)
			}
			//call 0nnn, where nnn is address (not usually used for some reason)
			//for machine code things (dont know how implentation would differ tho)
			else{
				stack[SP] = PC;
				++SP;
				PC = opcode & 0x0FFF;
			}
			break;
		case 0x1: //jump to nnn
			PC = opcode & 0x0FFF;
			break;
		case 0x2: //call subroutine at nnn
			stack[SP] = PC;
			++SP;
			PC = opcode & 0x0FFF;
			break;
		case 0x3://0x3xvv, checks if V[x] == vv
			if(V[(opcode&0x0F00)>>8] == (opcode&0x00FF)){
				PC+=2;
			}
			PC+=2;
			break;
		case 0x4://opposite of above
			if(V[(opcode&0x0F00)>>8] != (opcode&0x00FF)){
				PC+=2;
			}
			PC+=2;
		case 0x5://0x5xy0, skips next instruction if V[x] == V[y]
			if(V[(opcode&0xF00)>>8] == V[(opcode&0x00F0)>>4]){
				PC+=2;
			}
			PC+=2;
		case 0x6:
			

		default: 
			std::cerr << "An error has occurred. An opcode that does not exist has been called\n";
			std::cerr << "Opcode was " << opcode << "\n";
			//exit(1);
	}
}