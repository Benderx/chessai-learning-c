#include "bitboard.hpp"
#include "player.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <limits>
#include <time.h>

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

int main()
{
    srand(time(NULL));
    Engine* e = new Engine();
    
    Rand** players = (Rand**) malloc(2 * sizeof(Rand*));
    players[1] = new Rand(1, e); // white
    players[0] = new Rand(0, e); // black

    Rand* curr_player;

    int max_moves  = e->get_max_move_length();
    int moves_made = 0;
    int color = 1;
    int term;
    int move;

    while(moves_made < max_moves-1)
    {
        e->print_chess_char();

        curr_player = players[color];
        int* move_list = e->generate_legal_moves(color);

        term = e->is_terminal(color, move_list);
        if(term != -1)
        {
            std::cout << "game over, result is: " << term << std::endl;

            free(e);
            free(players);
            free(move_list);
            return term;
        }

        std::cout << color_to_string(color) << " to move." << std::endl;
        std::cout <<  "moves avaliable: " << move_list[0] << std::endl;
        move = curr_player->move(move_list);
        e->push_move(move);

        std::cin.ignore( std::numeric_limits <std::streamsize> ::max(), '\n' );

        moves_made++;
        color = 1-color;
        free(move_list);
    }

    free(e);
    free(players);
    return(2);
}