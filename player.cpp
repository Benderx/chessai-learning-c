#include "player.hpp"
#include "bitboard.hpp"
#include <stdlib.h>
#include <chrono>
#include <cstring>


// #define MAX(x, y) (((x) > (y)) ? (x) : (y))
// #define MIN(x, y) (((x) < (y)) ? (x) : (y))


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
    depth_search_limit = 3;
    // best_holder = {99999999, -99999999};
    // compare_holder = {&std::min, &std::max};
    // compare_holder[0] = &std::min;
    // compare_holder[1] = &std::max;
}

int Minimax::minimax(int depth, int color)
{
    node_count++;

    int* move_list = e->generate_legal_moves(color);
    int term = e->is_terminal(color, move_list);
    if(term != -1)
    {
        return(decode_terminal_score(term));
    }
    if(depth == 0)
    {
        return(simple_board_eval(color));
    }

    // actual minimax
    int temp_holder;
    int best;
    int num_moves = move_list[0];
    int* copied_move_list = copy_move_list(move_list);

    best = best_holder[color];
    // if(color) // true, or white
    // {
    //     for(int i = 0; i < num_moves; i++)
    //     {
    //         e->push_move(copied_move_list[i]);
    //         temp_holder = minimax(depth-1, 1-color);
    //         best = std::max(best, temp_holder);
    //         e->pop_move();
    //     }
    //     return(best);
    // }
    // else
    // {
    //     for(int i = 0; i < num_moves; i++)
    //     {
    //         e->push_move(copied_move_list[i]);
    //         temp_holder = minimax(depth-1, 1-color);
    //         best = std::min(best, minimax(depth-1, 1-color));
    //         e->pop_move();
    //     }
    //     return(best);
    // }

    for(int i = 0; i < num_moves; i++)
    {
        e->push_move(copied_move_list[i]);
        temp_holder = minimax(depth-1, 1-color);
        best = std::max(best, temp_holder); // function pointer call to corrct comparator (max or mix)
        e->pop_move();
    }
    free(copied_move_list);
    return(best);
}


std::chrono::duration<double, std::nano> cast_nano2(std::chrono::duration<double> x)
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(x);
}


int Minimax::move(int* move_list)
{
    //timing
    std::chrono::time_point<std::chrono::system_clock> t1, t2;
    std::chrono::duration<double, std::nano> time_cast_result;
    t1 = std::chrono::system_clock::now();
    //end timing


    node_count = 0;

    int num_moves = move_list[0];
    int* copied_move_list = copy_move_list(move_list);


    int best_score;
    int best_move = copied_move_list[0];
    int temp;

    e->push_move(copied_move_list[0]);
    best_score = minimax(depth_search_limit, 1-color);
    e->pop_move();

    if(color)
    {
        printf("MINIMAXING FOR WHITE num_moves %i\n", num_moves);
        for(int i = 1; i < num_moves; i++)
        {
            e->push_move(copied_move_list[i]);
            // e->print_chess_char();
            temp = minimax(depth_search_limit, 1-color);
            e->pop_move();
            if(temp > best_score)
            {
                best_score = temp;
                best_move = copied_move_list[i];
            }
        }
    }
    else
    {
        printf("MINIMAXING FOR BLACK\n");
        for(int i = 1; i < num_moves; i++)
        {
            e->push_move(copied_move_list[i]);
            e->print_chess_char();
            temp = minimax(depth_search_limit, 1-color);
            e->pop_move();
            if(temp < best_score)
            {
                best_score = temp;
                best_move = copied_move_list[i];
            }
        }
    }
    free(copied_move_list);

    //timing
    t2 = std::chrono::system_clock::now();
    time_cast_result = cast_nano2(t2 - t1);
    double timing_temp = (double) time_cast_result.count() / node_count;

    std::cout << "total nodes examined: " << node_count << " with " << timing_temp << " nanoseconds per move" << std::endl;
    std::cout << "resulting in NPS of: " << 1.0 / (timing_temp * .000000001) << std::endl;
    //end timing

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

double Minimax::simple_board_eval(int color)
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

    return(accum * (1 - 2 * color));
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