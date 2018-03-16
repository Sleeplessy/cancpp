#include "../basic_can_interface.hpp"
#include <iostream>
int main(){
	try{
	CAN::can_interface can("can0");
	}catch(...){
		throw;
	}
	std::cout <<std::hex<< can.read().can_id<<std::endl;
	return 0;
}
