#include "bitboard.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

class Rand
{
	public:

		int color;
		Engine *e;

		Rand(int col, Engine *engine)
		{
			color = col;
			e = engine;
		}

		int get_color()
		{
			return(this->color);
		}

		unsigned long long move()
		{
			std::cout << e->generate_legal_moves(color) << std::endl;
		}
};