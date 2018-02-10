#include "player.hpp"
#include <stdlib.h>

Player::Player(int col, Engine *engine)
{
	color = col;
	e = engine;
}

int Player::get_color()
{
	return(this.color);
}


Rand::Rand(int col, Engine *engine) 
: Player(col, *engine)

unsigned long long Rand::move()
{

}