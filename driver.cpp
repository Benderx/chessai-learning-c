#include "bitboard.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

// g++ bitboard.hpp bitboard.cpp driver.cpp -std=c++14 -o run

void print_pieces(Engine* e, std::string c, std::string s, unsigned long long pieces)
{
    std::cout << "All " << c << " " << s << std::endl;
    e->print_chess_rep(pieces);
    std::cout << std::endl;
}

void print_moves(Engine* e, std::string c, int* moves)
{
    print_pieces(e, "", "pieces", e->get_all());
    std::cout << "printing moves for " << c << std::endl;
    for(int i = 0; i < moves[0]; i++)
    {
        std::cout << "making move " << moves[i+1] << std::endl;
        e->push_move(moves[i+1]);
        e->print_chess_rep(e->get_all());
        e->pop_move();
        // std::cout << "popped" << std::endl;
        // e->print_chess_rep(e->get_all());
        // exit(0);
    }
    std::cout << "total legal moves: " << moves[0] << std::endl;
}

void print_all_pieces(Engine* e)
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

void test_pawn_moves(Engine* e)
{
    e->pos.white_pawns = 0b0000000000000000000000000000000000010000000000001110111100000000;
    e->pos.black_pawns = 0b0000000011110111000000000000100000000000000000000000000000000000;

    print_pieces(e, "white", "pawns", e->pos.white_pawns);
    print_pieces(e, "black", "pawns", e->pos.black_pawns);

    print_pieces(e, "white", "pawn moves", e->pre_check_white_pawn_moves(e->pos.white_pawns, e->get_all(), e->get_all_black()));
    print_pieces(e, "black", "pawn moves", e->pre_check_black_pawn_moves(e->pos.black_pawns, e->get_all(), e->get_all_white()));
}

void test_king_moves(Engine* e)
{
    e->pos.white_kings = 0b0000000000000000000000000000000000000000000100000000000000000000;
    e->pos.black_kings = 0b0000000000000000000100000000000000000000000000000000000000000000;

    print_pieces(e, "white", "kings", e->pos.white_kings);
    print_pieces(e, "black", "kings", e->pos.black_kings);

    print_pieces(e, "white", "king moves", e->pre_check_king_moves(1));
    print_pieces(e, "black", "king moves", e->pre_check_king_moves(0));
}

void test_night_moves(Engine* e)
{
    e->pos.white_nights = 0b0000000000000000000000000000000000000000001000000000000000000000;
    e->pos.black_nights = 0b0000000000000000000100000000000000000000000000000000000000000000;

    print_pieces(e, "white", "nights", e->pos.white_nights);
    print_pieces(e, "black", "nights", e->pos.black_nights);

    print_pieces(e, "white", "night moves", e->pre_check_night_moves(1));
    print_pieces(e, "black", "night moves", e->pre_check_night_moves(0));
}

void test_bishop_moves(Engine* e)
{
    e->pos.white_bishops = 0b0000000000000000000000000000000000000000001000000000000000000000;
    e->pos.black_bishops = 0b0000000000000000000100000000000000000000000000000000000000000000;

    print_all_pieces(e);
    print_pieces(e, "white", "bishops", e->pos.white_bishops);
    print_pieces(e, "black", "bishops", e->pos.black_bishops);

    print_pieces(e, "white", "bishop moves", e->pre_check_bishop_moves(1));
    print_pieces(e, "black", "bishop moves", e->pre_check_bishop_moves(0));
}

void test_rook_moves(Engine* e)
{
    e->pos.white_rooks = 0b0000000000000000000000000000000000000000001000000000000000000000;
    e->pos.black_rooks = 0b0000000000000000000100000000000000000000000000000000000000000000;

    print_pieces(e, "white", "rooks", e->pos.white_rooks);
    print_pieces(e, "black", "rooks", e->pos.black_rooks);

    print_pieces(e, "white", "rook moves", e->pre_check_rook_moves(1));
    print_pieces(e, "black", "rook moves", e->pre_check_rook_moves(0));
}

void test_queen_moves(Engine* e)
{
    e->pos.white_queens = 0b0000000000000000000000000000000000000000001000000000000000000000;
    e->pos.black_queens = 0b0000000000000000000100000000000000000000000000000000000000000000;

    print_pieces(e, "white", "queens", e->pos.white_queens);
    print_pieces(e, "black", "queens", e->pos.black_queens);

    print_pieces(e, "white", "queen moves", e->pre_check_queen_moves(1));
    print_pieces(e, "black", "queen moves", e->pre_check_queen_moves(0));
}

void test_white_moves(Engine* e)
{
    e->pos.white_pawns = 0b0000000000000000000000000000000000000000000000011111111000000000;
    // e->pos.black_pawns = 0b0000000001111111010000000000000000000000000000000000000000000000;

    print_pieces(e, "white", "pawns", e->pos.white_pawns);
    print_pieces(e, "black", "pawns", e->pos.black_pawns);

    print_moves(e, "white", e->generate_legal_moves(1));
    // std::cout << "why\n";
    // print_pieces(e, "white", "moves", e->generate_legal_moves(1));
    // print_pieces(e, "black", "moves", e->pre_check_queen_moves(0));
}

void test_mask_check(Engine* e)
{
    print_pieces(e, "pieces", "masked", e->get_all() & e->col_mask[0]);
}

void write_board(std::ofstream* file, Engine* e)
{
    unsigned long long* rep = e->get_board_rep();
    std::cout << "yaboi" << std::endl;
    for(int i = 0; i < 12; i++)
    {
        (*file) << rep[i] << "\n";
    }
}

int main()
{
    Engine* e = new Engine();
    print_all_pieces(e);

    std::ofstream* file = new std::ofstream();
    file->open("games/game0.txt");


    test_mask_check(e);
    e->reset_engine();

    std::cout << "testing pawn moves" << std::endl;
    test_pawn_moves(e);
    e->reset_engine();

    std::cout << "testing king moves" << std::endl;
    test_king_moves(e);
    e->reset_engine();

    std::cout << "testing night moves" << std::endl;
    test_night_moves(e);
    e->reset_engine();

    std::cout << "testing bishop moves" << std::endl;
    test_bishop_moves(e);
    e->reset_engine();

    std::cout << "testing rook moves" << std::endl;
    test_rook_moves(e);
    e->reset_engine();

    std::cout << "testing queen moves" << std::endl;
    test_queen_moves(e);
    e->reset_engine();


    std::cout << "testing white moves" << std::endl;
    test_white_moves(e);
    e->reset_engine();



    write_board(file, e);
    file->close();

    return 0;
}