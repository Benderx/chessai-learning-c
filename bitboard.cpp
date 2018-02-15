#include "bitboard.hpp"
#include <stdlib.h>
#include <unordered_map>
#include <cmath>



Engine::Engine()
{
    init_position();
    init_engine();
    init_lsb_lookup();
}

Engine::Engine(unsigned long long* board_data)
{
    init_position(board_data);
    init_engine();
    init_lsb_lookup();
}

void Engine::init_engine()
{
    max_move_length = 500; // This assumes there are only 500 possible legal moves at any one time (affects move array intilization)
    move_arr_size = 500;

    move_stack = (int*) malloc((max_move_length + 1) * sizeof(int)); // +1 because pushing moves is nesseccary to chekc fo legality
    stack_index = -1;

    in_check = false;
    init_masks();
}

void Engine::reset_engine()
{
    stack_index = -1;
    in_check = false;
    init_position();
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

void Engine::init_lsb_lookup()
{
    for(int i=0;i<64;i++)
    {
        lsb_lookup.insert({(unsigned long long) std::pow(2,i),i});
    }
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
int Engine::get_rank(unsigned long long num)
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
    return -1;
}


// Takes in a 64 bit number with single bit
// Returns the file piece is on 0-7, left to right
// Alters nothing 
int Engine::get_file(unsigned long long num)
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
// Mallocs int array of size 2. Free when done
// Look here for issues with diag, altered in confusion
int* Engine::get_diag(int rank, int file)
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

    int* diag = (int*) malloc(2 * sizeof(int));
    diag[0] = total_val;
    diag[1] = right;

    return(diag);
}

// Takes in move information
//     start int 0-63 
// Square moved piece started on
// //     end int 0-63 
// Square moved piece ended on
// //     m_type int 0-63 
// Type of move made
// //     piece int 0-4 
// Piece taken in move
// //     promotion int 2-5 
// Piece to promote pawn to
// Return an int representing all above info
// Alters nothing
int Engine::encode_move(int start, int end, int m_type, int piece, int promotion)
{
    int encode_start = start;
    int encode_end = end << 6;
    int encode_type = m_type << 12;
    int encode_piece = piece << 14;
    int encode_promotion = promotion << 17;
    return(encode_start | encode_end | encode_type | encode_piece | encode_promotion);
}

// Takes in an int move
// Returns square number the moved piece originated from
// Alters nothing
int Engine::decode_from(int move)
{
    return(move & 63);
}

// Takes in an int move
// Returns square number moved piece travels to
// Alters nothing
int Engine::decode_to(int move)
{
    return((move >> 6) & 63);
}

// Takes in an int move
// Returns type of move made
// Alters nothing
int Engine::decode_type(int move)
{
    return((move >> 12) & 3);
}   

// Takes in an int move
// Returns any piece taken by move
// Alters nothing
int Engine::decode_piece(int move)
{
    return((move >> 14) & 7);
}

// Takes in an int move
// Returns new piece pawn promoted to
// Alters nothing
int Engine::decode_promo(int move)
{
    return((move >> 17) & 3);
}

std::string Engine::piece_type_to_string(int piece)
{
    if(piece == PAWN)
    {
        return("pawn");
    }
    else if(piece == ROOK)
    {
        return("rook");
    }
    else if(piece == NIGHT)
    {
        return("night");
    }
    else if(piece == BISHOP)
    {
    return("bishop");
    }
    else if(piece == QUEEN)
    {
        return("queen");
    }
    else if(piece == KING)
    {
        return("king");
    }
    else if(piece == NONE)
    {
        return("nothing");
    }
    else
    {
        return("WARNING, PIECE INFO IS GARBAGE");
    }
}

void Engine::print_move_info(int move)
{
    int piece, piece_taken;
    std::string piece_name, piece_taken_name;
    int move_type;
    
    piece = get_piece_by_bitboard(square_to_bitboard(decode_from(move)));
    piece_taken = decode_piece(move);

    piece_name = piece_type_to_string(piece);
    piece_taken_name = piece_type_to_string(piece_taken);

    move_type = decode_type(move);

    std::cout << "moving " << piece_name << " from " << decode_from(move) << " to " << decode_to(move) << 
                " and taking " << piece_taken_name << " move_type: " << move_type << std::endl;
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
int Engine::stack_pop()
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
    int end = decode_to(move);
    int taken_piece_type = decode_piece(move);
    int color;

    unsigned long long curr_piece_loc = square_to_bitboard(start);
    unsigned long long taken_piece_loc = square_to_bitboard(end);

    color = get_color_by_bitboard(curr_piece_loc);

    int curr_piece_type = get_piece_by_bitboard(color, curr_piece_loc);
    remove_piece(color, curr_piece_type, curr_piece_loc);

    if(taken_piece_type)
    {
        // int taken_piece_type = get_piece_by_bitboard(1-color, taken_piece_loc);
        // std::cout << taken_piece_type << std::endl;
        remove_piece(1-color, taken_piece_type, taken_piece_loc);
    }

    place_piece(color, curr_piece_type, taken_piece_loc);
}

// Takes in a move, alters the BitboardEngine's representation to the PREVIOUS state based on the LAST move action
void Engine::pop_move()
{
    int move = stack_pop();

    int start = decode_from(move);
    int end = decode_to(move);
    int taken_piece_type = decode_piece(move);
    int color;

    unsigned long long curr_piece_loc = square_to_bitboard(end);
    unsigned long long orig_piece_loc = square_to_bitboard(start);

    color = get_color_by_bitboard(curr_piece_loc);

    int curr_piece_type = get_piece_by_bitboard(color, curr_piece_loc);
    remove_piece(color, curr_piece_type, curr_piece_loc);

    if(taken_piece_type)
    {
        // std::cout << taken_piece_type << std::endl;
        // exit(0);
        place_piece(1-color, taken_piece_type, curr_piece_loc);
    }

    place_piece(color, curr_piece_type, orig_piece_loc);
}

