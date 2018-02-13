#ifndef PLAYER_H
#define PLAYER_H

#include "bitboard.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

class Player
{
	public:
		//Vars
		int color;
		Engine *e;

		//Methods
		Player(int col, Engine *engine);
		int get_color();

	private:

};

class Rand: public Player
{
	public:
		//Var
		int color;
		Engine *e;

		//Methods
		Rand(int col, Engine* engine);
		unsigned long long move(int* move_list);

	private:
};


#endif