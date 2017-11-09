#include <iostream>


enum Piece(Enum)
{   NONE,
    PAWN,
    ROOK,
    NIGHT,
    BISHOP,
    QUEEN,
    KING
};

enum MoveType(Enum)
{
    REGULAR,
    CASTLE,
    ENPASSANT,
    PROMOTION
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
        void Engine();
        void Engine(unsigned long long *board_data);

    private: 
        int max_move_length;
        int stack_index;
        unsigned long long move_stack[]; 
        bool in_check;
        position pos;

        unsigned long long *row_mask;
        unsigned long long *col_mask;
        unsigned long long *diag_left_mask;
        unsigned long long *diag_right_mask;

        void init_engine();
        void init_position();
        void init_position(unsigned long long *board_data);
        void init_masks();
        void make_col_mask(unsigned long long mask);
        void fill_col_mask_arr();
        void make_row_mask(unsigned long long mask);
        void fill_row_mask_arr();
}

void Engine::Engine()
{
    init_position();
    init_engine();
}


void Enigne::Engine(unsigned long long *board_data)
{
    init_position(board_data);
    init_engine();
}


void Enigne::init_engine()
{
    max_move_length = 500; // This assumes there are only 500 possible legal moves at any one time (affects move array intilization)
    
    move_stack[max_move_length] = {0};
    int stack_index = -1;

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


void Engine::make_row_mask(unsigned long long mask)
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
        mask = mask | ((mask & BR_mask) >> np.uint64(7));
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
    unsigned long long TR_mask = ~((self.row_mask[7]) | (self.col_mask[7]));
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
        self.diag_right_mask[j] = make_diag_right_mask(start);
        start = start << 8;
    }          
}

int Engine::get_max_move_length()
{
    return(self.max_move_length)
}


unsigned long long get_all_white()
{
    all_white = white_pawns | white_rooks | white_nights | white_bishops | white_kings | white_queens;
    return(all_white);
}


unsigned long long get_all_black()
{
    all_black = black_pawns | black_rooks | black_nights | black_bishops | black_kings | black_queens;
    return(all_black);
}


unsigned long long get_all(self)
{
    white = self.get_all_white();
    black = self.get_all_black();
    all_pieces = white | black;
    return(all_pieces);
}


// Takes in a 64 bit number with single bit
// Returns the rank piece is on 0-7, bottom to top
// Alters nothing
int get_rank(self,num)
{
    unsigned long long max0 = 128; // 2^7
    if(num <= max0)
    {
        return(0);
    }

    unsigned long long max1 = 32768; // 2^15
    if(num <= max1)
    {
        return(1);
    }

    unsigned long long max2 = 8388608; // 2^23
    if(num <= max2)
    {
        return(2);
    }

    unsigned long long max3 = 2147483648; // 2^31
    if(num <= max3)
    {
        return(3);
    }

    unsigned long long max4 = 549755813888; // 2^39
    if(num <= max4)
    {
        return(4);
    }

    unsigned long long max5 = 140737488355328; // 2^47
    if(num <= max5)
    {
        return(5);
    }

    unsigned long long max6 = 36028797018963968; // 2^55
    if(num <= max6)
    {
        return(6);
    }

    unsigned long long max7 = 9223372036854775808; // 2^63
    if(num <= max7)
    {
        return(7);
    }
}


// Takes in a 64 bit number with single bit
// Returns the file piece is on 0-7, left to right
// Alters nothing 
int get_file(self,num)
{
    switch(num)
    {
        //2^[0, 8, 16, 24, 32, 40, 48, 56]
        case 1:
        case 256:
        case 65536:
        case 16777216:
        case 4294967296:
        case 1099511627776:
        case 281474976710656:
        case 72057594037927936:
            return(0);

        //2^[1,9,17,25,33,41,49,57]
        case 2:
        case 512:
        case 131072:
        case 33554432:
        case 8589934592:
        case 2199023255552:
        case 562949953421312:
        case 144115188075855872:
            return(1);

        // 2^[2,10,18,26,34,42,50,58]
        case 4:
        case 1024:
        case 262144:
        case 67108864:
        case 17179869184:
        case 4398046511104:
        case 1125899906842624:
        case 288230376151711744:
            return(2);

        // 2^[3,11,19,27,35,43,51,59]    
        case 8:
        case 2048:
        case 524288:
        case 134217728:
        case 34359738368:
        case 8796093022208:
        case 2251799813685248:
        case 576460752303423488:
            return(3);

        // 2^[4,12,20,28,36,44,52,60]
        case 16:
        case 4096:
        case 1048576:
        case 268435456:
        case 68719476736:
        case 17592186044416:
        case 4503599627370496:
        case 1152921504606846976:
            return(4);

        // 2^[5,13,21,29,37,45,53,61]
        case 32:
        case 8192:
        case 2097152:
        case 536870912:
        case 137438953472:
        case 35184372088832:
        case 9007199254740992:
        case 2305843009213693952:
            return(5);

        // 2^[6, 14, 22, 30, 38, 46, 54, 62]
        case 64:
        case 16384:
        case 4194304:
        case 1073741824:
        case 274877906944:
        case 70368744177664:
        case 18014398509481984:
        case 4611686018427387904:
            return(6);

        // 2^[7,15,23,31,39,47,55,63]
        case 128:
        case 32768:
        case 8388608:
        case 2147483648:
        case 549755813888:
        case 140737488355328:
        case 36028797018963968:
        case 9223372036854775808:
            return(7);

        default:
            return(-1);
    }
}