// Takes in a bitboard and will return the bitboard representing only the least significant bit.
// Example: the initial white_nights bitboard, the least significant 1 occurs at index 1 (...00001000010)
// therefore simply return ((lots of zeros)00000000000010)
// YOU MAY ASSUME A 1 EXISTS, (0000000000000000000) will not be given
// n &= (n - 1) to find val of leftmost maybe faster
int Engine::lsb_digit(unsigned long long board)
{
    return(lsb_lookup[lsb_board(board)]);
}

// Takes in a bitboard
// Returns a bitboard with soley the least significant bit = 1
// All other bits = 0
// Alters nothing
unsigned long long Engine::lsb_board(unsigned long long board)
{
    return(board & (-board));
}


// See above, except return the move_list significant bit bitboard
unsigned long long Engine::msb_board(unsigned long long board)
{
    unsigned long long sol = pow(2,msb_digit(board));
    return(sol);
}

// See above, except return the move_list significant bit bitboard
// Returns the index (right = 0 left = 63) corresponding to the most significant 1
// Probably a speed up to be had by checking if its above 32 bit
unsigned long long Engine::msb_digit(unsigned long long board)
{
    uint32_t first = board >> 32;
    uint32_t second = board & 0xFFFFFFFFLL;
    if(first == 0)
        return(63-(32 + __builtin_clz(second)));
    else
        return(63-__builtin_clz(first));
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

// https://stackoverflow.com/questions/25802605/what-does-performing-a-byteswap-mean
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
    x = ((x >> 1) & k1) + 2 * (x & k1);
    x = ((x >> 2) & k2) + 4 * (x & k2);
    x = ((x >> 4) & k4) + 16 * (x & k4);
    return x;
}


