#include <iostream>
#include "Engine/Rave.h"
#include <memory>

rv::Result rv_main()
{
	std::cout << "Hello world!\n";
	std::cout << std::boolalpha;

	std::cin.ignore();

	return rv::success;
}