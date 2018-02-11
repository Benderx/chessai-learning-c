#include "bitboard.hpp"
#include "player.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	Engine* e = new Engine();
	Rand *white_player = new Rand(1,e);
	Rand *black_player = new Rand(0,e);
}