//REWRITE
unsigned long long Engine::vertical_flip(unsigned long long x)
{
    return __builtin_bswap64(x);
    return (x >> 56) |
          ((x<<40) & 0x00FF000000000000) |
          ((x<<24) & 0x0000FF0000000000) |
          ((x<<8) & 0x000000FF00000000) |
          ((x>>8) & 0x00000000FF000000) |
          ((x>>24) & 0x0000000000FF0000) |
          ((x>>40) & 0x000000000000FF00) |
          (x << 56);
    // return((((unsigned long long) (x) & (unsigned long long) 0x00000000000000ff) << 56) | \
    // (((unsigned long long) (x) & (unsigned long long) 0x000000000000ff00) << 40) | \
    // (((unsigned long long) (x) & (unsigned long long) 0x0000000000ff0000) << 24) | \
    // (((unsigned long long) (x) & (unsigned long long) 0x00000000ff000000) <<  8) | \
    // (((unsigned long long) (x) & (unsigned long long) 0x000000ff00000000) >>  8) | \
    // (((unsigned long long) (x) & (unsigned long long) 0x0000ff0000000000) >> 24) | \
    // (((unsigned long long) (x) & (unsigned long long) 0x00ff000000000000) >> 40) | \
    // (((unsigned long long) (x) & (unsigned long long) 0xff00000000000000) >> 56));
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

    int shifter;
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

void Engine::print_chess_char()
{
    char* b = (char*) malloc(8*8*sizeof(char));
    for(int i = 0; i<8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            b[j+i*8] = '-';
        }
    }

    unsigned long long p;
    unsigned long long one_p;

    p = pos.white_pawns;
    while(p)
    {
        one_p = lsb_board(p);
        p = p & ~one_p;
        b[get_file(one_p)+8*(7-get_rank(one_p))] = 'P';
    }

    p = pos.black_pawns;
    while(p)
    {
        one_p = lsb_board(p);
        p = p & ~one_p;
        b[get_file(one_p)+8*(7-get_rank(one_p))] = 'p';
    }

    p = pos.white_rooks;
    while(p)
    {
        one_p = lsb_board(p);
        p = p & ~one_p;
        b[get_file(one_p)+8*(7-get_rank(one_p))] = 'R';
    }

    p = pos.black_rooks;
    while(p)
    {
        one_p = lsb_board(p);
        p = p & ~one_p;
        b[get_file(one_p)+8*(7-get_rank(one_p))] = 'r';
    }

    p = pos.white_nights;
    while(p)
    {
        one_p = lsb_board(p);
        p = p & ~one_p;
        b[get_file(one_p)+8*(7-get_rank(one_p))] = 'N';
    }

    p = pos.black_nights;
    while(p)
    {
        one_p = lsb_board(p);
        p = p & ~one_p;
        b[get_file(one_p)+8*(7-get_rank(one_p))] = 'n';
    }

    p = pos.white_bishops;
    while(p)
    {
        one_p = lsb_board(p);
        p = p & ~one_p;
        b[get_file(one_p)+8*(7-get_rank(one_p))] = 'B';
    }

    p = pos.black_bishops;
    while(p)
    {
        one_p = lsb_board(p);
        p = p & ~one_p;
        b[get_file(one_p)+8*(7-get_rank(one_p))] = 'b';
    }

    p = pos.white_kings;
    while(p)
    {
        one_p = lsb_board(p);
        p = p & ~one_p;
        b[get_file(one_p)+8*(7-get_rank(one_p))] = 'K';
    }

    p = pos.black_kings;
    while(p)
    {
        one_p = lsb_board(p);
        p = p & ~one_p;
        b[get_file(one_p)+8*(7-get_rank(one_p))] = 'k';
    }

    p = pos.white_queens;
    while(p)
    {
        one_p = lsb_board(p);
        p = p & ~one_p;
        b[get_file(one_p)+8*(7-get_rank(one_p))] = 'Q';
    }

    p = pos.black_queens;
    while(p)
    {
        one_p = lsb_board(p);
        p = p & ~one_p;
        b[get_file(one_p)+8*(7-get_rank(one_p))] = 'q';
    }

    for(int i = 0; i<8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            std::cout << b[j+i*8];
        }
        std::cout << std::endl;
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

//check implementation
int Engine::bitboard_to_square(unsigned long long piece)
{
    return(lsb_digit(piece));
}

unsigned long long Engine::square_to_bitboard(int square)
{
    return(1ULL << square);
}

unsigned long long Engine::get_bitboard_of_piece(Piece piece, int color)
{
    if(color == 1)
    {
        if(piece == KING)
        {
            return pos.white_kings;
        }
        else
        {
            std::cout << "WARNING RETURNING GARBAGE IN GET_SQUARE";
            return(-1);
        }
    }
    // black
    else
    {
        if(piece == KING)
        {
            return pos.black_kings;
        }
        else
        {
            std::cout << "WARNING RETURNING GARBAGE IN GET_SQUARE";
            return(-1);
        }
    } 
    // if(color)
    // {

    // }
    // // black
    // else
    // {

    // } 
}

Piece Engine::get_piece_by_bitboard(int color, unsigned long long board)
{
    unsigned long long pawns, rooks, nights, bishops, queens, kings;
    if(color == 1)
    {
        pawns = pos.white_pawns;
        rooks = pos.white_rooks;
        nights = pos.white_nights;
        bishops = pos.white_bishops;
        queens = pos.white_queens;
        kings = pos.white_kings;
    }
    else
    {
        pawns = pos.black_pawns;
        rooks = pos.black_rooks;
        nights = pos.black_nights;
        bishops = pos.black_bishops;
        queens = pos.black_queens;
        kings = pos.black_kings;
    }

    if(board & pawns)
    {
        return(PAWN);
    }
    else if(board & rooks)
    {
        return(ROOK);
    }
    else if(board & nights)
    {
        return(NIGHT);
    }
    else if(board & bishops)
    {
        return(BISHOP);
    }
    else if(board & queens)
    {
        return(QUEEN);
    }
    else if(board & kings)
    {
        return(KING);
    }
    else
    {
        return(NONE);
    }
}

Piece Engine::get_piece_by_bitboard(unsigned long long board)
{
    return(get_piece_by_bitboard(get_color_by_bitboard(board), board));
}

void Engine::remove_piece(int color, int type, unsigned long long board)
{
    if(color == 1)
    {
        if(type == PAWN)
        {
            pos.white_pawns = pos.white_pawns - board;
        }
        else if(type == ROOK)
        {
            pos.white_rooks = pos.white_rooks - board;
        }
        else if(type == NIGHT)
        {
            pos.white_nights = pos.white_nights - board;
        }
        else if(type == BISHOP)
        {
            pos.white_bishops = pos.white_bishops - board;
        }
        else if(type == QUEEN)
        {
            pos.white_queens = pos.white_queens - board;
        }
        else
        {
            pos.white_kings = pos.white_kings - board;
        }
    }
    else
    {
        if(type == PAWN)
        {
            pos.black_pawns = pos.black_pawns - board;
        }
        else if(type == ROOK)
        {
            pos.black_rooks = pos.black_rooks - board;
        }
        else if(type == NIGHT)
        {
            pos.black_nights = pos.black_nights - board;
        }
        else if(type == BISHOP)
        {
            pos.black_bishops = pos.black_bishops - board;
        }
        else if(type == QUEEN)
        {
            pos.black_queens = pos.black_queens - board;
        }
        else
        {
            pos.black_kings = pos.black_kings - board;
        }
    }
}

void Engine::place_piece(int color, int type, unsigned long long board)
{
    if(color == 1)
    {
        if(type == PAWN)
        {
            pos.white_pawns = pos.white_pawns | board;
        }
        else if(type == ROOK)
        {
            pos.white_rooks = pos.white_rooks | board;
        }
        else if(type == NIGHT)
        {
            pos.white_nights = pos.white_nights | board;
        }
        else if(type == BISHOP)
        {
            pos.white_bishops = pos.white_bishops | board;
        }
        else if(type == QUEEN)
        {
            pos.white_queens = pos.white_queens | board;
        }
        else
        {
            pos.white_kings = pos.white_kings | board;
        }
    }
    else
    {
        if(type == PAWN)
        {
            pos.black_pawns = pos.black_pawns | board;
        }
        else if(type == ROOK)
        {
            pos.black_rooks = pos.black_rooks | board;
        }
        else if(type == NIGHT)
        {
            pos.black_nights = pos.black_nights | board;
        }
        else if(type == BISHOP)
        {
            pos.black_bishops = pos.black_bishops | board;
        }
        else if(type == QUEEN)
        {
            pos.black_queens = pos.black_queens | board;
        }
        else
        {
            pos.black_kings = pos.black_kings | board;
        }
    }
}

int Engine::get_color_by_bitboard(unsigned long long board)
{
    if(board & get_all_white())
    {
        return(1);
    }
    else
    {
        return(0);
    }
}

unsigned long long* Engine::get_board_rep()
{
    unsigned long long* rep = (unsigned long long*) malloc(12 * sizeof(unsigned long long));
    rep[0] = pos.white_pawns;
    rep[1] = pos.white_rooks;
    rep[2] = pos.white_nights;
    rep[3] = pos.white_bishops;
    rep[4] = pos.white_queens;
    rep[5] = pos.white_kings;

    rep[6] = pos.black_pawns;
    rep[7] = pos.black_rooks;
    rep[8] = pos.black_nights;
    rep[9] = pos.black_bishops;
    rep[10] = pos.black_queens;
    rep[11] = pos.black_kings;
    return rep;
}


// Some hueristics have been met, the only way to check if a move is legal or not, we must make it.
bool Engine::check_legal(int move, int color)
{
    // return false;
    std::cout << "checking legality of: " << move << std::endl;
    print_move_info(move);
    push_move(move);
    print_chess_char();
    if(get_in_check(color))
    {
        std::cout << "popping, NOT LEGAL: " << move << std::endl;
        // print_chess_char();
        pop_move();
        // print_chess_char();
        return(false);
    }
    std::cout << "popping, LEGAL: " << move << std::endl;
    // print_chess_char();
    pop_move();
    // print_chess_char();
    return(true);
}


// Returns a bitboard of pieces that are pinned against their king 
unsigned long long Engine::pinned_pieces(int color)
{
    unsigned long long defenders;
    unsigned long long enemy_rook;
    unsigned long long enemy_bishop;
    unsigned long long enemy_queen;
    // unsigned long long enemy_pieces;

    unsigned long long attacker_squares;

    // CHECK COLORS
    // Replace king with enemy queen
    // Find kings defenders
    // Declare enemy
    // white
    if(color == 1)
    {
        defenders = pre_check_queen_attacks(pos.white_kings) & get_all_white();
        enemy_rook = pos.black_rooks;
        enemy_bishop = pos.black_bishops;
        enemy_queen = pos.black_queens;
        // enemy_pieces = get_all_black();
    }
    // black
    else
    {
        defenders = pre_check_queen_attacks(pos.black_kings) & get_all_black();
        enemy_rook = pos.white_rooks;
        enemy_bishop = pos.white_bishops;
        enemy_queen = pos.white_queens;
        // enemy_pieces = get_all_white();
    }
    // Compile all squares under attack from enemy
    attacker_squares = pre_check_rook_attacks(enemy_rook) | 
                        pre_check_bishop_attacks(enemy_bishop) | 
                        pre_check_queen_attacks(enemy_queen);


    // Defenders in attacker squares are pinned pieces
    return(defenders & attacker_squares);
}


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

        move_list[move_list[0]+1] = encode_move(curr_pos, bitboard_to_square(new_pos), REGULAR, piece_taken, 0);
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
void Engine::generate_pre_check_moves(int color, int* move_list, unsigned long long pinned)
{
    unsigned long long p, one_p, all_occupied, own_occupied, temp, enemy_occupied;
    all_occupied = get_all();


    bool print = false;

    if(color == 1)
    {
        own_occupied = get_all_white();
        enemy_occupied = get_all_black();

        p = pos.white_rooks & ~pinned;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;

            if(print)
            {
                std::cout << "printing white rook boardrep" << std::endl;
                print_chess_rep(one_p);
                std::cout << "printing white rook boardrep2" << std::endl;
                print_chess_rep(pre_check_one_rook_moves(one_p, all_occupied, own_occupied));
            }

            pop_and_add_regular_moves(color, move_list, pre_check_one_rook_moves(one_p, 
                all_occupied, own_occupied), bitboard_to_square(one_p));
        }


        p = pos.white_pawns & ~pinned;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;

            if(print)
            {
                std::cout << "printing white pawn boardrep" << std::endl;
                print_chess_rep(one_p);
                std::cout << "printing white pawn boardrep2" << std::endl;
                print_chess_rep(pre_check_white_pawn_moves(one_p, all_occupied, enemy_occupied));
            }

            pop_and_add_regular_moves(color, move_list, pre_check_white_pawn_moves(one_p, 
                all_occupied, enemy_occupied), bitboard_to_square(one_p));
        }

        p = pos.white_bishops & ~pinned;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;

            if(print)
            {
                std::cout << "printing white bishops boardrep" << std::endl;
                print_chess_rep(one_p);
                std::cout << "printing white bishops boardrep2" << std::endl;
                print_chess_rep(pre_check_one_bishop_moves(one_p, all_occupied, own_occupied));
                // exit(0);
            }

            pop_and_add_regular_moves(color, move_list, pre_check_one_bishop_moves(one_p, 
                all_occupied, own_occupied), bitboard_to_square(one_p));
        }

        p = pos.white_nights & ~pinned;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;

            if(print)
            {
                std::cout << "printing white night boardrep" << std::endl;
                print_chess_rep(one_p);
                std::cout << "printing white night boardrep2" << std::endl;
                print_chess_rep(pre_check_night_moves(one_p, own_occupied));
            }

            pop_and_add_regular_moves(color, move_list, pre_check_night_moves(one_p, 
                own_occupied), bitboard_to_square(one_p));
        }

        p = pos.white_queens & ~pinned;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;

            if(print)
            {
                std::cout << "printing white queen boardrep" << std::endl;
                print_chess_rep(one_p);
                std::cout << "printing white queen boardrep2" << std::endl;
                print_chess_rep(pre_check_one_queen_moves(one_p, all_occupied, own_occupied));
            }

            pop_and_add_regular_moves(color, move_list, pre_check_one_queen_moves(one_p, 
                all_occupied, own_occupied), bitboard_to_square(one_p));
        }

        p = pos.white_kings & ~pinned;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;

            if(print)
            {
                std::cout << "printing white king boardrep" << std::endl;
                print_chess_rep(one_p);
                std::cout << "printing white king boardrep2" << std::endl;
                print_chess_rep(pre_check_king_moves(one_p, own_occupied));
            }

            pop_and_add_regular_moves(color, move_list, pre_check_king_moves(one_p, 
                own_occupied), bitboard_to_square(one_p));
        }

    }
    else
    {
        own_occupied = get_all_black();
        enemy_occupied = get_all_white();

        p = pos.black_rooks & ~pinned;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;

            if(print)
            {
                std::cout << "printing black rook boardrep" << std::endl;
                print_chess_rep(one_p);
                std::cout << "printing black rook boardrep2" << std::endl;
                print_chess_rep(pre_check_one_rook_moves(one_p, all_occupied, own_occupied));
            }

            pop_and_add_regular_moves(color, move_list, pre_check_one_rook_moves(one_p, 
                all_occupied, own_occupied), bitboard_to_square(one_p));
        }


        p = pos.black_pawns & ~pinned;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;

            if(print)
            {
                std::cout << "printing black pawn boardrep" << std::endl;
                print_chess_rep(one_p);
                std::cout << "printing black pawn boardrep2" << std::endl;
                print_chess_rep(pre_check_black_pawn_moves(one_p, all_occupied, enemy_occupied));
            }

            pop_and_add_regular_moves(color, move_list, pre_check_black_pawn_moves(one_p, 
                all_occupied, enemy_occupied), bitboard_to_square(one_p));
        }

        p = pos.black_bishops & ~pinned;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;

            if(print)
            {
                std::cout << "printing black bishops boardrep" << std::endl;
                print_chess_rep(one_p);
                std::cout << "printing black bishops boardrep2" << std::endl;
                print_chess_rep(pre_check_one_bishop_moves(one_p, all_occupied, own_occupied));
            }

            pop_and_add_regular_moves(color, move_list, pre_check_one_bishop_moves(one_p, 
                all_occupied, own_occupied), bitboard_to_square(one_p));
        }

        p = pos.black_nights & ~pinned;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;

            if(print)
            {
                std::cout << "printing black night boardrep" << std::endl;
                print_chess_rep(one_p);
                std::cout << "printing black night boardrep2" << std::endl;
                print_chess_rep(pre_check_night_moves(one_p, own_occupied));
            }

            pop_and_add_regular_moves(color, move_list, pre_check_night_moves(one_p, 
                own_occupied), bitboard_to_square(one_p));
        }

        p = pos.black_queens & ~pinned;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;

            if(print)
            {
                std::cout << "printing black queen boardrep" << std::endl;
                print_chess_rep(one_p);
                std::cout << "printing black queen boardrep2" << std::endl;
                print_chess_rep(pre_check_one_queen_moves(one_p, all_occupied, own_occupied));
            }

            pop_and_add_regular_moves(color, move_list, pre_check_one_queen_moves(one_p, 
                all_occupied, own_occupied), bitboard_to_square(one_p));
        }

        p = pos.black_kings & ~pinned;
        while(p)
        {
            one_p = lsb_board(p);
            p = p & ~one_p;

            if(print)
            {
                std::cout << "printing black king boardrep" << std::endl;
                print_chess_rep(one_p);
                std::cout << "printing black king boardrep2" << std::endl;
                print_chess_rep(pre_check_king_moves(one_p, own_occupied));
            }

            pop_and_add_regular_moves(color, move_list, pre_check_king_moves(one_p, 
                own_occupied), bitboard_to_square(one_p));
        }
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


    if(!get_bitboard_of_piece(KING, color))
    {
        std::cout << "\n\nKING SQUARE IS " << king_square << "COLOR IS" << color << "\n\n";
        std::cout << "\n\nKING SQUARE BITBOARD IS" << "\n";
        print_chess_rep(get_bitboard_of_piece(KING, color));
        std::cout << "\n\n";
        print_chess_char();

        print_chess_rep(pos.white_kings);
        print_chess_rep(pos.black_kings);
        std::cout << (KING == KING) << "\n";
        exit(0);
    }

    // if(in_check)
    // {
    //     // generate<EVASIONS>(pos, moveList)         last_move_index returned
    // }
    // else
    // {
    //     // generate<NON_EVASIONS>(pos, moveList)     last_move_index returned
    // }

    generate_pre_check_moves(color, move_list, pinned);

    int move_iter = 0;
    bool check_status = get_in_check(color);
    while(move_iter < move_list[0])
    {
        int move = move_list[move_iter+1];
        // if((pinned || decode_from(move) == king_square || decode_type(move) == ENPASSANT) && ~(check_legal(move)))
        if((decode_from(move) == king_square || decode_type(move) == ENPASSANT || check_status) && !(check_legal(move, color)))
        {
            // std::cout << "removing move cause not legal: " << move << std::endl;
            move_list[move_iter+1] = move_list[move_list[0]];
            move_list[0]--;
            move_iter--;
        }
        move_iter++;
    }
    return(move_list);
}

