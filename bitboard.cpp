#include "bitboard.hpp"



Engine::Engine()
{
    init_position();
    init_engine();
}

Engine::Engine(unsigned long long *board_data)
{
    init_position(board_data);
    init_engine();
}

void Engine::init_engine()
{
    max_move_length = 500; // This assumes there are only 500 possible legal moves at any one time (affects move array intilization)
    move_arr_size = 500;

    move_stack = (int*) malloc(max_move_length * sizeof(int));
    stack_index = -1;

    in_check = false;
    init_masks();
}

void Engine::init_position()
{
    pos.white_pawns = 0b0000000000000000000000000000000000000000000000001111111100000000; // 65280
    pos.white_rooks = 0b0000000000000000000000000000000000000000000000000000000010000001; // 129
    pos.white_nights = 0b0000000000000000000000000000000000000000000000000000000001000010; // 66
    pos.white_bishops = 0b0000000000000000000000000000000000000000000000000000000000100100;
    pos.white_queens = 0b0000000000000000000000000000000000000000000000000000000000001000;
    pos.white_kings = 0b0000000000000000000000000000000000000000000000000000000000010000;

    pos.black_pawns = 0b0000000011111111000000000000000000000000000000000000000000000000; // 71776119061217280
    pos.black_rooks = 0b1000000100000000000000000000000000000000000000000000000000000000; // 9295429630892703744
    pos.black_nights = 0b0100001000000000000000000000000000000000000000000000000000000000; // 4755801206503243776
    pos.black_bishops = 0b0010010000000000000000000000000000000000000000000000000000000000;
    pos.black_queens = 0b0000100000000000000000000000000000000000000000000000000000000000;
    pos.black_kings = 0b0001000000000000000000000000000000000000000000000000000000000000;
}

void Engine::init_position(unsigned long long *board_data)
{
    pos.white_pawns = board_data[0];
    pos.white_rooks = board_data[1];
    pos.white_nights = board_data[2];
    pos.white_bishops = board_data[3]; 
    pos.white_queens = board_data[4]; 
    pos.white_kings = board_data[5]; 

    pos.black_pawns = board_data[6];
    pos.black_rooks = board_data[7];  
    pos.black_nights = board_data[8];  
    pos.black_bishops = board_data[9]; 
    pos.black_queens = board_data[10]; 
    pos.black_kings = board_data[11];
}

void Engine::init_masks()
{
    row_mask = (unsigned long long*) malloc(8 * sizeof(unsigned long long));    
    fill_row_mask_arr();

    col_mask = (unsigned long long*) malloc(8 * sizeof(unsigned long long));
    fill_col_mask_arr();

    diag_left_mask = (unsigned long long*) malloc(15 * sizeof(unsigned long long));
    fill_diag_left_mask_arr();
    // Diag left masks start on left side and moves from left to right, top to bottom
    // [0] corresponds to bottom left corner
    // [0]-[7] moves up y axis along x=0
    // [7] is top left corner
    // [7]-[14] moves across x-axis along y=7
    // [14] is top right corner

    diag_right_mask = (unsigned long long*) malloc(15 * sizeof(unsigned long long));
    fill_diag_right_mask_arr();
    // Diag right masks start on bottom side and moves from left to right, bottom to top
    // [0] corresponds to bottom right corner
    // [0]-[7] moves down the x axis along y=0
    // [7] is bottom left corner
    // [7]-[14] moves up the y-axis along x=0
    // [14] is top left corner
}

unsigned long long Engine::make_col_mask(unsigned long long mask)
{
    for(int i = 0; i < 7; i++)
    {
        mask = mask | mask << 8;
    }
    return(mask);
}

void Engine::fill_col_mask_arr()
{
    for(int i = 0; i < 8; i++)
    {
        col_mask[i] = make_col_mask(1ULL << i);
    }
}

unsigned long long Engine::make_row_mask(unsigned long long mask)
{
    for(int i = 0; i < 7; i++)
    {
        mask = mask | mask << 1;
    }
    return(mask);
}

void Engine::fill_row_mask_arr()
{
    for(int i = 0; i < 8; i++)
    {
        row_mask[i] = make_row_mask(1ULL << 8*i);
    }
}

