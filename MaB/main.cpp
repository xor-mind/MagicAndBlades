#include <SDL.h>
#include "MaB.h"
////#include "iostream_app.h"
#include <iostream>
#include <memory>
#include <string>
#include "resource.h"


int SDL_main(int argc, char *argv[])
{
	std::unique_ptr<App> a(new MaB); int r = a->Execute();
	std::cout << "Exitting!" << std::endl; std::cin.get();
	return r;
}