#include "chip.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <bitset>
#include <unistd.h>
#include <iomanip>
#include <mutex>
#include <chrono>

#define SCREEN_MULTIPLE 20

std::mutex m;


int main(int argc, char **argv){

	Chip8 chip8;
	chip8.initialise(argv[1]); //probably add some safety incase called without arg
	chip8.window.create(sf::VideoMode(64*SCREEN_MULTIPLE,32*SCREEN_MULTIPLE), "Chip8 Emulator");

	
	while(chip8.window.isOpen()){
		//first emulate a cycle (and print to screen)
		chip8.emulate();
		//then deal with events
		sf::Event event;
        while (chip8.window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed){
                chip8.window.close();
            }
        }
        chip8.checkKeys();

	}
	
	return 0;
}


//print debugging information
//(except for memory cause too big)
void Chip8::debugInfo(){
	std::cout << "opcode = " << std::setw(4) << std::setfill('0') << std::hex << opcode << "\n";
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
	int debug_until = 1000;
	for(int x = 0; x < debug_until; ++x){
		if(x%0x10==0)std::cout << "\n" << std::setw(3) << std::setfill('0') << x << " ";
		std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)memory[x] << " ";
	}
	std::cout << "\n";
}

//show graphics array
void Chip8::debugScreen(){
	for(int y = 0;y < 32;++y){
		for(int x = 0;x < 64;++x){
			std::cout << (int)screen[x][y];
		}
		std::cout << "\n";
	}
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
	//some reason C++ libraries just give errors, so back to the old days!
	FILE *fp = fopen(file.c_str(),"r");
	short c = fgetc(fp);
	int x = 0;

	while(c != -1){
		memory[512+x] = c;
		++x;
		c = fgetc(fp);
	}

	//setup timers thread
	auto t1 = std::thread(&Chip8::timers,this);
	t1.detach();

	//setup random number generator
	srand(time(NULL));	
	
}

//call in a seperate thread
void Chip8::timers(){
	while(true){
		m.lock();
		if(delay_timer != 0){
			--delay_timer;
		}
		if(sound_timer != 0){
			--sound_timer;
			//supposed to beep here but whatever
		}
		m.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(1000/60));
	}
}

//check input
void Chip8::checkKeys(){

	for(int x = 0;x < 16;++x){
		keypad[x] = 0;
	}
	
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)){
        keypad[0] = 1;
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)){
    	keypad[1] = 1;
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)){
    	keypad[2] = 1;
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)){
    	keypad[3] = 1;
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Q)){
    	keypad[4] = 1;
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
    	keypad[5] = 1;
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::E)){
    	keypad[6] = 1;
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::R)){
    	keypad[7] = 1;
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
    	keypad[8] = 1;
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
    	keypad[9] = 1;
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
    	keypad[0xA] = 1;
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::F)){
    	keypad[0xB] = 1;
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Z)){
    	keypad[0xC] = 1;
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::X)){
    	keypad[0xD] = 1;
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::C)){
    	keypad[0xE] = 1;
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::V)){
    	keypad[0xF] = 1;
    }


}

//print display
void Chip8::printScreen(){
    window.clear();
	for(int y = 0;y < 32;++y){
		for(int x = 0;x < 64;++x){

			sf::RectangleShape shape(sf::Vector2f(SCREEN_MULTIPLE, SCREEN_MULTIPLE));
		    shape.setPosition(x*SCREEN_MULTIPLE,y*SCREEN_MULTIPLE);
			if(screen[x][y]) shape.setFillColor(sf::Color::Green);
			else shape.setFillColor(sf::Color::Black);
		    window.draw(shape);
			
		}
	}
    window.display();
}

