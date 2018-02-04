#include "bitboard.hpp"
#include <stdio.h>
#include <stdlib.h>

// g++ bitboard.hpp bitboard.cpp driver.cpp -std=c++14 -o run

int print_pieces(Engine *e, std::string c, std::string s, unsigned long long pieces)
{
    std::cout << "All " << c << " " << s << std::endl;
    e->print_chess_rep(pieces);
    std::cout << std::endl;
}

int print_all_pieces(Engine *e)
{
    print_pieces(e, "", "pieces", e->get_all());
    print_pieces(e, "white", "pieces", e->get_all_white());
    print_pieces(e, "black", "pieces", e->get_all_black());

    std::string c = "white";
    print_pieces(e, c, "kings", e->pos.white_kings);
    print_pieces(e, c, "queens", e->pos.white_queens);
    print_pieces(e, c, "knights", e->pos.white_nights);
    print_pieces(e, c, "bishops", e->pos.white_bishops);
    print_pieces(e, c, "rooks", e->pos.white_rooks);
    print_pieces(e, c, "pawns", e->pos.white_pawns);

    c = "black";
    print_pieces(e, c, "kings", e->pos.black_kings);
    print_pieces(e, c, "queens", e->pos.black_queens);
    print_pieces(e, c, "knights", e->pos.black_nights);
    print_pieces(e, c, "bishops", e->pos.black_bishops);
    print_pieces(e, c, "rooks", e->pos.black_rooks);
    print_pieces(e, c, "pawns", e->pos.black_pawns);
}

int test_pawn(Engine *e)
{
    e->pos.white_pawns = 0b0000000000000000000000000000000000010000000000001110111100000000;
    e->pos.black_pawns = 0b0000000011110111000000000000100000000000000000000000000000000000;

    print_pieces(e, "white", "pawns", e->pos.white_pawns);
    print_pieces(e, "black", "pawns", e->pos.black_pawns);

    print_pieces(e, "white", "pawn moves", e->pre_check_white_pawn_moves(e->pos.white_pawns, e->get_all(), e->get_all_black()));
    print_pieces(e, "black", "pawn moves", e->pre_check_black_pawn_moves(e->pos.black_pawns, e->get_all(), e->get_all_white()));
}


int main()
{
    Engine *e = new Engine();
    print_all_pieces(e);

    std::cout << "testing pawn" << std::endl;
    test_pawn(e);

    e->reset_engine();

    return 0;
}