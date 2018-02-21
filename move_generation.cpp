#include "bitboard.hpp"
#include <stdlib.h>
#include <unordered_map>
#include <cmath>
#include <strings.h>



void Engine::pop_and_add_regular_moves(int color, int* move_list, unsigned long long board, int curr_pos)
{
    unsigned long long new_pos;
    int piece_taken;
    while(board)
    {
        // std::cout << "lmao" << std::endl;
        // print_chess_rep(board);

        new_pos = lsb_board(board);
        piece_taken = get_piece_by_bitboard(1-color, new_pos);

        // std::cout << "okmyman " << bitboard_to_square(new_pos) << ", " << move_list[0] << std::endl;
        // print_chess_rep(board);

        // std::cout << "YABOI^2 " << bitboard_to_square(new_pos) << ", " << move_list[0] << std::endl;
        // print_chess_rep(new_pos);

        move_list[move_list[0]+1] = encode_move(curr_pos, bitboard_to_square(new_pos), REGULAR, piece_taken, 0, color);
        // std::cout << "but how: " << move_list[move_list[0]+1] << ", " << piece_type_to_string(get_piece_by_bitboard(square_to_bitboard(decode_from(move_list[move_list[0]+1])))) << std::endl;
        // if(piece_type_to_string(get_piece_by_bitboard(square_to_bitboard(decode_from(move_list[move_list[0]+1])))) == "nothing")
        // {
        //     std::cout << "moves in move_list: " << move_list[0] << std::endl;
        //     std::cout << "all_moves" << std::endl;
        //     print_chess_rep(board);
        //     std::cout << "one_move" << std::endl;
        //     print_chess_rep(new_pos);
        //     std::cout << "bitboard_to_square one move: " << bitboard_to_square(new_pos) << std::endl;
        //     std::cout << "piece_taken: " << piece_taken << std::endl;
        //     std::cout << "curr_pos: " << curr_pos << std::endl;
        //     print_chess_rep(new_pos);
        //     exit(0);
        // }
        board = board - new_pos;
        move_list[0]++;
    }
}


// Generates and fills move_list for a color before checking checks
// DOES NOT CHECK BOUNDS FOR move_arr_size
// 5000 ns
void Engine::generate_pre_check_moves(int color, int* move_list)
{
    unsigned long long p, one_p, all_occupied, own_occupied, enemy_occupied;
    all_occupied = get_all();

    if(color == 1)
    {
        own_occupied = get_all_white();
        enemy_occupied = get_all_black();

        p = pos.white_rooks;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_one_rook_moves(one_p, 
                all_occupied, own_occupied), bitboard_to_square(one_p));
        }


        p = pos.white_pawns;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_white_pawn_moves(one_p, 
                all_occupied, enemy_occupied), bitboard_to_square(one_p));
        }

        p = pos.white_bishops;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_one_bishop_moves(one_p, 
                all_occupied, own_occupied), bitboard_to_square(one_p));
        }

        p = pos.white_nights;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_night_moves(one_p, 
                own_occupied), bitboard_to_square(one_p));
        }

        p = pos.white_queens;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_one_queen_moves(one_p, 
                all_occupied, own_occupied), bitboard_to_square(one_p));
        }

        one_p = lsb_board(pos.white_kings);
        pop_and_add_regular_moves(color, move_list, pre_check_king_moves(one_p, 
            own_occupied), bitboard_to_square(one_p));
    }
    else
    {
        own_occupied = get_all_black();
        enemy_occupied = get_all_white();

        p = pos.black_rooks;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_one_rook_moves(one_p, 
                all_occupied, own_occupied), bitboard_to_square(one_p));
        }


        p = pos.black_pawns;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_black_pawn_moves(one_p, 
                all_occupied, enemy_occupied), bitboard_to_square(one_p));
        }

        p = pos.black_bishops;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_one_bishop_moves(one_p, 
                all_occupied, own_occupied), bitboard_to_square(one_p));
        }

        p = pos.black_nights;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_night_moves(one_p, 
                own_occupied), bitboard_to_square(one_p));
        }

        p = pos.black_queens;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_one_queen_moves(one_p, 
                all_occupied, own_occupied), bitboard_to_square(one_p));
        }

        one_p = lsb_board(pos.black_kings);
        pop_and_add_regular_moves(color, move_list, pre_check_king_moves(one_p, 
            own_occupied), bitboard_to_square(one_p));
    }
}


