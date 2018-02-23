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
void Engine::generate_moves(int color, int* move_list, unsigned long long danger)
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
            own_occupied) & danger, bitboard_to_square(one_p));
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
            own_occupied) & danger, bitboard_to_square(one_p));
    }
}

void Engine::generate_moves_pinned(int color, int* move_list, unsigned long long danger, unsigned long long pinned)
{
    unsigned long long p, one_p, all_occupied, own_occupied, enemy_occupied;
    all_occupied = get_all();

    if(color == 1)
    {
        own_occupied = get_all_white();
        enemy_occupied = get_all_black();

        p = pos.white_rooks & !pinned;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_one_rook_moves(one_p, 
                all_occupied, own_occupied), bitboard_to_square(one_p));
        }


        p = pos.white_pawns & !pinned;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_white_pawn_moves(one_p, 
                all_occupied, enemy_occupied), bitboard_to_square(one_p));
        }

        p = pos.white_bishops & !pinned;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_one_bishop_moves(one_p, 
                all_occupied, own_occupied), bitboard_to_square(one_p));
        }

        p = pos.white_nights & !pinned;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_night_moves(one_p, 
                own_occupied), bitboard_to_square(one_p));
        }

        p = pos.white_queens & !pinned;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_one_queen_moves(one_p, 
                all_occupied, own_occupied), bitboard_to_square(one_p));
        }

        one_p = lsb_board(pos.white_kings);
        pop_and_add_regular_moves(color, move_list, pre_check_king_moves(one_p, 
            own_occupied) & danger, bitboard_to_square(one_p));
    }
    else
    {
        own_occupied = get_all_black();
        enemy_occupied = get_all_white();

        p = pos.black_rooks & !pinned;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_one_rook_moves(one_p, 
                all_occupied, own_occupied), bitboard_to_square(one_p));
        }


        p = pos.black_pawns & !pinned;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_black_pawn_moves(one_p, 
                all_occupied, enemy_occupied), bitboard_to_square(one_p));
        }

        p = pos.black_bishops & !pinned;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_one_bishop_moves(one_p, 
                all_occupied, own_occupied), bitboard_to_square(one_p));
        }

        p = pos.black_nights & !pinned;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_night_moves(one_p, 
                own_occupied), bitboard_to_square(one_p));
        }

        p = pos.black_queens & !pinned;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_one_queen_moves(one_p, 
                all_occupied, own_occupied), bitboard_to_square(one_p));
        }

        one_p = lsb_board(pos.black_kings);
        pop_and_add_regular_moves(color, move_list, pre_check_king_moves(one_p, 
            own_occupied) & danger, bitboard_to_square(one_p));
    }
}

// generates evasive moves
void Engine::generate_moves_single_check(int color, int* move_list, unsigned long long danger, unsigned long long legal_defense)
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
            own_occupied) & danger, bitboard_to_square(one_p));
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
            own_occupied) & danger, bitboard_to_square(one_p));
    }
}