unsigned long long Engine::make_diag_left_mask(unsigned long long mask)
{
    unsigned long long BR_mask = ~(row_mask[0] | col_mask[7]);
    for(int i = 0; i < 8; i++)
    {
        mask = mask | ((mask & BR_mask) >> 7);
    }
    return(mask);
}


void Engine::fill_diag_left_mask_arr()
{
    unsigned long long start = 1;

    for(int i = 0; i < 8; i++)
    {
        diag_left_mask[i] = make_diag_left_mask(start);
        if(i != 7)
        {
            start = start << 8;
        }
    }
    start = start << 1;

    for(int j = 8; j < 15; j++)
    {
        diag_left_mask[j] = make_diag_left_mask(start);
        start = start << 1;
    }
}


unsigned long long Engine::make_diag_right_mask(unsigned long long mask)
{
    unsigned long long TR_mask = ~((row_mask[7]) | (col_mask[7]));
    for(int i = 0; i < 8; i++)
    {
        mask = mask | ((mask & TR_mask) << 9);
    }
    return(mask);
}


void Engine::fill_diag_right_mask_arr()
{
    unsigned long long start = 1ULL << 7;
    for(int i = 0; i < 8; i++)
    {
        diag_right_mask[i] = make_diag_right_mask(start);
        if(i != 7)
        {
            start = start >> 1;
        } 
    }
    start = start << 8;

    for(int j = 8; j < 15; j++)
    {
        diag_right_mask[j] = make_diag_right_mask(start);
        start = start << 8;
    }          
}

int Engine::get_max_move_length()
{
    return(max_move_length);
}

unsigned long long Engine::get_all_white()
{
    return(pos.white_pawns | pos.white_rooks | pos.white_nights | pos.white_bishops | pos.white_kings | pos.white_queens);
}


unsigned long long Engine::get_all_black()
{
    return(pos.black_pawns | pos.black_rooks | pos.black_nights | pos.black_bishops | pos.black_kings | pos.black_queens);
}


unsigned long long Engine::get_all()
{
    return(get_all_white() | get_all_black());
}


// Takes in a 64 bit number with single bit
// Returns the rank piece is on 0-7, bottom to top
// Alters nothing
int get_rank(unsigned long long num)
{
    unsigned long long max0 = 128ULL; // 2^7
    if(num <= max0)
    {
        return(0);
    }

    unsigned long long max1 = 32768ULL; // 2^15
    if(num <= max1)
    {
        return(1);
    }

    unsigned long long max2 = 8388608ULL; // 2^23
    if(num <= max2)
    {
        return(2);
    }

    unsigned long long max3 = 2147483648ULL; // 2^31
    if(num <= max3)
    {
        return(3);
    }

    unsigned long long max4 = 549755813888ULL; // 2^39
    if(num <= max4)
    {
        return(4);
    }

    unsigned long long max5 = 140737488355328ULL; // 2^47
    if(num <= max5)
    {
        return(5);
    }

    unsigned long long max6 = 36028797018963968ULL; // 2^55
    if(num <= max6)
    {
        return(6);
    }

    unsigned long long max7 = 9223372036854775808ULL; // 2^63
    if(num <= max7)
    {
        return(7);
    }
}


