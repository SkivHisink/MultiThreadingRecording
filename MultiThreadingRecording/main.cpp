#include <clocale>

#include "Application.hpp"

int main()
{
	SetConsoleCP(1251); // configure console encoding (input)
	SetConsoleOutputCP(1251); // (output)

	// set up internal mechanisms for this encoding
	setlocale(LC_ALL, ".1251");

	Application App;
	App.init();
	App.start();
	return 0;
}