// -1 for not over
// 0 for black winning
// 1 for white winning
// 2 for draw
int Engine::is_terminal(int color, int* move_list)
{
    if(move_list[0] == 0)
    {
        if(get_in_check(color))
        {
            return(1-color);
        }
        return(2);
    }
    return(-1);
}

bool Engine::get_in_check(int color)
{
    unsigned long long my_king;
    unsigned long long enemy_pawns, enemy_rooks, enemy_nights, enemy_bishops, enemy_queens, enemy_kings;
    unsigned long long pawn_attackers, rook_attackers, night_attackers, bishop_attackers, king_attackers;

    if(color == 1)
    {
        my_king = pos.white_kings;

        enemy_pawns = pos.black_pawns;
        enemy_rooks = pos.black_rooks;
        enemy_nights = pos.black_nights;
        enemy_bishops = pos.black_bishops;
        enemy_queens = pos.black_queens;
        enemy_kings = pos.black_kings;

        pawn_attackers = pre_check_white_pawn_attacks(my_king) & enemy_pawns;
    }
    else
    {
        my_king = pos.black_kings;

        enemy_pawns = pos.white_pawns;
        enemy_rooks = pos.white_rooks;
        enemy_nights = pos.white_nights;
        enemy_bishops = pos.white_bishops;
        enemy_queens = pos.white_queens;
        enemy_kings = pos.white_kings;

        pawn_attackers = pre_check_black_pawn_attacks(my_king) & enemy_pawns;
    }

    if(pawn_attackers)
    {
        return true;
    }

    rook_attackers = pre_check_one_rook_attacks(my_king) & (enemy_rooks | enemy_queens);
    if(rook_attackers)
    {
        return true;
    }

    night_attackers = pre_check_night_attacks(my_king) & enemy_nights;
    if(night_attackers)
    {
        return true;
    }

    bishop_attackers = pre_check_one_bishop_attacks(my_king) & (enemy_bishops | enemy_queens);
    if(bishop_attackers)
    {
        return true;
    }

    king_attackers = pre_check_king_attacks(my_king) & enemy_kings;
    if(king_attackers)
    {
        return true;
    }
    return false;
}