// Takes in a rank, and file
// Returns the left and right diagonal mask indexes as [left,right]
// Alters nothing 

int get_diag(self, rank, file)
{
    int total_val = rank+file;
    //Total val also equals left diag index

    if(rank > file) //Above the middle diagonal line r = 7
    {
        int right =7+(total_val-2*file);
    }
    else //Below middle line
    {
        int right = 7-(total_val-2*rank);
    }

    int diag[2] = {total_val,right};

    return(diag);
}


Takes in move information
    start 
        {

        } int 0-63 
{

} Square moved piece started on
//     end 
{

} int 0-63 
{

} Square moved piece ended on
//     m_type
{

} int 0-3 
{

} Type of move made
//     piece
{

} int 0-4 
{

} Piece taken in move
//     promotion
{

} int 2-5 
{

} Piece to promote pawn to
// Return a np.uint32 representing all above info
// Alters nothing
void encode_move(self, start, end, m_type, piece, promotion)
{

}
    encode_start = np.uint8(start)
    encode_end = np.uint16(end) << np.uint8(6)
    encode_type = np.uint32(m_type) << np.uint8(12)
    encode_piece = np.uint32(piece) << np.uint8(14)
    encode_promotion = np.uint32(promotion) << np.uint(17)
    return(encode_start & encode_end & encode_type & encode_piece & encode_promotion)

    
// Takes in a np.uint32 move
// Returns square number moved piece originated from
// Alters nothing
void decode_from(self,move)
{

}
    return(move & np.uint8(63))


// Takes in a np.uint32 move
// Returns square number moved piece travels to
// Alters nothing
void decode_to(self,move)
{

}
    return((move >> np.uint8(6)) & np.uint8(63))


// Takes in a np.uint32 move
// Returns type of move made
// Alters nothing
void decode_type(self,move)
{

}
    return((move >> np.uint8(12)) & np.uint8(3)) 


// Takes in a np.uint32 move
// Returns any piece taken by move
// Alters nothing
void decode_piece(self,move)
{

}
    return((move >> np.uint8(14)) & np.uint8(7)) 

// Takes in a np.uint32 move
// Returns new piece pawn promoted to
// Alters nothing
void decode_promo(self,move)
{

}
    return((move >> np.uint8(17)) & np.uint8(3))

 // Takes in a move to be added to the move stack
 // Returns nothing
 // Alters the move stack and stack_index value
void stack_push(self, move)
{
    // get pointer to stack index
    // get pointer to move_stack
    move_stack[++(*stack_index)] = move;
}

 // Takes in nothing
 // Returns the last move in the move stack
 // Alters the stack_index value
unsigned long long stack_pop(self)
{
    // get pointer to stack index
    // get pointer to move_stack
    return(move_stack[(*stack_index)--]);
}



// Takes in a bitboard and will return the bitboard representing only the least significant bit.
// Example
    {

    } the initial white_nights bitboard, the least significant 1 occurs at index 1 (...00001000010)
// therefore simply return ((lots of zeros)00000000000010)
// YOU MAY ASSUME A 1 EXISTS, (0000000000000000000) will not be given
void lsb_digit(self)
{

}
    return((num & -num).bit_length()-1)

// Takes in a bitboard
// Returns a bitboard with soley the least significant bit = 1
// All other bits = 0
// Alters nothing
void lsb_board(self)
{

}
    return(num & -num)


// See above, except return the move_list significant bit bitboard
void msb(self)
{

}
    pass


