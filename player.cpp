#include "player.hpp"
#include "bitboard.hpp"
#include <stdlib.h>

Player::Player(int col, Engine* engine)
{
	color = col;
	e = engine;
}

int Player::get_color()
{
	return(this->color);
}


Rand::Rand(int col, Engine* engine) : Player(col, engine)
{
	//
}

unsigned long long Rand::move()
{
	int* move_list = e->generate_legal_moves(color);
	int num_moves = move_list[0];
	move_list++;


	std::cout << "yo" << std::endl;
}

// class Rand
// {
// 	public:

// 		int color;
// 		Engine* e;

// 		Rand(int col, Engine* engine)
// 		{
// 			color = col;
// 			e = engine;
// 		}

// 		int get_color()
// 		{
// 			return(this->color);
// 		}

// 		unsigned long long move()
// 		{
// 			std::cout << e->generate_legal_moves(color) << std::endl;
// 		}
// };