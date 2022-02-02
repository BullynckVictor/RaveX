#include <iostream>
#include "Engine/Rave.h"
#include <memory>
#include <comdef.h>
#include <fstream>

constexpr float foo()
{
	int z = 5;
	rv::Vector2 vec1 = z;
	rv::Vector3 vec2 = rv::Vector3(5, 7.0, 9.0f);
	vec2 += 2.1;

	return vec2.z;
}

template<float F>
struct A { static constexpr float value = F; };

rv::Result rv_main()
{
	rv_result;
	std::cout << A<foo()>::value;
	std::cin.ignore();
	return result;
}