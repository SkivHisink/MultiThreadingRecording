#include <clocale>

#include "Application.hpp"

int main()
{
	SetConsoleCP(1251); // настраиваем кодировку консоли (ввод)
	SetConsoleOutputCP(1251); // (вывод)

	// настраиваем внутренние механизмы на эту кодировку
	setlocale(LC_ALL, ".1251");

	Application App;
	App.init();
	App.start();
	return 0;
}