// Takes in a 64 bit number with single bit
// Returns the file piece is on 0-7, left to right
// Alters nothing 
int get_file(unsigned long long num)
{
    switch(num)
    {
        //2^[0, 8, 16, 24, 32, 40, 48, 56]
        case 1ULL:
        case 256ULL:
        case 65536ULL:
        case 16777216ULL:
        case 4294967296ULL:
        case 1099511627776ULL:
        case 281474976710656ULL:
        case 72057594037927936ULL:
            return(0);

        //2^[1,9,17,25,33,41,49,57]
        case 2ULL:
        case 512ULL:
        case 131072ULL:
        case 33554432ULL:
        case 8589934592ULL:
        case 2199023255552ULL:
        case 562949953421312ULL:
        case 144115188075855872ULL:
            return(1);

        // 2^[2,10,18,26,34,42,50,58]
        case 4ULL:
        case 1024ULL:
        case 262144ULL:
        case 67108864ULL:
        case 17179869184ULL:
        case 4398046511104ULL:
        case 1125899906842624ULL:
        case 288230376151711744ULL:
            return(2);

        // 2^[3,11,19,27,35,43,51,59]    
        case 8ULL:
        case 2048ULL:
        case 524288ULL:
        case 134217728ULL:
        case 34359738368ULL:
        case 8796093022208ULL:
        case 2251799813685248ULL:
        case 576460752303423488ULL:
            return(3);

        // 2^[4,12,20,28,36,44,52,60]
        case 16ULL:
        case 4096ULL:
        case 1048576ULL:
        case 268435456ULL:
        case 68719476736ULL:
        case 17592186044416ULL:
        case 4503599627370496ULL:
        case 1152921504606846976ULL:
            return(4);

        // 2^[5,13,21,29,37,45,53,61]
        case 32ULL:
        case 8192ULL:
        case 2097152ULL:
        case 536870912ULL:
        case 137438953472ULL:
        case 35184372088832ULL:
        case 9007199254740992ULL:
        case 2305843009213693952ULL:
            return(5);

        // 2^[6, 14, 22, 30, 38, 46, 54, 62]
        case 64ULL:
        case 16384ULL:
        case 4194304ULL:
        case 1073741824ULL:
        case 274877906944ULL:
        case 70368744177664ULL:
        case 18014398509481984ULL:
        case 4611686018427387904ULL:
            return(6);

        // 2^[7,15,23,31,39,47,55,63]
        case 128ULL:
        case 32768ULL:
        case 8388608ULL:
        case 2147483648ULL:
        case 549755813888ULL:
        case 140737488355328ULL:
        case 36028797018963968ULL:
        case 9223372036854775808ULL:
            return(7);

        default:
            return(-1);
    }
}

// Takes in a rank, and file
// Returns the left and right diagonal mask indexes as [left,right]
// Alters nothing 

int get_diag(unsigned long long rank, unsigned long long file)
{
    int total_val = rank+file;
    int right;

    //Total val also equals left diag index

    if(rank > file) //Above the middle diagonal line r = 7
    {
        right = 7+(total_val-2*file);
    }
    else //Below middle line
    {
        right = 7-(total_val-2*rank);
    }

    return(right);
    // int diag[2] = {total_val,right};
}

// Takes in move information
//     start int 0-63 
// Square moved piece started on
// //     end int 0-63 
// Square moved piece ended on
// //     m_type int 0-3 
//Type of move made
// //     piece int 0-4 
// Piece taken in move
// //     promotion int 2-5 
// Piece to promote pawn to
// Return a np.uint32 representing all above info
// Alters nothing
int Engine::encode_move(int start, int end, int m_type, int piece, int promotion)
{
    int encode_start = start;
    int encode_end = end << 6;
    int encode_type = m_type << 12;
    int encode_piece = piece << 14;
    int encode_promotion = promotion << 17;
    return(encode_start & encode_end & encode_type & encode_piece & encode_promotion);
}

// Takes in a np.uint32 move
// Returns square number moved piece originated from
// Alters nothing
int Engine::decode_from(int move)
{
    return(move & 63);
}

// Takes in a np.uint32 move
// Returns square number moved piece travels to
// Alters nothing
int Engine::decode_to(int move)
{
    return((move >> 6) & 63);
}

// Takes in a np.uint32 move
// Returns type of move made
// Alters nothing
int Engine::decode_type(int move)
{
    return((move >> 12) & 3);
}   

// Takes in a np.uint32 move
// Returns any piece taken by move
// Alters nothing
int Engine::decode_piece(int move)
{
    return((move >> 14) & 7);
}

// Takes in a np.uint32 move
// Returns new piece pawn promoted to
// Alters nothing
int Engine::decode_promo(int move)
{
    return((move >> 17) & 3);
}

// Takes in a move to be added to the move stack
// Returns nothing
// Alters the move stack and stack_index value
void Engine::stack_push(int move)
{
    // get pointer to stack index
    // get pointer to move_stack
    move_stack[++stack_index] = move;
}

// Takes in nothing
// Returns the last move in the move stack
// Alters the stack_index value
unsigned long long Engine::stack_pop()
{
    // get pointer to stack index
    // get pointer to move_stack
    return(move_stack[stack_index--]);
}

