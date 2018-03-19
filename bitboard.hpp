#ifndef BITBOARD_H
#define BITBOARD_H


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <bitset>
#include <unordered_map>


typedef unsigned long long U64;


enum Piece
{   
    NONE = 0,
    PAWN = 1,
    ROOK = 2,
    NIGHT = 3,
    BISHOP = 4,
    QUEEN = 5,
    KING = 6
};

enum MoveType
{
    REGULAR = 0,
    CASTLE = 1,
    ENPASSANT = 2,
    PROMOTION = 3
};

struct position
{
    U64 white_pawns; // 65280
    U64 white_rooks; // 129
    U64 white_nights; // 66
    U64 white_bishops;
    U64 white_queens;
    U64 white_kings;

    U64 black_pawns; // 71776119061217280
    U64 black_rooks; // 9295429630892703744
    U64 black_nights; // 4755801206503243776
    U64 black_bishops;
    U64 black_queens;
    U64 black_kings;


    U64 left_white_rook_square_moves = 0;
    U64 right_white_rook_square_moves = 0;
    U64 left_black_rook_square_moves = 0;
    U64 right_black_rook_square_moves = 0;

    U64 white_king_square_moves = 0;
    U64 black_king_square_moves = 0;

    int last_pawn_file; // -1 for none    
};


struct precomputed_masks
{
   U64 left_diag_mask_excluded;
   U64 right_diag_mask_excluded;
   U64 file_mask_excluded;
}; // 2 KByte





class Engine
{
    public:
        Engine();
        Engine(U64 *board_data);


        //Vars
        position pos;


        // Get piece bitboards
        U64 get_all_white();
        U64 get_all_black();
        U64 get_all();

        //printing
        void print_chess_rep(U64 num);
        void print_chess_char();
        void write_move_to_file(int file_num);


        // TEMPORARILY PUBLIC

        U64 *row_mask;
        U64 *col_mask;
        U64 *diag_left_mask;
        U64 *diag_right_mask;


        void init_engine();
        void reset_engine();
        void init_position();
        void init_position(U64 *board_data);
        void init_lsb_lookup();
        void clean_up();
        
        // masks
        void init_masks();
        U64 make_col_mask(U64 mask);
        void fill_col_mask_arr();
        U64 make_row_mask(U64 mask);
        void fill_row_mask_arr();
        U64 make_diag_left_mask(U64 mask);
        void fill_diag_left_mask_arr();
        U64 make_diag_right_mask(U64 mask);
        void fill_diag_right_mask_arr();

        void fill_square_masks();

        // maximum number of moves allowed in the game
        int get_max_move_length();

        // get rank/file/diag info
        int get_rank(U64 num);
        int get_file(U64 num);
        // int* get_diag(int rank, int file);
        int get_diag(int rank, int file);

        // move encoding and decoding
        int encode_move(int start, int end, int m_type, int piece, int promotion, int color);
        int decode_from(int move);
        int decode_to(int move);
        int decode_type(int move);
        int decode_piece(int move);
        int decode_promo(int move);
        int decode_color(int move);
        std::string piece_type_to_string(int move);
        void print_move_info(int move);

        // pushing and popping moves from stack
        void stack_push(int move);
        int stack_pop();
        void push_psuedo_move(int move);
        void push_move(int move);
        void pop_psuedo_move();
        void pop_move();
        void compute_get_holders();

        // bitboard tricks
        int lsb_digit(U64 board);
        U64 lsb_board(U64 board);
        U64 msb_digit(U64 board);
        U64 msb_board(U64 board);

        // reversing and flipping
        U64 reverse_8_bits(U64 x);
        U64 reverse_64_bits(U64 x);
        U64 horizontal_flip(U64 x);
        U64 vertical_flip(U64 x);

        // board helper functions
        int bitboard_to_square(U64 piece);
        U64 square_to_bitboard(int square);
        U64 get_bitboard_of_piece(Piece piece, int color);
        Piece get_piece_by_bitboard(int color, U64 board);
        Piece get_piece_by_bitboard(U64 board);
        void remove_piece(int color, int type, U64 board);
        void place_piece(int color, int type, U64 board);
        int get_color_by_bitboard(U64 board);
        U64* get_board_rep();
        void load_in_string(std::string rep);

