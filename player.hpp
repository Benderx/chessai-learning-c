#ifndef PLAYER_H
#define PLAYER_H

#include "bitboard.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

// typedef int (*foo_ptr_t)( int );
typedef void (*fp)(int);

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
        // int minimax_white(int depth);
        // int minimax_black(int depth);
        int minimax(int depth, int color);
        int move(int* move_list);
        int* copy_move_list(int* move_list);
        double simple_board_eval_helper(unsigned long long pieces, double val);
        double simple_board_eval(int color);
        int decode_terminal_score(int term);

    private:
        int depth_search_limit;
        int node_count;
        int best_holder[2] = {99999999, -99999999};
        // void** compare_holder[2] = {&std::min, &std::max};
        // foo_ptr_t compare_holder[2] = {&std::min, &std::max};
        // fp compare_holder[2] = {std::min, std::max};
        // fp compare_holder[2];
        // compare_holder[0] = &std::min;
        // compare_holder[1] = &std::max;
};


#endif