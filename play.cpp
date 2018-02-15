#include "bitboard.hpp"
#include "player.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <limits>
#include <time.h>
#include <chrono>

// g++ bitboard.hpp bitboard.cpp player.hpp player.cpp play.cpp -std=c++14 -o run

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


int play_game(Engine* e, Rand** players, Rand* curr_player, int* num_moves)
{
    int max_moves  = e->get_max_move_length();
    int moves_made = 0;
    int color = 1;
    int term;
    int move;

    // e->print_chess_char();
    // std::cout << std::endl;

    while(moves_made < max_moves)
    {
        curr_player = players[color];
        int* move_list = e->generate_legal_moves(color);

        term = e->is_terminal(color, move_list);
        if(term != -1)
        {
            std::cout << "game over, result is: " << term << " in " << moves_made  << " moves" << std::endl;
            // e->print_chess_char();

            free(move_list);
            return(term);
        }

        // std::cout << color_to_string(color) << " to move." << std::endl;
        // std::cout <<  "moves avaliable: " << move_list[0] << std::endl;
        move = curr_player->move(move_list);
        // std::cout <<  "making move: " << move << std::endl;
        // e->print_move_info(move);
        e->push_move(move);
        num_moves[0]++;

        // e->print_chess_char();
        // std::cin.ignore( std::numeric_limits <std::streamsize> ::max(), '\n' );

        moves_made++;
        color = 1-color;
        free(move_list);
    }

    std::cout << "game over, result is draw from making max moves: " << max_moves << std::endl;
    return(2); // draw
}


int main()
{
    srand(time(NULL));
    Engine* e = new Engine();
    
    Rand** players = (Rand**) malloc(2 * sizeof(Rand*));
    players[1] = new Rand(1, e); // white
    players[0] = new Rand(0, e); // black

    Rand* curr_player;
    int result;
    int* num_moves = (int*) malloc(sizeof(int));;



    // timing
    std::chrono::time_point<std::chrono::system_clock> t1, t2;
    std::chrono::duration<double, std::nano> time_cast_result;

    num_moves[0] = 0;
    t1 = std::chrono::system_clock::now();
    
    for(int i = 0; i < 1000; i++)
    {
        result = play_game(e, players, curr_player, num_moves);
        // e->print_chess_char();
        e->reset_engine();        
    }
    
    t2 = std::chrono::system_clock::now();
    time_cast_result = cast_nano(t2 - t1);
    double temp = (double) time_cast_result.count() / num_moves[0];

    std::cout << "total moves made: " << num_moves[0] << " with " << temp << " nanoseconds per move" << std::endl;



    free(players[0]);
    free(players[1]);
    free(players);
    free(e);
    free(num_moves);
    return(0);
}