// Reverses a uint8 number, like this (00110000 -> 00001100)
// To improve, possibly just not(11111111 - num)??? 
void reverse_8_bit(self, row)
{

}
    num = np.uint8(row)
    reverse_num = np.uint8(row)
    one_8 = np.uint8(1)
    count = np.uint8(7);
     
    num = num >> one_8
    while(num)
    {

    }
        reverse_num = reverse_num << one_8    
        reverse_num = reverse_num | (num & one_8)
        num = num >> one_8
        count -= one_8
    reverse_num = reverse_num << count
    return reverse_num
    // return ~(np.uint8(255) - np.uint8(row))


void print_chess_rep(self, num)
{

}
    for i in range(7, -1, -1)
    {

    }
        shifter = np.uint64(8 * i)
        row = (num & self.row_mask[i]) >> shifter
        rev = self.reverse_8_bit(row)
        print('{0
            {

            }08b}'.format(rev))


void print_chess_rep_byteswap(self, num)
{

}
    for i in range(7, -1, -1)
    {

    }
        shifter = np.uint64(8 * i)
        row = (num & self.row_mask[i]) >> shifter
        rev = (row.byteswap() >> np.uint64(56))
        print('{0
            {

            }08b}'.format(rev))


void reverse_8_bits(self, x)
{

}
    return (x * np.uint64(0x0202020202) & np.uint64(0x010884422010)) % np.uint64(1023)


void reverse_64_bits(self, x)
{

}
    return self.vertical_flip(self.horizontal_flip(x))
    // return (x * np.uint64(0x0202020202) & np.uint64(0x010884422010)) % np.uint64(1023);


void horizontal_flip(self, x)
{

}
    k1 = np.uint64(0x5555555555555555)
    k2 = np.uint64(0x3333333333333333)
    k4 = np.uint64(0x0f0f0f0f0f0f0f0f)
    x = ((x >> np.uint64(1)) & k1) + np.uint64(2) * (x & k1);
    x = ((x >> np.uint64(2)) & k2) + np.uint64(4) * (x & k2);
    x = ((x >> np.uint64(4)) & k4) + np.uint64(16) * (x & k4);
    return x;


void vertical_flip(self, x)
{

}
    return x.byteswap()


// East
    {

    }      << 1
// Southeast
{

} >> 7
// South
{

}     >> 8
// Southwest
{

} >> 9
// West
{

}      >> 1
// Northwest
{

} << 7
// North
{

}     << 8
// Northeast
{

} << 9


// Takes in a move, alters the BitboardEngine's representation to the NEXT state based on the CURRENT move action
void push_move(self, move)
{

}
    pass


// Takes in a move, alters the BitboardEngine's representation to the PREVIOUS state based on the LAST move action
void push_move(self, move)
{

}
    pass


void get_square(self, piece, color)
{

}
    if color
    {

    } // white
        if piece == Piece.KING
        {

        }
            return self.white_kings
        else
        {

        }
            pass
    else
    {

    } // black
        if piece == Piece.KING
        {

        }
            return self.black_kings
        else
        {

        }
            pass


// Some hueristics have been met, the only way to check if a move is legal or not, we must make it.
void check_legal(self, move)
{

}
    pass


// Returns a bitboard of pieces that are pinned against their king 
void pinned_pieces(self, color)
{

}
    #Replace king with enemy queen
    #Find kings defenders
    #Declare enemy
    if color
    {

    } // white
        defenders = self.queen_attacks(self.white_kings,0)
        enemy_rook = self.black_rooks
        enemy_bishop = self.black_bishops
        enemy_queen = self.black_queens
        enemy_color = 0
    else
    {

    } // black
        defenders = self.queen_attacks(self.black_kings,1)
        enemy_rook = self.white_rooks
        enemy_bishop = self.white_bishops
        enemy_queen = self.white_queens
        enemy_color = 1

    #Compile all squares under attack from enemy
    attacker_squares = self.rook_attacks(enemy_rook,enemy_color) | self.bishop_attacks(enemy_bishop,enemy_color) | self.queen_attacks(enemy_queen,enemy_color)
    #Defenders in attacker squares are pinned pieces
    return(defenders & attacker_squares)


// Generates and fills move_list for a color before checking checks
void generate_pre_check_moves(self, color, move_list)
{

}
    king_loc = self.pre_check_king_bitboard()
    return all_pre_check_moves