        //move gen
        void pop_and_add_regular_moves(int color, int inv_color, int* move_list, U64 board, int curr_pos);
        void generate_moves(int color, int* move_list, U64 danger, U64 pinned);
        // void generate_moves(int color, int* move_list, U64 danger);
        // void generate_moves_pinned(int color, int* move_list, U64 danger, U64 pinned);
        void generate_moves_single_check(int color, int* move_list, U64 danger, U64 legal_defense, U64 pinned);
        // void generate_moves_single_check(int color, int* move_list, U64 danger, U64 legal_defense);
        // void generate_moves_pinned_single_check(int color, int* move_list, U64 danger, U64 legal_defense, U64 pinned);
        void generate_moves_double_check(int color, int* move_list, U64 danger);
        void pin_card_helper(int color, Piece p, U64 board, U64 los, int* move_list);
        void pin_diag_helper(int color, Piece p, U64 board, U64 los, int* move_list);
        U64 pinned_pieces(int color, int* move_list);
        U64 danger_squares(int color);
        int* generate_legal_moves(int color);

        // board state helpers
        bool check_legal(int move, int color);
        int is_terminal(int color, int* moves);
        bool get_in_check(int color);
        U64* get_attackers_blocks(int color);


        U64 pre_check_white_pawn_attacks(U64 white_pawns);
        U64 pre_check_black_pawn_attacks(U64 black_pawns);
        U64 pre_check_white_pawn_moves(U64 white_pawns, U64 all_pieces, U64 all_black_pieces);
        U64 pre_check_black_pawn_moves(U64 black_pawns, U64 all_pieces, U64 all_white_pieces);

        U64 pre_check_king_attacks(U64 kings);
        U64 pre_check_king_moves(U64 kings, U64 own_occupied);
        U64 pre_check_king_moves(int color);

        U64 pre_check_night_attacks(U64 nights);
        U64 pre_check_night_moves(U64 nights, U64 own_occupied);
        U64 pre_check_night_moves(int color);

        U64 pre_check_one_bishop_attacks_ANTI(U64 bishop, int square);
        U64 pre_check_one_bishop_attacks(U64 bishop);
        U64 pre_check_bishop_attacks(U64 bishops);
        U64 pre_check_one_bishop_moves(U64 bishops, U64 all_occupied, U64 own_occupied);
        U64 pre_check_bishop_moves(U64 bishops, U64 all_occupied, U64 own_occupied);
        U64 pre_check_bishop_moves(int color);
        U64 pre_check_bishop_moves(U64 bishops, int color);
        // U64 bishop_left_flood(U64 bishops1, U64 bishops2);
        // U64 bishop_right_flood(U64 bishops1, U64 bishops2);

        U64 pre_check_one_rook_attacks(U64 rook);
        U64 pre_check_rook_attacks(U64 rooks);
        U64 pre_check_one_rook_moves(U64 rook, U64 all_occupied, U64 own_occupied);
        U64 pre_check_rook_moves(U64 rooks, U64 all_occupied, U64 own_occupied);
        U64 pre_check_rook_moves(int color);
        U64 pre_check_rook_moves(U64 rooks, int color);

        U64 pre_check_one_queen_attacks(U64 queen);
        U64 pre_check_queen_attacks(U64 queens);
        U64 pre_check_one_queen_moves(U64 queen, U64 all_occupied, U64 own_occupied);
        U64 pre_check_queen_moves(U64 queens, U64 all_occupied, U64 own_occupied);
        U64 pre_check_queen_moves(int color);
        U64 pre_check_queen_moves(U64 queens, int color);

        //floods
        U64 vert_flood(U64 rooks);
        U64 hori_flood(U64 rooks);

        U64 north_flood(U64 rooks, U64 prop);
        U64 south_flood(U64 rooks, U64 prop);
        U64 east_flood(U64 rooks, U64 prop);
        U64 west_flood(U64 rooks, U64 prop);

        U64 left_diag_flood(U64 bishops);
        U64 right_diag_flood(U64 bishops);

        U64 north_east_flood(U64 bishops, U64 prop);
        U64 south_east_flood(U64 bishops, U64 prop);
        U64 south_west_flood(U64 bishops, U64 prop);
        U64 north_west_flood(U64 bishops, U64 prop);


    private: 
        int max_move_length;
        int move_arr_size;

        int stack_index;
        int* move_stack; 
        bool in_check;

        std::unordered_map<U64, int> lsb_lookup;
        int *move_list;

        precomputed_masks square_masks[64];

        // get all savers
        U64 get_all_holder, get_black_holder, get_white_holder;

        const int index64[64] = {
            0,  1, 48,  2, 57, 49, 28,  3,
           61, 58, 50, 42, 38, 29, 17,  4,
           62, 55, 59, 36, 53, 51, 43, 22,
           45, 39, 33, 30, 24, 18, 12,  5,
           63, 47, 56, 27, 60, 41, 37, 16,
           54, 35, 52, 21, 44, 32, 23, 11,
           46, 26, 40, 15, 34, 20, 31, 10,
           25, 14, 19,  9, 13,  8,  7,  6
        };
};

#endif