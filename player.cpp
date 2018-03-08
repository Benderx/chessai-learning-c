#include "player.hpp"
#include "bitboard.hpp"
#include <stdlib.h>
#include <cstring>


#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))


// PLAYER


Player::Player(int col, Engine* engine)
{
    color = col;
    e = engine;
}

int Player::get_color()
{
    return(this->color);
}


// RAND


Rand::Rand(int col, Engine* engine) : Player(col, engine)
{
    //
}

int Rand::move(int* move_list)
{
    int* rand_list = e->generate_legal_moves(color);
    int num_moves = rand_list[0];

    return(rand_list[(rand() % num_moves) + 1]);
}


// MINIMAX

Minimax::Minimax(int col, Engine* engine) : Player(col, engine)
{
    depth_search_limit = 2;
}

int Minimax::minimax(int depth, int color, int max_depth)
{
    node_count++;

    int* move_list = e->generate_legal_moves(color);
    int num_moves = move_list[0];
    int term = e->is_terminal(color, move_list);
    if(term != -1)
    {
        return(decode_terminal_score(term));
    }
    if(depth == max_depth)
    {
        // std::cout << "max_depth" << std::endl;
        return(simple_board_eval());
    }

    // actual minimax

    int best;
    move_list = copy_move_list(move_list);
    if(color) // true, or white
    {
        best = -999999999;
        for(int i = 0; i < num_moves; i++)
        {
            e->push_move(move_list[i]);
            best = MAX(best, minimax(depth+1, 1-color, max_depth));
            e->pop_move();
        }
        return(best);
    }
    else
    {
        best = 999999999;
        for(int i = 0; i < num_moves; i++)
        {
            e->push_move(move_list[i]);
            best = MIN(best, minimax(depth+1, 1-color, max_depth));
            e->pop_move();
        }
        return(best);
    }
    free(move_list);
}

int Minimax::move(int* move_list)
{
    node_count = 0;

    int num_moves = move_list[0];
    move_list = copy_move_list(move_list);

    int best_score;
    int best_move = move_list[0];
    int temp;

    e->push_move(move_list[0]);
    best_score = minimax(0, 1-color, depth_search_limit);
    e->pop_move();

    if(color)
    {
        for(int i = 1; i < num_moves; i++)
        {
            e->push_move(move_list[i]);
            temp = minimax(0, 1-color, depth_search_limit);
            e->pop_move();
            if(temp > best_score)
            {
                best_score = temp;
                best_move = move_list[i];
            }
        }
    }
    else
    {
        for(int i = 1; i < num_moves; i++)
        {
            e->push_move(move_list[i]);
            temp = minimax(0, 1-color, depth_search_limit);
            e->pop_move();
            if(temp < best_score)
            {
                best_score = temp;
                best_move = move_list[i];
            }
        }
    }
    free(move_list);

    std::cout << "nodes examined: " << node_count << std::endl;
    return(best_move);
}

int* Minimax::copy_move_list(int* move_list)
{
    int* new_move_list = (int*) malloc(move_list[0] * sizeof(int));
    std::memcpy(new_move_list, move_list + 1, move_list[0] * sizeof(int));
    return(new_move_list);
}

double Minimax::simple_board_eval_helper(unsigned long long pieces, double val)
{
    double accum = 0;
    while(pieces)
    {
        accum += val;
        pieces =  pieces - e->lsb_board(pieces);
    }
    return(accum);
}

double Minimax::simple_board_eval()
{
    double accum = 0;
    accum += simple_board_eval_helper(e->pos.white_pawns, 1);
    accum += simple_board_eval_helper(e->pos.white_rooks, 5);
    accum += simple_board_eval_helper(e->pos.white_nights, 3);
    accum += simple_board_eval_helper(e->pos.white_bishops, 3.1);
    accum += simple_board_eval_helper(e->pos.white_queens, 9);

    accum += simple_board_eval_helper(e->pos.black_pawns, -1);
    accum += simple_board_eval_helper(e->pos.black_rooks, -5);
    accum += simple_board_eval_helper(e->pos.black_nights, -3);
    accum += simple_board_eval_helper(e->pos.black_bishops, -3.1);
    accum += simple_board_eval_helper(e->pos.black_queens, -9);

    return(accum);
}

int Minimax::decode_terminal_score(int term)
{
    if(term == 2)
    {
        return(0);
    }
    else if(term == 1)
    {
        return(999999999);
    }
    else
    {
        return(-999999999);
    }
}