void Engine::generate_moves_pinned_single_check(int color, int* move_list, unsigned long long danger, unsigned long long legal_defense, unsigned long long pinned)
{
    unsigned long long p, one_p, all_occupied, own_occupied, enemy_occupied;
    all_occupied = get_all();

    if(color == 1)
    {
        own_occupied = get_all_white();
        enemy_occupied = get_all_black();

        p = pos.white_rooks & !pinned;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_one_rook_moves(one_p, 
                all_occupied, own_occupied) & legal_defense, bitboard_to_square(one_p));
        }


        p = pos.white_pawns & !pinned;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_white_pawn_moves(one_p, 
                all_occupied, enemy_occupied) & legal_defense, bitboard_to_square(one_p));
        }

        p = pos.white_bishops & !pinned;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_one_bishop_moves(one_p, 
                all_occupied, own_occupied) & legal_defense, bitboard_to_square(one_p));
        }

        p = pos.white_nights & !pinned;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_night_moves(one_p, 
                own_occupied) & legal_defense, bitboard_to_square(one_p));
        }

        p = pos.white_queens & !pinned;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_one_queen_moves(one_p, 
                all_occupied, own_occupied) & legal_defense, bitboard_to_square(one_p));
        }

        one_p = lsb_board(pos.white_kings);
        pop_and_add_regular_moves(color, move_list, pre_check_king_moves(one_p, 
            own_occupied) & danger, bitboard_to_square(one_p));
    }
    else
    {
        own_occupied = get_all_black();
        enemy_occupied = get_all_white();

        p = pos.black_rooks & !pinned;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_one_rook_moves(one_p, 
                all_occupied, own_occupied) & legal_defense, bitboard_to_square(one_p));
        }


        p = pos.black_pawns & !pinned;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_black_pawn_moves(one_p, 
                all_occupied, enemy_occupied) & legal_defense, bitboard_to_square(one_p));
        }

        p = pos.black_bishops & !pinned;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_one_bishop_moves(one_p, 
                all_occupied, own_occupied) & legal_defense, bitboard_to_square(one_p));
        }

        p = pos.black_nights & !pinned;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_night_moves(one_p, 
                own_occupied) & legal_defense, bitboard_to_square(one_p));
        }

        p = pos.black_queens & !pinned;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;
            pop_and_add_regular_moves(color, move_list, pre_check_one_queen_moves(one_p, 
                all_occupied, own_occupied) & legal_defense, bitboard_to_square(one_p));
        }

        one_p = lsb_board(pos.black_kings);
        pop_and_add_regular_moves(color, move_list, pre_check_king_moves(one_p, 
            own_occupied) & danger, bitboard_to_square(one_p));
    }
}

void Engine::generate_moves_double_check(int color, int* move_list, unsigned long long danger)
{
    unsigned long long one_p, own_occupied;

    if(color == 1)
    {
        one_p = lsb_board(pos.white_kings);
        pop_and_add_regular_moves(color, move_list, pre_check_king_moves(one_p, 
            get_all_white()) & danger, bitboard_to_square(one_p));
    }
    else
    {
        one_p = lsb_board(pos.black_kings);
        pop_and_add_regular_moves(color, move_list, pre_check_king_moves(one_p, 
            get_all_black()) & danger, bitboard_to_square(one_p));
    }
}

void Engine::pin_card_helper(int color, Piece p, unsigned long long board, unsigned long long los, int* move_list)
{
    unsigned long long all_occupied, own_occupied, enemy_occupied;
    all_occupied = get_all();

    if(color == 1)
    {
        own_occupied = get_all_white();
        enemy_occupied = get_all_black();

        if(p == ROOK || p == QUEEN)
        {
            pop_and_add_regular_moves(color, move_list, pre_check_one_rook_moves(board, 
                all_occupied, own_occupied) & los, bitboard_to_square(board));
        }

        else if(p == PAWN)
        {
            pop_and_add_regular_moves(color, move_list, pre_check_white_pawn_moves(board, 
                all_occupied, enemy_occupied) & los, bitboard_to_square(board));
        }
    }
    else
    {
        own_occupied = get_all_black();
        enemy_occupied = get_all_white();

        if(p == ROOK || p == QUEEN)
        {
            pop_and_add_regular_moves(color, move_list, pre_check_one_rook_moves(board, 
                all_occupied, own_occupied) & los, bitboard_to_square(board));
        }

        else if(p == PAWN)
        {
            pop_and_add_regular_moves(color, move_list, pre_check_black_pawn_moves(board, 
                all_occupied, enemy_occupied) & los, bitboard_to_square(board));
        }
    }
}

