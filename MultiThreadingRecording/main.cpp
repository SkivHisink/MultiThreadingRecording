#include "Application.hpp"

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "ru_RU.UTF-8");

	Application App;
	if (App.init()) {
		App.start();
	}
	return 0;
}