// PAWNS
unsigned long long Engine::pre_check_white_pawn_attacks(unsigned long long white_pawns)
{
    // left side, filter out left file
    unsigned long long pawn_right = (white_pawns & ~col_mask[0]) << 7;

    // right side, filter out right file
    unsigned long long pawn_left = (white_pawns & ~col_mask[7]) << 9;

    // or together pawn attacks
    return(pawn_right | pawn_left);
}

unsigned long long Engine::pre_check_black_pawn_attacks(unsigned long long black_pawns)
{
    // left side, filter out left file
    unsigned long long pawn_left = (black_pawns & ~col_mask[0]) >> 9;

    // right side, filter out right file
    unsigned long long pawn_right = (black_pawns & ~col_mask[7]) >> 7;

    // or together pawn attacks
    return(pawn_left | pawn_right);
}

unsigned long long Engine::pre_check_white_pawn_moves(unsigned long long white_pawns, unsigned long long all_pieces, unsigned long long all_black_pieces)
{
    // pushes all pawns forard one
    unsigned long long pawn_one = (white_pawns << 8) & ~all_pieces; 

    // get all pieces that may be able to move to rank 4 in a double push, masks with rank 2 pawns first
    unsigned long long pawn_two = ((pawn_one & row_mask[2]) << 8) & ~all_pieces; 

    // or together for total moves
    unsigned long long both_pawn = pawn_one | pawn_two;

    // attacks
    unsigned long long pawn_attacks = pre_check_white_pawn_attacks(white_pawns);

    // and together black pieces
    unsigned long long pawn_valid_attacks = pawn_attacks & all_black_pieces;

    // or together moves and attacks and return
    return(both_pawn | pawn_valid_attacks);
}