// Takes in a move, alters the BitboardEngine's representation to the NEXT state based on the CURRENT move action
// Currently 
void Engine::push_move(int move)
{
    stack_push(move);
    int start = decode_from(move);
    int end = decode_from(move);
    int taken = decode_piece(move);

    unsigned long long bb_start = 2^start;
    unsigned long long bb_end = 2^end;

    curr_piece = // LOOKUP VAL -> pointer to piece val on that square
    curr_piece = (curr_piece | bb_end) & (get_uint64_max()-bb_start)

    // EDIT LOOKUP TABLE THAT THE USED SQUARE IS NOW EMPTY
    // self.edit_lookup(start,None)

    if taken:
        taken_piece = //LOOKUP VAL -> pointer to piece val on that square
        taken_piece = taken_piece & (get_uint64_max()-bb_end)

    // EDIT LOOKUP TABLE THAT THE FINAL SQUARE IS NOW NEW PIECE
    // self.edit_lookup(end,curr_piece)
}

// Takes in a move, alters the BitboardEngine's representation to the PREVIOUS state based on the LAST move action
void Engine::pop_move(int move)
{
    //
}

//REWRITE
// Takes in a bitboard and will return the bitboard representing only the least significant bit.
// Example: the initial white_nights bitboard, the least significant 1 occurs at index 1 (...00001000010)
// therefore simply return ((lots of zeros)00000000000010)
// YOU MAY ASSUME A 1 EXISTS, (0000000000000000000) will not be given
int Engine::lsb_digit(unsigned long long board)
{
    return((board & -board).bit_length()-1);
}

// Takes in a bitboard
// Returns a bitboard with soley the least significant bit = 1
// All other bits = 0
// Alters nothing
unsigned long long Engine::lsb_board(unsigned long long board)
{
    return(board & -board);
}


// See above, except return the move_list significant bit bitboard
void Engine::msb(unsigned long long board)
{
    //
}


// REWRITE
// Reverses a uint8 number, like this (00110000 -> 00001100)
// void Engine::reverse_8_bit(int row)
// {
//     num = np.uint8(row)
//     reverse_num = np.uint8(row)
//     one_8 = np.uint8(1)
//     count = np.uint8(7);
     
//     num = num >> one_8
//     while(num)
//     {
//         reverse_num = reverse_num << one_8;
//         reverse_num = reverse_num | (num & one_8);
//         num = num >> one_8;
//         count -= one_8;
//     }
//     reverse_num = reverse_num << count
//     return reverse_num
//     // return ~(np.uint8(255) - np.uint8(row))
// }

unsigned long long Engine::reverse_8_bits(unsigned long long x)
{
    return (x * 0x0202020202 & 0x010884422010) % 1023;
}


unsigned long long Engine::reverse_64_bits(unsigned long long x)
{
    return vertical_flip(horizontal_flip(x));
    // return (x * np.uint64(0x0202020202) & np.uint64(0x010884422010)) % np.uint64(1023);
}

unsigned long long Engine::horizontal_flip(unsigned long long x)
{
    unsigned long long k1 = 0x5555555555555555;
    unsigned long long k2 = 0x3333333333333333;
    unsigned long long k4 = 0x0f0f0f0f0f0f0f0f;
    unsigned long long x = ((x >> 1) & k1) + 2 * (x & k1);
    x = ((x >> 2) & k2) + 4 * (x & k2);
    x = ((x >> 4) & k4) + 16 * (x & k4);
    return x;
}


//REWRITE
unsigned long long Engine::vertical_flip(unsigned long long x)
{
    return _byteswap_uint64(x);
}

//REWRITE
void Engine::print_chess_rep(unsigned long long board)
{
    // for i in range(7, -1, -1)
    // {
    //     shifter = i * 8;
    //     row = (board & self.row_mask[i]) >> shifter;
    //     rev = reverse_8_bits(row);
    //     // print('{0}'.format(rev));
    // }
    unsigned long long to_print = horizontal_flip(board);
    unsigned long long row;
    for(int i = 7; i > -1; i--)
    {
        shifter = i * 8;
        row = (to_print & row_mask[i]) >> shifter;
        std::bitset<8> lol(row);
        std::cout << lol << std::endl;
    }
}

// East << 1
// Southeast >> 7
// South >> 8
// Southwest >> 9
// West >> 1
// Northwest << 7
// North << 8
// Northeast << 9

