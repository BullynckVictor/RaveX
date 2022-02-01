#include <iostream>

double square(double x)
{
	return x * x;
}

double m(unsigned int day)
{
	return double(10 - day + 1);
}

double calculate_first(unsigned int day_orange)
{
	if (day_orange == 1)
	{
		std::cout << "(2 / 10) * ";
		return 2.0 / 10.0;
	}
	else
	{
		std::cout << "(8 / 10) * ";
		return 8.0 / 10.0;
	}
}

double calculate_day_before(unsigned int day)
{
	double m = ::m(day);
	std::cout << "(" << square(m) - 4 << " / " << square(m) << ") * ";
	return (square(m) - 4) / square(m);
}

double calculate_day_orange(unsigned int day_orange)
{
	double m = ::m(day_orange);
	std::cout << "(" << 4 << " / " << square(m) << ") * ";
	return 4.0 / square(m);
}

double calculate_day_right_after(unsigned int day_orange)
{
	if (day_orange != 9)
	{
		double m = ::m(day_orange + 1);
		std::cout << " * (" << square(m - 1.0) << " / " << square(m) << ")";
		return square((m - 1.0) / m);
	}
	else
		return 1.0;
}

double calculate_day_after(unsigned int day)
{
	double m = ::m(day);
	std::cout << " * (" << square(m) - 1.0 << " / " << square(m) << ")";
	return (square(m) - 1.0) / square(m);
}

double calculate_probability(unsigned int day_orange)
{
	double probability = calculate_first(day_orange);

	if (day_orange != 1)
	{
		for (unsigned int day = 2; day < day_orange; ++day)
			probability *= calculate_day_before(day);

		probability *= calculate_day_orange(day_orange);
	}

	probability *= calculate_day_right_after(day_orange);

	for (unsigned int day = day_orange + 2; day < 10; ++day)
		probability *= calculate_day_after(day);

	return probability;
}

int main()
{
	double probability = 0.0;
	for (unsigned int day_orange = 1; day_orange < 10; ++day_orange)
	{
		std::cout << "Dag " << day_orange << ": \n";
		double p = calculate_probability(day_orange);
		std::cout << '\n';
		probability += p;
		std::cout << p << '\n';
	}
	std::cout << "Uiteindelijk: " << probability;
	std::cin.ignore();
}