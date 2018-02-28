
#include "block.h"

int main( int argc, char** argv )
{

	BlockTest test;
	
	test.start();
	test.block();
	
	std::cout << "Main Thread" << std::endl;
	
	getchar();
	test.cancel();

	return 0;
}