// Generates and returns a list of legal moves for a color
void generate_legal_moves(self, color)
{

}
    all_legal_moves = np.zeros((self.max_move_length,), dtype='uint32')
    last_move_index = 0

    pinned = self.pinned_pieces(color);
    king_square = self.get_square(Piece.KING, color)

    if self.in_check
    {

    }
        pass
        // generate<EVASIONS>(pos, moveList)         last_move_index returned
    else
    {

    }
        pass
        // generate<NON_EVASIONS>(pos, moveList)     last_move_index returned

    move_iter = 0
    while move_iter < last_move_index
    {

    }
        move = all_legal_moves[move_iter]
        if (pinned or self.decode_from(move) == king_square or self.decode_type(move) == MoveType.ENPASSANT) and not self.check_legal(move)
        {

        }
            last_move_index -= 1
            all_legal_moves[move_iter] = all_legal_moves[last_move_index]

    return all_legal_moves


void pop_moves(self, moves, move_board, curr_pos, t, piece, promo)
{

}
    while(board)
    {

    }
        move = self.lsb_board(board)
        self.encode_move(self.lsb_digit(curr_pos), self.lsb_digit(move_board), t, piece, promo)
        board = board & (~move)


void one_rook_attack(self, board, color)
{

}
    pass
    // row = np.uint64(2)
    // col = np.uint64(6)

    // s = board
    // o = self.get_all()

    // // white
    // if color == 1
    {

    }
    //     own = self.get_all_white()
    // // black
    // else
    {

    }
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


// void rook_attacks(self, board, color)
    {

    }
//     new = np.uint64(0)
//     while board
{

}
//         s = self.lsb_board(board)
//         p = (o - s - s) ^ self.reverse_64_bits((~o) - (~s) - (~s))
//         new = new | p
//         board = board - s
//     return new


void one_bishop_attack(self, board, color)
{

}
    row = self.get_rank(board)
    col = self.get_file(board)
    line1_mask = self.get_diag(row, col, 0)
    line2_mask = self.get_diag(row, col, 1)

    s = board
    o = self.get_all()

    // white
    if color == 1
    {

    }
        own = self.get_all_white()
    // black
    else
    {

    }
        own = self.get_all_black()

    forward = o & self.diag_left_mask[line1_mask]
    reverse = self.reverse_64_bits(forward)
    forward = forward - s
    reverse = reverse - self.reverse_64_bits(s)
    forward = forward ^ self.reverse_64_bits(reverse)

    return forward & self.diag_left_mask[line1_mask] & ~own

    // lineMask = diagonalMaskEx[sqOfSlider]; // excludes square of slider
    // slider   = singleBitboard[sqOfSlider]; // single bit 1 << sq, 2^sq

    // forward  = occ & lineMask; // also performs the first subtraction by clearing the s in o
    // reverse  = byteswap( forward ); // o'-s'
    // forward -=         ( slider  ); // o -2s
    // reverse -= byteswap( slider  ); // o'-2s'
    // forward ^= byteswap( reverse );
    // return forward & lineMask;      // mask the line again


void bishop_attacks(self, board, color)
{

}
    pass


void queen_attacks(self, board, color)
{

}
    return(self.rook_attacks(board,color) | self.bishop_attacks(board,color))


// Takes in king_rep (bitboad representing that colors king location)
// Takes in same_occupied (bitboard representing all pieces of that color)
// Returns bitboard representing all possible pre_check moves that the king can make
void pre_check_king_bitboard(self, king_rep, same_occupied)
{

}
    king_mask_file_0 = king_rep & ~self.col_mask[0]
    king_mask_file_7 = king_rep & ~self.col_mask[7] 

    spot_0 = king_mask_file_7 >> np.uint64(7) // Southeast
    spot_1 = king_rep >> np.uint64(8) // South
    spot_2 = king_mask_file_7 >> np.uint64(9) // Southwest
    spot_3 = king_mask_file_7 >> np.uint64(1) // West

    spot_4 = king_mask_file_0 << np.uint64(7) // Northwest
    spot_5 = king_rep << np.uint64(8) // North
    spot_6 = king_mask_file_0 << np.uint64(9) // Northeast
    spot_7 = king_rep << np.uint64(1) // East

    king_moves = spot_0 | spot_1 | spot_2 | spot_3 | spot_4 | spot_5 | spot_6 | spot_7 

    return king_moves & ~same_occupied;


void get_king_moves(self, color)
{

}
    if color == 1
    {

    }
        return self.pre_check_king_bitboard(self.white_kings, self.get_all_white())
    else
    {

    }
        return self.pre_check_king_bitboard(self.black_kings, self.get_all_black())


// Takes in night_rep (bitboad representing that colors night location)
// Takes in same_occupied (bitboard representing all pieces of that color)
// Returns bitboard representing all possible pre_check moves that that night can make
void pre_check_night(self, king_rep, same_occupied)
{

}
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