unsigned long long Engine::pre_check_black_pawn_moves(unsigned long long black_pawns, unsigned long long all_pieces, unsigned long long all_white_pieces)
{
    // pushes all pawns forard one
    unsigned long long pawn_one = (black_pawns >> 8) & ~all_pieces; 

    // get all pieces that may be able to move to rank 4 in a double push, masks with rank 2 pawns first
    unsigned long long pawn_two = ((pawn_one & row_mask[5]) >> 8) & ~all_pieces; 

    // or together for total moves
    unsigned long long both_pawn = pawn_one | pawn_two;

    // attacks
    unsigned long long pawn_attacks = pre_check_black_pawn_attacks(black_pawns);

    // and together black pieces
    unsigned long long pawn_valid_attacks = pawn_attacks & all_white_pieces;

    // or together moves and attacks and return
    return(both_pawn | pawn_valid_attacks);
}



//KINGS



unsigned long long Engine::pre_check_king_attacks(unsigned long long kings)
{
    unsigned long long king_mask_file_0 = kings & ~col_mask[0];
    unsigned long long king_mask_file_7 = kings & ~col_mask[7];

    unsigned long long spot_0 = king_mask_file_7 >> 7; // Southeast
    unsigned long long spot_1 = kings >> 8; // South
    unsigned long long spot_2 = king_mask_file_0 >> 9; // Southwest
    unsigned long long spot_3 = king_mask_file_0 >> 1; // West

    unsigned long long spot_4 = king_mask_file_0 << 7; // Northwest
    unsigned long long spot_5 = kings << 8; // North
    unsigned long long spot_6 = king_mask_file_7 << 9; // Northeast
    unsigned long long spot_7 = king_mask_file_7 << 1; // East

    return(spot_0 | spot_1 | spot_2 | spot_3 | spot_4 | spot_5 | spot_6 | spot_7);
}

