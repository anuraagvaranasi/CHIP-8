#include "chip.h"
#include <iostream>
#include <regex>


int main(void){

	std::string s("0FED");
	std::regex exp("^0([0-9A-F]{3})$");
	std::smatch sm;
	std::regex_match(s,sm,exp);
	std::cout << sm[0] << "\n";

	
	return 0;
}