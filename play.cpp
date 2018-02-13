#include "bitboard.hpp"
#include "player.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <limits>

// g++ bitboard.hpp bitboard.cpp player.hpp player.cpp play.cpp -std=c++14 -o run

int main()
{
	Engine* e = new Engine();
	Rand* white_player = new Rand(1, e);
	Rand* black_player = new Rand(0, e);

	int max_moves  = e->get_max_move_length();
	int moves_made = 0;

	while(moves_made < max_moves-1)
	{
		std::cout << "White to move." << std::endl;
		
		std::cin.ignore( std::numeric_limits <std::streamsize> ::max(), '\n' );
		moves_made++;
	}
}