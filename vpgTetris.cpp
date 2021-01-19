
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <iostream>
#include "CTetris.h"



int main() {
	CTetris game;
	game.Play();
	std::cout << "Game over...\n";
	std::cout << "Score: " << game.GetScore() << "\n";
	system("pause");
	return 0;
}