int Engine::get_square(int piece, int color)
{
    // white
    if(color)
    {
        if(piece == Piece.KING)
        {
            return pos.white_kings;
        }
            return self.white_kings
        else
        {
            //
        }
    }
    // black
    else
    {
        if(piece == Piece.KING)
        {
            return pos.black_kings;
        }
            return self.black_kings
        else
        {
            //
        }
    } 
}


// Some hueristics have been met, the only way to check if a move is legal or not, we must make it.
bool Engine::check_legal(int move)
{
    //
}


// Returns a bitboard of pieces that are pinned against their king 
unsigned long long Engine::pinned_pieces(int color)
{
    unsigned long long defenders;
    unsigned long long enemy_rook;
    unsigned long long enemy_bishop;
    unsigned long long enemy_queen;
    unsigned long long enemy_color;

    // Replace king with enemy queen
    // Find kings defenders
    // Declare enemy
    // white
    if(color)
    {
        defenders = queen_attacks(pos.white_kings, 0);
        enemy_rook = pos.black_rooks;
        enemy_bishop = pos.black_bishops;
        enemy_queen = pos.black_queens;
        enemy_color = 0;
    }
    // black
    else
    {
        defenders = queen_attacks(pos.black_kings, 1);
        enemy_rook = pos.white_rooks;
        enemy_bishop = pos.white_bishops;
        enemy_queen = pos.white_queens;
        enemy_color = 1;
    }
    // Compile all squares under attack from enemy
    unsigned long long attacker_squares = rook_attacks(enemy_rook, enemy_color) | bishop_attacks(enemy_bishop, enemy_color) | queen_attacks(enemy_queen, enemy_color);
    // Defenders in attacker squares are pinned pieces
    return(defenders & attacker_squares);
}


// Generates and fills move_list for a color before checking checks
void Engine::generate_pre_check_moves(int color, int* move_list)
{
    // king_loc = pre_check_king_bitboard();
    // return(all_pre_check_moves);
}


// Generates and returns a list of legal moves for a color
int* Engine::generate_legal_moves(int color)
{
    int *all_legal_moves = (int*) malloc(move_arr_size * sizeof(int)); 
    int last_move_index = 0;

    pinned = self.pinned_pieces(color);;
    king_square = self.get_square(Piece.KING, color);

    if(in_check)
    {
        // generate<EVASIONS>(pos, moveList)         last_move_index returned
    }
    else
    {
        // generate<NON_EVASIONS>(pos, moveList)     last_move_index returned
    }

    int move_iter = 0;
    while(move_iter < last_move_index)
    {
        move = all_legal_moves[move_iter]
        if (pinned or decode_from(move) == king_square or self.decode_type(move) == MoveType.ENPASSANT) and not self.check_legal(move)
        {
            last_move_index -= 1
            all_legal_moves[move_iter] = all_legal_moves[last_move_index]
        }
    }
    return all_legal_moves
}


void Engine::extract_moves(int* moves, unsigned long long move_board, int curr_pos, int t, int piece, int promo)
{
    while(board)
    {
        move = self.lsb_board(board)
        self.encode_move(lsb_digit(curr_pos), lsb_digit(move_board), t, piece, promo)
        board = board & (~move)
    }
}


unsigned long long Engine::one_rook_attack(self, board, color)
{
    // row = np.uint64(2)
    // col = np.uint64(6)

    // s = board
    // o = self.get_all()

    // // white
    // if color == 1
    //     own = self.get_all_white()
    // // black
    // else
    //     own = self.get_all_black()

    // o_rev = self.reverse_64_bits(o)
    // s_rev = self.reverse_64_bits(s)
    // two = np.uint64(2)

    // hori = (o - two*s) ^ self.reverse_64_bits(o_rev - two*s_rev)
    // hori = hori & self.row_mask[row]


    // o_mask = o & self.col_mask[col]
    // o_rev_mask = self.reverse_64_bits(o_mask)
    // vert = (o_mask - two*s) ^ self.reverse_64_bits(o_rev_mask - two*s_rev)
    // vert = vert & self.col_mask[col]

    // res = hori | vert
    // return res & ~own
}


// void Engine::rook_attacks(self, board, color)
//     new = np.uint64(0)
//     while board
//         s = self.lsb_board(board)
//         p = (o - s - s) ^ self.reverse_64_bits((~o) - (~s) - (~s))
//         new = new | p
//         board = board - s
//     return new