//emulate a cycle
void Chip8::emulate(){
	//Fetch
	//Decode
	//Execute

	//Fetch:
	//break into 2 parts, since its a 2 byte code
	//first byte is usually to determine opcode function 
	opcode = memory[PC] << 8 | memory[PC+1];
	char opcode_byte = (opcode&0xF000) >> 12;//shift it 3 bytes
	//Decode and Execute done in a switch statement
	switch(opcode_byte){
		case 0x0://3 different opcodes for 0
			//00E0 = clear screen
			if((opcode&0xFF) == 0xE0){
				for(int y = 0;y < 32;++y){
					for(int x = 0;x < 64;++x){
						screen[x][y] = 0;
					}
				}
				PC+=2;
				printScreen();
			}
			//00EE = return
			else if((opcode&0x00FF) == 0xEE){
				--SP;
				PC = stack[SP];
				stack[SP] = 0;//clear stack (shouldnt be needed but incase)
				PC+=2;
			}
			//call 0nnn, where nnn is address (not usually used for some reason)
			//for machine code things (dont know how implentation would differ tho)
			else{
				stack[SP] = PC;
				++SP;
				PC = opcode&0x0FFF;
			}
			break;
		case 0x1: //jump to nnn
			PC = opcode&0x0FFF;
			break;
		case 0x2: //call subroutine at nnn
			stack[SP] = PC;
			++SP;
			PC = opcode&0x0FFF;
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
			break;
		case 0x5://0x5xy0, skips next instruction if V[x] == V[y]
			if(V[(opcode&0xF00)>>8] == V[(opcode&0x00F0)>>4]){
				PC+=2;
			}
			PC+=2;
			break;
		case 0x6://0x6xkk, sets V[x] to kk
			V[(opcode&0x0F00)>>8] = (opcode&0x00FF);
			PC+=2;
			break;
		case 0x7://0x7xkk, adds kk to V[x]
			V[(opcode&0x0F00)>>8] += opcode&0x00FF;
			PC+=2;
			break;
		case 0x8:{//has 9 different instructions
			char last_bit = opcode&0x000F;//instruction dependant on last bit
			short x = (opcode&0x0F00)>>8;//easier than writing opcode&blabla everytime
			short y = (opcode&0x00F0)>>4;//above
			if(last_bit == 0x0){//set Vx to Vy
				V[x] = V[y];
			}
			else if(last_bit == 0x1){//Vx = Vx OR(|) Vy
				V[x] = V[x] | V[y];
			}
			else if(last_bit == 0x2){//Vx = Vx AND(&) Vy
				V[x] = V[x] & V[y];
			}			
			else if(last_bit == 0x3){//Vx = Vx XOR(^) Vy
				V[x] = V[x] ^ V[y];
			}
			else if(last_bit == 0x4){//Vx = Vx+Vy (set Vf to carry)
				if((V[x] + V[y]) > 0xFF) V[0xF] = 1;
				V[x] = V[x] + V[y];
			}
			else if(last_bit == 0x5){//Vx = Vx-Vy, set Vf to NOT borrow
				if(V[x] > V[y]) V[0xF] = 1;
				V[x] = V[x] - V[y];
			}
			else if(last_bit == 0x6){//set Vf to least sig byte of Vx, then shift Vx right
				V[0xF] = V[x]&0b00000001;
				V[x] = V[x]>>1;
			}
			else if(last_bit == 0x7){//Vx = Vy - Vx, Vf set to NOT borrow
				if(V[y] > V[x]) V[0xF] = 1;
				V[x] = V[y] - V[x];
			}
			else if(last_bit == 0xE){//set Vf to most sig byte of Vx, then shift Vx left
				V[0xF] = (V[x]&0b10000000)>>7;
				V[x] = V[x]<<1;
			}
			else{
				std::cerr << "An error has occurred. An opcode that does not exist has been called\n";
				std::cerr << "Opcode was " << std::hex << opcode << "\n";
				exit(1);
			}

			PC+=2;
			break;
		}
		case 0x9://skip next instruction if V[x] != V[y]
			if((V[opcode&0x0F00])>>8 != V[(opcode&0x00F0)>>4]) PC += 2;
			PC+=2;
			break;
		case 0xA://Annn, sets I to nnn
			I = opcode&0x0FFF;
			PC+=2;
			break;
		case 0xB://0xBnnn, set PC to nnn + V0
			PC = opcode&0x0FFF;
			PC+= V[0];
			break;
		case 0xC://Cxkk, set Vx = kk AND(&) a random byte
			V[(opcode&0x0F00)>>8] = (opcode&0x00FF)&(rand()%256);	
			PC+=2;
			break;
		case 0xD:{//Draw a sprite(look up exact definition, too large)
			unsigned short x = V[(opcode & 0x0F00) >> 8];
			unsigned short y = V[(opcode & 0x00F0) >> 4];
			unsigned short height = opcode & 0x000F;
			unsigned short pixel;

			V[0xF] = 0;
			for (int yline = 0; yline < height; yline++)
			{
				pixel = memory[I + yline];
				for(int xline = 0; xline < 8; xline++)
				{
					if((pixel & (0x80 >> xline)) != 0)
					{
						if(screen[x + xline][y + yline] == 1)
						{
							V[0xF] = 1;                                    
						}
						screen[x + xline][y + yline] ^= 1;
					}
				}
			}
						
			printScreen();			
			PC += 2;
		}
		break;

		case 0xE://has 2 instructions
			if((opcode&0x00F0)>>4 == 0x9){//skip if key Vx is pressed
				if(keypad[V[(opcode&0x0F00)>>8]] == 1){
					PC+=2;
				}
			} 
			else if((opcode&0x00F0)>>4 == 0xA){//skip if not pressed
				if(keypad[V[(opcode&0x0F00)>>8]] == 0){
					PC+=2;
				}
			}
			PC+=2;
			break;
		case 0xF:{//has 9 instructions
			char last_byte = opcode&0x00FF;//last byte determines instruction
			short x = (opcode&0x0F00)>>8;//so we dont need to keep calculating x
			
			if(last_byte == 0x07){ //store delay timer in Vx
				V[x] = delay_timer;
			}
			else if(last_byte == 0x0A){//wait for a key press, and store that into Vx
				for(int i = 0;i < 16;++i){
					if(keypad[i] == 1){
						PC+=2;
						V[x] = i;
						break;
					}
				}
			}
			else if(last_byte == 0x15){//delay timer = Vx
				delay_timer = V[x];
			}
			else if(last_byte == 0x18){//set sound timer to Vx
				sound_timer = V[x];
			}
			else if(last_byte == 0x1E){ //I = I + V[x]
				I += V[x];
			}
			else if(last_byte == 0x29){//set I to location of sprite for digit V[x]
				I = V[x] * 5;//each digit sprite is 5 Bytes long
			}
			else if(last_byte == 0x33){//stores BCD of Vx in I/I+1/I+2
				short BCD = V[x];
				memory[I+2] = BCD%10;
				BCD /= 10;
				memory[I+1] = BCD%10;
				BCD /= 10;
				memory[I] = BCD%10;
			}
			else if(last_byte == 0x55){//store V0 -> Vx in memory I->I+x
				for(int counter = 0;counter <= x;++counter){
					memory[I+counter] = V[counter];
				}
				//also I = I + x + 1
				I = I + x + 1;
			}
			else if(last_byte == 0x65){//opposite of above
				for(int counter = 0;counter <= x;++counter){
					V[counter] = memory[I+counter];
				}	
				I = I + x + 1;
			}
			else{
				std::cerr << "An error has occurred. An opcode that does not exist has been called\n";
				std::cerr << "Opcode was " << std::hex << opcode << "\n";
				exit(1);
			}
			
			PC+=2;
		}
		break;
		default: 
			std::cerr << "An error has occurred. An opcode that does not exist has been called\n";
			std::cerr << "Opcode was " << std::hex << opcode << "\n";
			exit(1);
	}
}
