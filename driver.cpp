#include "bitboard.cpp"


// g++ bitboard.hpp bitboard.cpp driver.cpp -std=c++14 -o run

int main()
{
	Engine *e = new Engine();
	e->print_chess_rep(e->pos.white_kings);
	return 0;
}