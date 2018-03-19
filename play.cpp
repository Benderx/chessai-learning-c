#include "bitboard.hpp"
#include "player.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <limits>
#include <time.h>
#include <chrono>
#include <vector>

// g++ bitboard.hpp bitboard.cpp player.hpp player.cpp play.cpp -std=c++14 -o run

//optimized
// g++ bitboard.hpp bitboard.cpp player.hpp player.cpp play.cpp -std=c++14 -O3 -funroll-loops -Wall -Wno-unused-variable -Wno-unused-value -Wno-comment -Wno-unused-but-set-variable -Wno-maybe-uninitialized  -o run


// to profile: valgrind --tool=callgrind ./play
// more profiling info: http://zariko.taba.free.fr/c++/callgrind_profile_only_a_part.html

//memleaks and stuff: valgrind --tool=memcheck --leak-check=yes --log-file=out.log ./play

// cache misses: valgrind --tool=cachegrind ./play
// cachegrind read: cg_annotate cachegrind.out.601


std::string color_to_string(int color)
{
    if(color == 1)
    {
        return("White");
    }
    else
    {
        return("Black");
    }
}


std::chrono::duration<double, std::nano> cast_nano(std::chrono::duration<double> x)
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(x);
}


int play_game(Engine* e, std::vector<Player*> players, int* num_moves)
{
    int max_moves  = e->get_max_move_length();
    int moves_made = 0;
    int color = 1;
    int term;
    int move;
    int* move_list;

    // e->print_chess_char();
    // std::cout << std::endl;

    while(moves_made < max_moves)
    {
        // curr_player = players[color];
        move_list = e->generate_legal_moves(color);

        term = e->is_terminal(color, move_list);
        if(term != -1)
        {
            // std::cout << "game over, result is: " << term << " in " << moves_made  << " moves" << std::endl;
            // e->print_chess_char();
            return(term);
        }

        // std::cout << color_to_string(color) << " to move." << std::endl;
        // std::cout <<  "moves avaliable: " << move_list[0] << std::endl;
        move = players[color]->move(move_list);
        // std::cout <<  "making move: " << move << std::endl;
        // e->print_move_info(move);
        e->push_move(move);
        num_moves[0]++;
        // exit(0);
        // return(2);
        // e->print_chess_char();
        // std::cin.ignore( std::numeric_limits <std::streamsize> ::max(), '\n' );

        moves_made++;
        color = 1-color;
    }


    // std::cout << "game over, result is draw from making max moves: " << max_moves << std::endl;
    return(2); // draw
}


int main()
{
    srand(time(NULL));
    Engine* e = new Engine();
    
    // Player** players = (Player**) malloc(2 * sizeof(Player*));
    std::vector<Player*> players;
    players.push_back(new Rand(0, e)); // black
    players.push_back(new Rand(1, e)); // white

    int result;
    unsigned long long result2;
    int* num_moves = (int*) malloc(sizeof(int));



    // timing
    std::chrono::time_point<std::chrono::system_clock> t1, t2;
    std::chrono::duration<double, std::nano> time_cast_result;

    num_moves[0] = 0;
    t1 = std::chrono::system_clock::now();


    // result = play_game(e, players, num_moves);
    
    for(int i = 0; i < 10; i++)
    {
        result = play_game(e, players, num_moves);
        // e->print_chess_char();
        e->reset_engine();  
        // exit(0);      
        // std::cout << "game" << std::endl;
    }
    
    // int* garbage = (int*) malloc(10000 * sizeof(int));
    // garbage[0] = 0;
    // for(int i = 0; i < 10000000; i++)
    // {
        // e->generate_pre_check_moves(1, garbage, 0); 
        // result = e->lsb_digit(e->pos.white_kings);
        // result2 = e->reverse_64_bits(e->pos.white_pawns);
        // result = e->bitboard_to_square(e->get_bitboard_of_piece(KING, 1));
        // result2 = e->pre_check_rook_moves(e->pos.white_rooks);
        // std::cout << result << std::endl; 
        // exit(0);
        // garbage[0] = 0;
        // result = e->get_bitboard_of_piece(KING, 1);
        // result2 = e->get_all();
    // }

    t2 = std::chrono::system_clock::now();
    time_cast_result = cast_nano(t2 - t1);
    // double temp = (double) time_cast_result.count() / 10000000;
    double temp = (double) time_cast_result.count() / num_moves[0];

    std::cout << "total moves made: " << num_moves[0] << " with " << temp << " nanoseconds per move" << std::endl;
    std::cout << "resulting in NPS of: " << 1.0 / (temp * .000000001) << std::endl;

    // delete(players[0]);
    // delete(players[1]);
    players.clear();
    e->clean_up();
    delete(e);
    free(num_moves);
    return(0);
}