unsigned long long Engine::one_bishop_attack(self, board, color)
{
    // lineMask = diagonalMaskEx[sqOfSlider]; // excludes square of slider
    // slider   = singleBitboard[sqOfSlider]; // single bit 1 << sq, 2^sq

    // forward  = occ & lineMask; // also performs the first subtraction by clearing the s in o
    // reverse  = byteswap( forward ); // o'-s'
    // forward -=         ( slider  ); // o -2s
    // reverse -= byteswap( slider  ); // o'-2s'
    // forward ^= byteswap( reverse );
    // return forward & lineMask;      // mask the line again
}



// unsigned long long Engine::bishop_attacks(board, color)
// {
//     //
// }


unsigned long long Engine::queen_attacks(board, color)
{
    return(self.rook_attacks(board,color) | self.bishop_attacks(board,color));
}


// Takes in king_rep (bitboad representing that colors king location)
// Takes in same_occupied (bitboard representing all pieces of that color)
// Returns bitboard representing all possible pre_check moves that the king can make
unsigned long long Engine::pre_check_king_bitboard(unsigned long long king_rep, same_occupied)
{
    unsigned long long king_mask_file_0 = king_rep & ~self.col_mask[0];
    unsigned long long king_mask_file_7 = king_rep & ~self.col_mask[7];

    unsigned long long spot_0 = king_mask_file_7 >> 7; // Southeast
    unsigned long long spot_1 = king_rep >> 8; // South
    unsigned long long spot_2 = king_mask_file_7 >> 9; // Southwest
    unsigned long long spot_3 = king_mask_file_7 >> 1; // West

    unsigned long long spot_4 = king_mask_file_0 << 7; // Northwest
    unsigned long long spot_5 = king_rep << 8; // North
    unsigned long long spot_6 = king_mask_file_0 << 9; // Northeast
    unsigned long long spot_7 = king_rep << 1; // East

    unsigned long long king_moves = spot_0 | spot_1 | spot_2 | spot_3 | spot_4 | spot_5 | spot_6 | spot_7;

    return king_moves & ~same_occupied;
}


unsigned long long Engine::get_king_moves(int color)
{
    if(color == 1)
    {
        return(self.pre_check_king_bitboard(self.white_kings, self.get_all_white()));
    }
    else
    {
        return(self.pre_check_king_bitboard(self.black_kings, self.get_all_black()));
    }
}


// Takes in night_rep (bitboad representing that colors night location)
// Takes in same_occupied (bitboard representing all pieces of that color)
// Returns bitboard representing all possible pre_check moves that that night can make
void Engine::pre_check_night(unsigned long long king_rep, unsigned long long same_occupied)
{
    // pass
    // // spot_1_clip = tbls->ClearFile[FILE_A] & tbls->ClearFile[FILE_B];
    // // spot_2_clip = tbls->ClearFile[FILE_A];
    // // spot_3_clip = tbls->ClearFile[FILE_H];
    // // spot_4_clip = tbls->ClearFile[FILE_H] & tbls->ClearFile[FILE_G];

    // // spot_5_clip = tbls->ClearFile[FILE_H] & tbls->ClearFile[FILE_G];
    // // spot_6_clip = tbls->ClearFile[FILE_H];
    // // spot_7_clip = tbls->ClearFile[FILE_A];
    // // spot_8_clip = tbls->ClearFile[FILE_A] & tbls->ClearFile[FILE_B];

    // // spot_1 = (night_loc & spot_1_clip) << 6;
    // // spot_2 = (night_loc & spot_2_clip) << 15;
    // // spot_3 = (night_loc & spot_3_clip) << 17;
    // // spot_4 = (night_loc & spot_4_clip) << 10;

    // // spot_5 = (night_loc & spot_5_clip) >> 6;
    // // spot_6 = (night_loc & spot_6_clip) >> 15;
    // // spot_7 = (night_loc & spot_7_clip) >> 17;
    // // spot_8 = (night_loc & spot_8_clip) >> 10;

    // // nightValid = spot_1 | spot_2 | spot_3 | spot_4 | spot_5 | spot_6 |
    // //                 spot_7 | spot_8;

    // // /* compute only the places where the night can move and attack. The
    // //     caller will determine if this is a white or black night. */
    // // return nightValid & ~own_side;
}