void Engine::pin_diag_helper(int color, Piece p, unsigned long long board, unsigned long long los, int* move_list)
{
    unsigned long long all_occupied, own_occupied, enemy_occupied;
    all_occupied = get_all();

    if(color == 1)
    {
        own_occupied = get_all_white();
        enemy_occupied = get_all_black();

        if(p == BISHOP || p == QUEEN)
        {
            pop_and_add_regular_moves(color, move_list, pre_check_one_bishop_moves(board, 
                all_occupied, own_occupied) & los, bitboard_to_square(board));
        }

        else if(p == PAWN)
        {
            pop_and_add_regular_moves(color, move_list, pre_check_white_pawn_moves(board, 
                all_occupied, enemy_occupied) & los, bitboard_to_square(board));
        }
    }
    else
    {
        own_occupied = get_all_black();
        enemy_occupied = get_all_white();

        if(p == BISHOP || p == QUEEN)
        {
            pop_and_add_regular_moves(color, move_list, pre_check_one_bishop_moves(board, 
                all_occupied, own_occupied) & los, bitboard_to_square(board));
        }

        else if(p == PAWN)
        {
            pop_and_add_regular_moves(color, move_list, pre_check_black_pawn_moves(board, 
                all_occupied, enemy_occupied) & los, bitboard_to_square(board));
        }
    }
}


// Returns a bitboard of pieces that are pinned against their king 
// 1000ns
unsigned long long Engine::pinned_pieces(int color, int* move_list)
{
    unsigned long long my_king, enemy_rooks, enemy_bishops, enemy_queens, my_pieces;

    unsigned long long king_hori_attacks, king_vert_attacks;
    unsigned long long king_left_diag_attacks, king_right_diag_attacks;

    unsigned long long rook_vert_attacks, rook_hori_attacks;
    unsigned long long bishop_left_diag_attacks, bishop_right_diag_attacks;

    unsigned long long card_los, diag_los;
    unsigned long long the_card_pinned, the_diag_pinned, all_pinned;
    unsigned long long one_p;

    if(color == 1)
    {
        my_king = pos.white_kings;
        enemy_rooks = pos.black_rooks;
        enemy_bishops = pos.black_bishops;
        enemy_queens = pos.black_queens;
        my_pieces = get_all_white();
    }
    else
    {
        my_king = pos.black_kings;
        enemy_rooks = pos.white_rooks;
        enemy_bishops = pos.white_bishops;
        enemy_queens = pos.white_queens;
        my_pieces = get_all_black();
    }

    // calculating cardinal lines of sights
    king_vert_attacks = vert_flood(my_king);
    king_hori_attacks = hori_flood(my_king);

    rook_vert_attacks = vert_flood(enemy_rooks | enemy_queens);
    rook_hori_attacks = hori_flood(enemy_rooks | enemy_queens);

    card_los = (king_vert_attacks & rook_vert_attacks) | 
               (king_hori_attacks & rook_hori_attacks);

    // calculating diagonal lines of sights
    king_left_diag_attacks = left_diag_flood(my_king);
    king_right_diag_attacks = right_diag_flood(my_king);

    bishop_left_diag_attacks = left_diag_flood(enemy_bishops | enemy_queens);
    bishop_right_diag_attacks = right_diag_flood(enemy_bishops | enemy_queens);
    
    diag_los = (king_left_diag_attacks & bishop_left_diag_attacks) | 
               (king_right_diag_attacks & bishop_right_diag_attacks);    

    // calculates pins
    the_card_pinned = card_los & my_pieces;
    the_diag_pinned = diag_los & my_pieces;

    all_pinned = the_card_pinned | the_diag_pinned;

    // get moves for the pinned
    while(the_card_pinned)
    {
        one_p = lsb_board(the_card_pinned);
        if(get_file(my_king) == get_file(one_p))
        {
            pin_card_helper(color, get_piece_by_bitboard(color, one_p), one_p, get_file(my_king), move_list);
        }
        else
        {
            pin_card_helper(color, get_piece_by_bitboard(color, one_p), one_p, get_rank(my_king), move_list);
        }
        the_card_pinned = the_card_pinned - one_p;
    }

    while(the_diag_pinned)
    {
        one_p = lsb_board(the_diag_pinned);
        int king_diag = get_diag(get_rank(my_king), get_file(my_king));
        int pin_diag = get_diag(get_rank(one_p), get_file(one_p));
        if((king_diag >> 5) == (pin_diag >> 5))
        {
            pin_diag_helper(color, get_piece_by_bitboard(color, one_p), king_diag >> 5, diag_los, move_list);
        }
        else
        {
            pin_diag_helper(color, get_piece_by_bitboard(color, one_p), king_diag & 0x000000000000000F, diag_los, move_list);
        }
        // print_chess_rep(the_diag_pinned);
        // std::cout << std::endl;
        // print_chess_rep(card_los);
        // print_chess_char();
        // exit(0);
        the_diag_pinned = the_diag_pinned - one_p;
    }

    // Defenders in attacker squares are pinned pieces
    return(all_pinned);
}

