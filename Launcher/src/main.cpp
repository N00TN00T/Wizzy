#include <iostream>
#include <Wizzy/Entry.h>

int main(int argc, char** argv) {
	std::cout << "Hit enter to start Wizzy engine..." << std::endl;
	std::cin.get();
	start_wizzy(argc, argv);
	return 0;
}