// Takes in king_rep (bitboad representing that colors king location)
// Takes in same_occupied (bitboard representing all pieces of that color)
// Returns bitboard representing all possible pre_check moves that the king can make
unsigned long long Engine::pre_check_king_moves(unsigned long long kings, unsigned long long own_occupied)
{
    return(pre_check_king_attacks(kings) & ~own_occupied);
}

unsigned long long Engine::pre_check_king_moves(int color)
{
    if(color == 1)
    {
        return(pre_check_king_moves(pos.white_kings, get_all_white()));
    }
    else
    {
        return(pre_check_king_moves(pos.black_kings, get_all_black()));
    }
}


//NIGHTS


unsigned long long Engine::pre_check_night_attacks(unsigned long long nights)
{
    unsigned long long spot_1_clip = ~col_mask[0] & ~col_mask[1];
    unsigned long long spot_2_clip = ~col_mask[0];
    unsigned long long spot_3_clip = ~col_mask[7];
    unsigned long long spot_4_clip = ~col_mask[7] & ~col_mask[6];

    unsigned long long spot_5_clip = ~col_mask[7] & ~col_mask[6];
    unsigned long long spot_6_clip = ~col_mask[7];
    unsigned long long spot_7_clip = ~col_mask[0];
    unsigned long long spot_8_clip = ~col_mask[0] & ~col_mask[1];

    unsigned long long spot_1 = (nights & spot_1_clip) << 6;
    unsigned long long spot_2 = (nights & spot_2_clip) << 15;
    unsigned long long spot_3 = (nights & spot_3_clip) << 17;
    unsigned long long spot_4 = (nights & spot_4_clip) << 10;

    unsigned long long spot_5 = (nights & spot_5_clip) >> 6;
    unsigned long long spot_6 = (nights & spot_6_clip) >> 15;
    unsigned long long spot_7 = (nights & spot_7_clip) >> 17;
    unsigned long long spot_8 = (nights & spot_8_clip) >> 10;

    return(spot_1 | spot_2 | spot_3 | spot_4 | spot_5 | spot_6 | spot_7 | spot_8);
}


// Takes in night_rep (bitboad representing that colors night location)
// Takes in same_occupied (bitboard representing all pieces of that color)
// Returns bitboard representing all possible pre_check moves that that night can make
unsigned long long Engine::pre_check_night_moves(unsigned long long nights, unsigned long long own_occupied)
{
    return(pre_check_night_attacks(nights) & ~own_occupied);
}

unsigned long long Engine::pre_check_night_moves(int color)
{
    if(color == 1)
    {
        return(pre_check_night_moves(pos.white_nights, get_all_white()));
    }
    else
    {
        return(pre_check_night_moves(pos.black_nights, get_all_black()));
    }
}


/////BISHOPS//////


// bishops may be missing the ANTI diagonal: https://chessprogramming.wikispaces.com/Hyperbola+Quintessence
unsigned long long Engine::pre_check_one_bishop_attacks_ANTI(unsigned long long bishop)
{
    int* diags = get_diag(get_rank(bishop), get_file(bishop));
    unsigned long long line_mask = ~bishop & diag_right_mask[diags[1]]; // excludes square of slider
    free(diags);

    unsigned long long forward = get_all() & line_mask; // also performs the first subtraction by clearing the s in o
    unsigned long long reverse = vertical_flip(forward); // o'-s'

    forward = forward - bishop; // o -2s
    reverse = reverse - vertical_flip(bishop); // o'-2s'
    forward = forward ^ vertical_flip(reverse);
    return forward & line_mask;      // mask the line again
}


unsigned long long Engine::pre_check_one_bishop_attacks(unsigned long long bishop)
{
    int* diags = get_diag(get_rank(bishop), get_file(bishop));
    unsigned long long line_mask = ~bishop & diag_left_mask[diags[0]]; // excludes square of slider
    free(diags);

    // std::cout << "line_mask" << std::endl;
    // print_chess_rep(line_mask);

    // std::cout << "get_all()" << std::endl;
    // print_chess_rep(get_all());

    unsigned long long forward = get_all() & line_mask; // also performs the first subtraction by clearing the s in o
    unsigned long long reverse = vertical_flip(forward); // o'-s'

    // std::cout << "forward" << std::endl;
    // print_chess_rep(forward);


    // std::cout << "reverse" << std::endl;
    // print_chess_rep(reverse);

    forward = forward - bishop; // o -2s

    // std::cout << "o - 2s" << std::endl;
    // print_chess_rep(forward);

    reverse = reverse - vertical_flip(bishop); // o'-2s'

    // std::cout << "o' - 2s'" << std::endl;
    // print_chess_rep(reverse);

    forward = forward ^ vertical_flip(reverse);

    // std::cout << "forward ^ '(o'-2s')" << std::endl;
    // print_chess_rep(forward);

    return pre_check_one_bishop_attacks_ANTI(bishop) | (forward & line_mask);      // mask the line again
}

unsigned long long Engine::pre_check_bishop_attacks(unsigned long long bishops)
{
    unsigned long long one_bishop;
    unsigned long long bishop_attacks = 0;
    while(bishops)
    {
        one_bishop = lsb_board(bishops);
        bishops = bishops & ~one_bishop;
        bishop_attacks = bishop_attacks | pre_check_one_bishop_attacks(one_bishop);
    }
    return bishop_attacks;
}

unsigned long long Engine::pre_check_one_bishop_moves(unsigned long long bishop, unsigned long long all_occupied, unsigned long long own_occupied)
{
    return pre_check_one_bishop_attacks(bishop) & ~own_occupied;
}

