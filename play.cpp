#include "bitboard.hpp"
#include "Rand.cpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

// g++ bitboard.hpp bitboard.cpp play.cpp -std=c++14 -o run

int main()
{
	Engine* e = new Engine();
	Rand *white_player = new Rand(1, e);
	Rand *black_player = new Rand(0, e);

	max_moves  = e->get_max_move_length();
	int moves_made = 0;

	while(moves_made < max_moves)
	{
		std::cout << "White to move." << std::endl;
		
	}
}