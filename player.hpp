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
        virtual int move(int* move_list)=0;

    private:

};

class Rand: public Player
{
    public:
        Rand(int col, Engine* engine);
        int move(int* move_list);

    private:
};

class Minimax: public Player
{
    public:
        Minimax(int col, Engine* engine);
        int minimax(int depth, int color, int max_depth);
        int move(int* move_list);
        int* copy_move_list(int* move_list);
        double simple_board_eval_helper(unsigned long long pieces, double val);
        double simple_board_eval();
        int decode_terminal_score(int term);

    private:
        int depth_search_limit;
        unsigned long long node_count;
};


#endif