// generates evasive moves
void Engine::generate_pre_check_moves_single_check(int color, int* move_list, unsigned long long legal_defense)
{
    unsigned long long p, one_p, all_occupied, own_occupied, enemy_occupied;
    all_occupied = get_all();

    if(color == 1)
    {
        own_occupied = get_all_white();
        enemy_occupied = get_all_black();

        p = pos.white_rooks;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_one_rook_moves(one_p, 
                all_occupied, own_occupied) & legal_defense, bitboard_to_square(one_p));
        }


        p = pos.white_pawns;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_white_pawn_moves(one_p, 
                all_occupied, enemy_occupied) & legal_defense, bitboard_to_square(one_p));
        }

        p = pos.white_bishops;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_one_bishop_moves(one_p, 
                all_occupied, own_occupied) & legal_defense, bitboard_to_square(one_p));
        }

        p = pos.white_nights;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_night_moves(one_p, 
                own_occupied) & legal_defense, bitboard_to_square(one_p));
        }

        p = pos.white_queens;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_one_queen_moves(one_p, 
                all_occupied, own_occupied) & legal_defense, bitboard_to_square(one_p));
        }

        one_p = lsb_board(pos.white_kings);
        pop_and_add_regular_moves(color, move_list, pre_check_king_moves(one_p, 
            own_occupied), bitboard_to_square(one_p));
    }
    else
    {
        own_occupied = get_all_black();
        enemy_occupied = get_all_white();

        p = pos.black_rooks;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_one_rook_moves(one_p, 
                all_occupied, own_occupied) & legal_defense, bitboard_to_square(one_p));
        }


        p = pos.black_pawns;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_black_pawn_moves(one_p, 
                all_occupied, enemy_occupied) & legal_defense, bitboard_to_square(one_p));
        }

        p = pos.black_bishops;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_one_bishop_moves(one_p, 
                all_occupied, own_occupied) & legal_defense, bitboard_to_square(one_p));
        }

        p = pos.black_nights;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_night_moves(one_p, 
                own_occupied) & legal_defense, bitboard_to_square(one_p));
        }

        p = pos.black_queens;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_one_queen_moves(one_p, 
                all_occupied, own_occupied) & legal_defense, bitboard_to_square(one_p));
        }

        one_p = lsb_board(pos.black_kings);
        pop_and_add_regular_moves(color, move_list, pre_check_king_moves(one_p, 
            own_occupied), bitboard_to_square(one_p));
    }
}

void Engine::generate_pre_check_moves_double_check(int color, int* move_list)
{
    unsigned long long one_p, own_occupied;

    if(color == 1)
    {
        one_p = lsb_board(pos.white_kings);
        pop_and_add_regular_moves(color, move_list, pre_check_king_moves(one_p, 
            get_all_white()), bitboard_to_square(one_p));
    }
    else
    {
        one_p = lsb_board(pos.black_kings);
        pop_and_add_regular_moves(color, move_list, pre_check_king_moves(one_p, 
            get_all_black()), bitboard_to_square(one_p));
    }
}


// Generates and returns a list of legal moves for a color
int* Engine::generate_legal_moves(int color)
{
    int *move_list = (int*) malloc(move_arr_size * sizeof(int)); 
    // int last_move_index = 0;
    move_list[0] = 0; // encode index in array

    unsigned long long pinned = pinned_pieces(color);
    int king_square = bitboard_to_square(get_bitboard_of_piece(KING, color));

    unsigned long long* check_info = get_attackers_blocks(color);

    // if(check_info[0] != 0)
    // {
    //     std::cout << "check_info: " << check_info[0] << std::endl;
    //     std::cout << "legals" << std::endl;
    //     print_chess_rep(check_info[1] | check_info[2]);
    // }

    if(check_info[0]) // in check
    {
        if(check_info[0] > 1) // double check
        {
            generate_pre_check_moves_double_check(color, move_list);
        }
        else // single check
        {
            generate_pre_check_moves_single_check(color, move_list, check_info[1] | check_info[2]);
        }
    }
    else
    {
        generate_pre_check_moves(color, move_list);
    }

    int move_iter = 0;
    while(move_iter < move_list[0])
    {
        int move = move_list[move_iter+1];
        // if((pinned || decode_from(move) == king_square || decode_type(move) == ENPASSANT || check_status) && !(check_legal(move, color)))
        if((pinned || decode_from(move) == king_square || decode_type(move) == ENPASSANT) && !(check_legal(move, color)))
        {
            // std::cout << "removing move cause not legal: " << move << std::endl;
            move_list[move_iter+1] = move_list[move_list[0]];
            move_list[0]--;
            move_iter--;
        }
        move_iter++;
    }
    free(check_info);
    return(move_list);
}