unsigned long long Engine::pre_check_bishop_moves(unsigned long long bishops, unsigned long long all_occupied, unsigned long long own_occupied)
{
    unsigned long long one_bishop;
    unsigned long long bishop_moves = 0;
    while(bishops)
    {
        one_bishop = lsb_board(bishops);
        bishops = bishops & ~one_bishop;
        bishop_moves = bishop_moves | pre_check_one_bishop_moves(one_bishop, all_occupied, own_occupied);
    }
    return bishop_moves;
}

unsigned long long Engine::pre_check_bishop_moves(int color)
{
    if(color == 1)
    {
        return pre_check_bishop_moves(pos.white_bishops, color);
    }
    else
    {
        return pre_check_bishop_moves(pos.black_bishops, color);
    }
}

unsigned long long Engine::pre_check_bishop_moves(unsigned long long piece, int color)
{
    if(color == 1)
    {
        return(pre_check_bishop_moves(piece, get_all(), get_all_white()));
    }
    else
    {
        return(pre_check_bishop_moves(piece, get_all(), get_all_black()));
    }
}



/////ROOKS//////

// check args
unsigned long long Engine::pre_check_one_rook_attacks(unsigned long long rook)
{
    unsigned long long row = get_rank(rook);
    unsigned long long col = get_file(rook);

    unsigned long long s = rook;
    unsigned long long o = get_all();

    unsigned long long o_rev = reverse_64_bits(o);
    unsigned long long s_rev = reverse_64_bits(s);

    unsigned long long hori = (o - 2*s) ^ reverse_64_bits(o_rev - 2*s_rev);
    hori = hori & row_mask[row];


    unsigned long long o_mask = o & col_mask[col];
    unsigned long long o_rev_mask = reverse_64_bits(o_mask);
    unsigned long long vert = (o_mask - 2*s) ^ reverse_64_bits(o_rev_mask - 2*s_rev);
    vert = vert & col_mask[col];

    return(hori | vert);
}

unsigned long long Engine::pre_check_rook_attacks(unsigned long long rooks)
{
    unsigned long long one_rook;
    unsigned long long rook_attacks = 0;
    while(rooks)
    {
        one_rook = lsb_board(rooks);
        rooks = rooks & ~one_rook;
        rook_attacks = rook_attacks | pre_check_one_rook_attacks(one_rook);
    }
    return rook_attacks;
}

unsigned long long Engine::pre_check_one_rook_moves(unsigned long long rook, unsigned long long all_occupied, unsigned long long own_occupied)
{
    return pre_check_one_rook_attacks(rook) & ~own_occupied;
}

unsigned long long Engine::pre_check_rook_moves(unsigned long long rooks, unsigned long long all_occupied, unsigned long long own_occupied)
{
    unsigned long long one_rook;
    unsigned long long rook_moves = 0;
    while(rooks)
    {
        one_rook = lsb_board(rooks);
        rooks = rooks & ~one_rook;
        rook_moves = rook_moves | pre_check_one_rook_moves(one_rook, all_occupied, own_occupied);
    }
    return rook_moves;
}

unsigned long long Engine::pre_check_rook_moves(int color)
{
    if(color == 1)
    {
        return pre_check_rook_moves(pos.white_rooks, color);
    }
    else
    {
        return pre_check_rook_moves(pos.black_rooks, color);
    }
}

unsigned long long Engine::pre_check_rook_moves(unsigned long long piece, int color)
{
    if(color == 1)
    {
        return(pre_check_rook_moves(piece, get_all(), get_all_white()));
    }
    else
    {
        return(pre_check_rook_moves(piece, get_all(), get_all_black()));
    }
}



/////QUEENS//////


unsigned long long Engine::pre_check_one_queen_attacks(unsigned long long queen)
{
    return pre_check_one_bishop_attacks(queen) |
            pre_check_one_rook_attacks(queen);
}

unsigned long long Engine::pre_check_queen_attacks(unsigned long long queens)
{
    unsigned long long one_queen;
    unsigned long long queen_attacks = 0;
    while(queens)
    {
        one_queen = lsb_board(queens);
        queens = queens & ~one_queen;
        queen_attacks = queen_attacks | pre_check_one_queen_attacks(one_queen);
    }
    return queen_attacks;
}

unsigned long long Engine::pre_check_one_queen_moves(unsigned long long queen, unsigned long long all_occupied, unsigned long long own_occupied)
{
    return pre_check_one_queen_attacks(queen) & ~own_occupied;
}

unsigned long long Engine::pre_check_queen_moves(unsigned long long queens, unsigned long long all_occupied, unsigned long long own_occupied)
{
    unsigned long long one_queen;
    unsigned long long queen_moves = 0;
    while(queens)
    {
        one_queen = lsb_board(queens);
        queens = queens & ~one_queen;
        queen_moves = queen_moves | pre_check_one_queen_moves(one_queen, all_occupied, own_occupied);
    }
    return queen_moves;
}


unsigned long long Engine::pre_check_queen_moves(int color)
{
    if(color == 1)
    {
        return pre_check_queen_moves(pos.white_queens, color);
    }
    else
    {
        return pre_check_queen_moves(pos.black_queens, color);
    }
}

unsigned long long Engine::pre_check_queen_moves(unsigned long long piece, int color)
{
    if(color == 1)
    {
        return pre_check_bishop_moves(piece, get_all(), get_all_white()) | 
            pre_check_rook_moves(piece, get_all(), get_all_white());
    }
    else
    {
        return pre_check_bishop_moves(piece, get_all(), get_all_black()) | 
            pre_check_rook_moves(piece, get_all(), get_all_black());
    }
}