unsigned long long Engine::danger_squares(int color)
{
    unsigned long long danger = 0;
    unsigned long long temp_king;

    if(color)
    {
        temp_king = pos.white_kings;
        pos.white_kings = 0;

        danger = hori_flood(pos.black_rooks | pos.black_queens);
        danger = danger | vert_flood(pos.black_rooks | pos.black_queens);
        danger = danger | left_diag_flood(pos.black_bishops | pos.black_queens);
        danger = danger | right_diag_flood(pos.black_bishops | pos.black_queens);

        danger = danger | pre_check_black_pawn_attacks(pos.black_pawns);
        danger = danger | pre_check_night_attacks(pos.black_nights);
        danger = danger | pre_check_king_attacks(pos.black_kings);

        pos.white_kings = temp_king;
    }
    else
    {
        temp_king = pos.black_kings;
        pos.black_kings = 0;

        danger = hori_flood(pos.white_rooks | pos.white_queens);
        danger = danger | vert_flood(pos.white_rooks | pos.white_queens);
        danger = danger | left_diag_flood(pos.white_bishops | pos.white_queens);
        danger = danger | right_diag_flood(pos.white_bishops | pos.white_queens);

        danger = danger | pre_check_white_pawn_attacks(pos.white_pawns);
        danger = danger | pre_check_night_attacks(pos.white_nights);
        danger = danger | pre_check_king_attacks(pos.white_kings);

        pos.black_kings = temp_king;
    }
    return danger;
}

// Generates and returns a list of legal moves for a color
int* Engine::generate_legal_moves(int color)
{
    int *move_list = (int*) malloc(move_arr_size * sizeof(int)); 
    // int last_move_index = 0;
    move_list[0] = 0; // encode index in array

    unsigned long long pinned;
    int king_square = bitboard_to_square(get_bitboard_of_piece(KING, color));

    unsigned long long* check_info = get_attackers_blocks(color);
    unsigned long long danger = !danger_squares(color);
    // if(check_info[0] != 0)
    // {
    //     std::cout << "check_info: " << check_info[0] << std::endl;
    //     std::cout << "legals" << std::endl;
    //     print_chess_rep(check_info[1] | check_info[2]);
    // }

    // if(pos.white_kings == 0)
    // {
    //     // std::cout << "check_info: " << check_info[0] << std::endl;
    //     // std::cout << "legals" << std::endl;
    //     print_chess_char();
    //     exit(0);
    // }

    if(!check_info[0]) // not in check
    {
        pinned = pinned_pieces(color, move_list);
        if(pinned)
        {
            generate_moves_pinned(color, move_list, danger, pinned);
        }
        else
        {
            generate_moves(color, move_list, danger);
        }
    }
    else //in check
    {
        if(check_info[0] > 1) // double check
        {
            generate_moves_double_check(color, move_list, danger);
        }
        else // single check
        {
            pinned = pinned_pieces(color, move_list);
            if(pinned)
            {
                generate_moves_pinned_single_check(color, move_list, danger, check_info[1] | check_info[2], pinned);
            }
            else
            {
                generate_moves_single_check(color, move_list, danger, check_info[1] | check_info[2]);
            }
        }
    }

    int move_iter = 0;
    while(move_iter < move_list[0])
    {
        int move = move_list[move_iter+1];
        // if((pinned || decode_from(move) == king_square || decode_type(move) == ENPASSANT || check_status) && !(check_legal(move, color)))
        if(decode_type(move) == ENPASSANT && !check_legal(move, color))
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