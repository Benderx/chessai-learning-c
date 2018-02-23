#ifndef PLAYER_H
#define PLAYER_H

#include "bitboard.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

class Player
{
	public:
		int color;
		Engine *e;

		Player(int col, Engine *engine);
		int get_color();
		virtual unsigned long long move(int* move_list)=0;

	private:

};

class Rand: public Player
{
	public:
		Rand(int col, Engine* engine);
		unsigned long long move(int* move_list);

	private:
};

class Minimax: public Player
{
	public:
		Minimax(int col, Engine* engine);
		unsigned long long move(int* move_list);

	private:
};


#endif