#include "bitboard.hpp"


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
unsigned long long Engine::pre_check_one_bishop_attacks_ANTI(unsigned long long bishop, int right_diag)
{
    // int* diags = get_diag(get_rank(bishop), get_file(bishop));
    // int right_diag = get_diag(get_rank(bishop), get_file(bishop)) >> 5;
    unsigned long long line_mask = ~bishop & diag_right_mask[right_diag]; // excludes square of slider
    // free(diags);

    unsigned long long forward = get_all() & line_mask; // also performs the first subtraction by clearing the s in o
    unsigned long long reverse = vertical_flip(forward); // o'-s'

    forward = forward - bishop; // o -2s
    reverse = reverse - vertical_flip(bishop); // o'-2s'
    forward = forward ^ vertical_flip(reverse);
    return forward & line_mask;      // mask the line again
}


unsigned long long Engine::pre_check_one_bishop_attacks(unsigned long long bishop)
{
    // int* diags = get_diag(get_rank(bishop), get_file(bishop));
    int diag = get_diag(get_rank(bishop), get_file(bishop));
    int left_diag = diag >> 5;
    int right_diag = diag & 0x000000000000000F;

    unsigned long long line_mask = ~bishop & diag_left_mask[left_diag]; // excludes square of slider
    // free(diags);

    unsigned long long forward = get_all() & line_mask; // also performs the first subtraction by clearing the s in o
    unsigned long long reverse = vertical_flip(forward); // o'-s'

    forward = forward - bishop; // o -2s
    reverse = reverse - vertical_flip(bishop); // o'-2s'
    forward = forward ^ vertical_flip(reverse);

    return pre_check_one_bishop_attacks_ANTI(bishop, right_diag) | (forward & line_mask);      // mask the line again
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

unsigned long long Engine::pre_check_bishop_moves(unsigned long long bishop, int color)
{
    if(color == 1)
    {
        return(pre_check_bishop_moves(bishop, get_all(), get_all_white()));
    }
    else
    {
        return(pre_check_bishop_moves(bishop, get_all(), get_all_black()));
    }
}



/////ROOKS//////

// check args
// unsigned long long Engine::pre_check_one_rook_attacks(unsigned long long rook)
// {
//     unsigned long long row = get_rank(rook);
//     unsigned long long col = get_file(rook);

//     unsigned long long s = rook;
//     unsigned long long o = get_all();

//     unsigned long long o_rev = reverse_64_bits(o);
//     unsigned long long s_rev = reverse_64_bits(s);

//     unsigned long long hori = (o - 2*s) ^ reverse_64_bits(o_rev - 2*s_rev);
//     hori = hori & row_mask[row];


//     unsigned long long o_mask = o & col_mask[col];
//     unsigned long long o_rev_mask = reverse_64_bits(o_mask);
//     unsigned long long vert = (o_mask - 2*s) ^ reverse_64_bits(o_rev_mask - 2*s_rev);
//     vert = vert & col_mask[col];

//     return(hori | vert);
// }

unsigned long long Engine::pre_check_one_rook_attacks(unsigned long long rook)
{
    unsigned long long row = get_rank(rook);
    unsigned long long col = get_file(rook);

    unsigned long long s = rook;
    unsigned long long o = get_all();

    unsigned long long o_rev = vertical_flip(o);
    unsigned long long s_rev = vertical_flip(s);

    unsigned long long hori = (o - 2*s) ^ vertical_flip(o_rev - 2*s_rev);
    hori = hori & row_mask[row];


    unsigned long long o_mask = o & col_mask[col];
    unsigned long long o_rev_mask = vertical_flip(o_mask);
    unsigned long long vert = (o_mask - 2*s) ^ vertical_flip(o_rev_mask - 2*s_rev);
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

unsigned long long Engine::pre_check_rook_moves(unsigned long long rook, int color)
{
    if(color == 1)
    {
        return(pre_check_rook_moves(rook, get_all(), get_all_white()));
    }
    else
    {
        return(pre_check_rook_moves(rook, get_all(), get_all_black()));
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

unsigned long long Engine::pre_check_queen_moves(unsigned long long queen, int color)
{
    if(color == 1)
    {
        return pre_check_bishop_moves(queen, get_all(), get_all_white()) | 
            pre_check_rook_moves(queen, get_all(), get_all_white());
    }
    else
    {
        return pre_check_bishop_moves(queen, get_all(), get_all_black()) | 
            pre_check_rook_moves(queen, get_all(), get_all_black());
    }
}




// FLOODS

unsigned long long Engine::vert_flood(unsigned long long rooks)
{
    return(north_flood(rooks, ~get_all()) | south_flood(rooks, ~get_all()));
}

unsigned long long Engine::hori_flood(unsigned long long rooks)
{
    return(east_flood(rooks, ~get_all() & ~col_mask[7]) | west_flood(rooks, ~get_all() & ~col_mask[0]));
}


unsigned long long Engine::north_flood(unsigned long long rooks, unsigned long long prop)
{
    unsigned long long north_flood = 0;

    while(rooks) 
    {
        north_flood = north_flood | rooks;
        rooks = (rooks << 8) & prop;
    }
    return(north_flood << 8);
}

unsigned long long Engine::south_flood(unsigned long long rooks, unsigned long long prop)
{
    unsigned long long south_flood = 0;

    while(rooks) 
    {
        south_flood = south_flood | rooks;
        rooks = (rooks >> 8) & prop;
    }
    return(south_flood >> 8);
}


unsigned long long Engine::east_flood(unsigned long long rooks, unsigned long long prop)
{
    unsigned long long east_flood = 0;
    rooks = rooks & ~col_mask[7];

    while(rooks) 
    {
        east_flood = east_flood | rooks;
        rooks = (rooks << 1) & prop;
    }
    return((east_flood & ~col_mask[7]) << 1);
}

unsigned long long Engine::west_flood(unsigned long long rooks, unsigned long long prop)
{
    unsigned long long west_flood = 0;
    rooks = rooks & ~col_mask[0];

    while(rooks) 
    {
        west_flood = west_flood | rooks;
        rooks = (rooks >> 1) & prop;
    }
    return((west_flood & ~col_mask[0]) >> 1);
}


unsigned long long Engine::left_diag_flood(unsigned long long bishops)
{
    return(north_west_flood(bishops, ~get_all() & ~col_mask[0]) | south_east_flood(bishops, ~get_all() & ~col_mask[7]));
}

unsigned long long Engine::right_diag_flood(unsigned long long bishops)
{
    return(north_east_flood(bishops, ~get_all() & ~col_mask[7]) | south_west_flood(bishops, ~get_all() & ~col_mask[0]));
}


unsigned long long Engine::north_east_flood(unsigned long long bishops, unsigned long long prop)
{
    unsigned long long east_flood = 0;
    bishops = bishops & ~col_mask[7];

    while(bishops) 
    {
        east_flood = east_flood | bishops;
        bishops = (bishops << 9) & prop;
    }
    return((east_flood & ~col_mask[7]) << 9);
}

unsigned long long Engine::south_east_flood(unsigned long long bishops, unsigned long long prop)
{
    unsigned long long east_flood = 0;
    bishops = bishops & ~col_mask[7];

    while(bishops) 
    {
        east_flood = east_flood | bishops;
        bishops = (bishops >> 7) & prop;
    }
    return((east_flood & ~col_mask[7]) >> 7);
}

unsigned long long Engine::south_west_flood(unsigned long long bishops, unsigned long long prop)
{
    unsigned long long west_flood = 0;
    bishops = bishops & ~col_mask[0];

    while(bishops) 
    {
        west_flood = west_flood | bishops;
        bishops = (bishops >> 9) & prop;
    }
    return((west_flood & ~col_mask[0]) >> 9);
}

unsigned long long Engine::north_west_flood(unsigned long long bishops, unsigned long long prop)
{
    unsigned long long west_flood = 0;
    bishops = bishops & ~col_mask[0];

    while(bishops) 
    {
        west_flood = west_flood | bishops;
        bishops = (bishops << 7) & prop;
    }
    return((west_flood & ~col_mask[0]) << 7);
}