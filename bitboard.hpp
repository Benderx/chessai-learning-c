#ifndef BITBOARD_H
#define BITBOARD_H


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <bitset>
#include <unordered_map>


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
    unsigned long long white_pawns; // 65280
    unsigned long long white_rooks; // 129
    unsigned long long white_nights; // 66
    unsigned long long white_bishops;
    unsigned long long white_queens;
    unsigned long long white_kings;

    unsigned long long black_pawns; // 71776119061217280
    unsigned long long black_rooks; // 9295429630892703744
    unsigned long long black_nights; // 4755801206503243776
    unsigned long long black_bishops;
    unsigned long long black_queens;
    unsigned long long black_kings;
};





class Engine
{
    public:
        Engine();
        Engine(unsigned long long *board_data);


        //Vars
        position pos;


        // Get piece bitboards
        unsigned long long get_all_white();
        unsigned long long get_all_black();
        unsigned long long get_all();

        // pushing and popping moves from stack
        void push_move(int move);
        void pop_move();

        //printing
        void print_chess_rep(unsigned long long num);
        void print_chess_char();

        //move gen
        int* generate_legal_moves(int color);


        // TEMPORARILY PUBLIC

        unsigned long long *row_mask;
        unsigned long long *col_mask;
        unsigned long long *diag_left_mask;
        unsigned long long *diag_right_mask;


        void init_engine();
        void reset_engine();
        void init_position();
        void init_position(unsigned long long *board_data);
        void init_lsb_lookup();
        
        // masks
        void init_masks();
        unsigned long long make_col_mask(unsigned long long mask);
        void fill_col_mask_arr();
        unsigned long long make_row_mask(unsigned long long mask);
        void fill_row_mask_arr();
        unsigned long long make_diag_left_mask(unsigned long long mask);
        void fill_diag_left_mask_arr();
        unsigned long long make_diag_right_mask(unsigned long long mask);
        void fill_diag_right_mask_arr();

        // maximum number of moves allowed in the game
        int get_max_move_length();

        // get rank/file/diag info
        int get_rank(unsigned long long num);
        int get_file(unsigned long long num);
        int* get_diag(int rank, int file);

        // move encoding and decoding
        int encode_move(int start, int end, int m_type, int piece, int promotion);
        int decode_from(int move);
        int decode_to(int move);
        int decode_type(int move);
        int decode_piece(int move);
        int decode_promo(int move);
        std::string piece_type_to_string(int move);
        void print_move_info(int move);

        // pushing and popping moves from stack
        void stack_push(int move);
        int stack_pop();

        // bitboard tricks
        int lsb_digit(unsigned long long board);
        unsigned long long lsb_board(unsigned long long board);
        unsigned long long msb_digit(unsigned long long board);
        unsigned long long msb_board(unsigned long long board);

        // reversing and flipping
        unsigned long long reverse_8_bits(unsigned long long x);
        unsigned long long reverse_64_bits(unsigned long long x);
        unsigned long long horizontal_flip(unsigned long long x);
        unsigned long long vertical_flip(unsigned long long x);

        // board helper functions
        int bitboard_to_square(unsigned long long piece);
        unsigned long long square_to_bitboard(int square);
        int get_bitboard_of_piece(Piece piece, int color);
        Piece get_piece_by_bitboard(int color, unsigned long long board);
        Piece get_piece_by_bitboard(unsigned long long board);
        void remove_piece(int color, int type, unsigned long long board);
        void place_piece(int color, int type, unsigned long long board);
        int get_color_by_bitboard(unsigned long long board);
        unsigned long long* get_board_rep();

        //move gen helpers
        bool check_legal(int move, int color);
        unsigned long long pinned_pieces(int color);
        void pop_and_add_regular_moves(int color, int* move_list, unsigned long long board, int curr_pos);
        void generate_pre_check_moves(int color, int* move_list, unsigned long long pinned);

        //attacks and moves
        int is_terminal(int color, int* moves);
        bool get_in_check(int color);


        unsigned long long pre_check_white_pawn_attacks(unsigned long long white_pawns);
        unsigned long long pre_check_black_pawn_attacks(unsigned long long black_pawns);
        unsigned long long pre_check_white_pawn_moves(unsigned long long white_pawns, unsigned long long all_pieces, unsigned long long all_black_pieces);
        unsigned long long pre_check_black_pawn_moves(unsigned long long black_pawns, unsigned long long all_pieces, unsigned long long all_white_pieces);

        unsigned long long pre_check_king_attacks(unsigned long long kings);
        unsigned long long pre_check_king_moves(unsigned long long kings, unsigned long long own_occupied);
        unsigned long long pre_check_king_moves(int color);

        unsigned long long pre_check_night_attacks(unsigned long long nights);
        unsigned long long pre_check_night_moves(unsigned long long nights, unsigned long long own_occupied);
        unsigned long long pre_check_night_moves(int color);

        unsigned long long pre_check_one_bishop_attacks(unsigned long long bishop);
        unsigned long long pre_check_bishop_attacks(unsigned long long bishops);
        unsigned long long pre_check_one_bishop_moves(unsigned long long bishops, unsigned long long all_occupied, unsigned long long own_occupied);
        unsigned long long pre_check_bishop_moves(unsigned long long bishops, unsigned long long all_occupied, unsigned long long own_occupied);
        unsigned long long pre_check_bishop_moves(int color);
        unsigned long long pre_check_bishop_moves(unsigned long long piece, int color);

        unsigned long long pre_check_one_rook_attacks(unsigned long long rook);
        unsigned long long pre_check_rook_attacks(unsigned long long rooks);
        unsigned long long pre_check_one_rook_moves(unsigned long long rook, unsigned long long all_occupied, unsigned long long own_occupied);
        unsigned long long pre_check_rook_moves(unsigned long long rooks, unsigned long long all_occupied, unsigned long long own_occupied);
        unsigned long long pre_check_rook_moves(int color);
        unsigned long long pre_check_rook_moves(unsigned long long piece, int color);

        unsigned long long pre_check_one_queen_attacks(unsigned long long queen);
        unsigned long long pre_check_queen_attacks(unsigned long long queens);
        unsigned long long pre_check_one_queen_moves(unsigned long long queen, unsigned long long all_occupied, unsigned long long own_occupied);
        unsigned long long pre_check_queen_moves(unsigned long long queens, unsigned long long all_occupied, unsigned long long own_occupied);
        unsigned long long pre_check_queen_moves(int color);
        unsigned long long pre_check_queen_moves(unsigned long long piece, int color);


    private: 
        int max_move_length;
        int move_arr_size;

        int stack_index;
        int* move_stack; 
        bool in_check;

        std::unordered_map<unsigned long long, int> lsb_lookup;

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