#include "bitboard.hpp"

typedef unsigned long long U64;


// PAWNS
U64 Engine::white_pawn_attacks(U64 white_pawns)
{
    // left side, filter out left file
    U64 pawn_right = (white_pawns & ~col_mask[0]) << 7;

    // right side, filter out right file
    U64 pawn_left = (white_pawns & ~col_mask[7]) << 9;

    // or together pawn attacks
    return(pawn_right | pawn_left);
}

U64 Engine::black_pawn_attacks(U64 black_pawns)
{
    // left side, filter out left file
    U64 pawn_left = (black_pawns & ~col_mask[0]) >> 9;

    // right side, filter out right file
    U64 pawn_right = (black_pawns & ~col_mask[7]) >> 7;

    // or together pawn attacks
    return(pawn_left | pawn_right);
}

U64 Engine::white_pawn_moves(U64 white_pawns, U64 all_pieces, U64 all_black_pieces)
{
    // pushes all pawns forard one
    U64 pawn_one = (white_pawns << 8) & ~all_pieces; 

    // get all pieces that may be able to move to rank 4 in a double push, masks with rank 2 pawns first
    U64 pawn_two = ((pawn_one & row_mask[2]) << 8) & ~all_pieces; 

    // or together for total moves
    U64 both_pawn = pawn_one | pawn_two;

    // attacks
    U64 pawn_attacks = white_pawn_attacks(white_pawns);

    // and together black pieces
    U64 pawn_valid_attacks = pawn_attacks & all_black_pieces;

    // or together moves and attacks and return
    return(both_pawn | pawn_valid_attacks);
}

U64 Engine::black_pawn_moves(U64 black_pawns, U64 all_pieces, U64 all_white_pieces)
{
    // pushes all pawns forard one
    U64 pawn_one = (black_pawns >> 8) & ~all_pieces; 

    // get all pieces that may be able to move to rank 4 in a double push, masks with rank 2 pawns first
    U64 pawn_two = ((pawn_one & row_mask[5]) >> 8) & ~all_pieces; 

    // or together for total moves
    U64 both_pawn = pawn_one | pawn_two;

    // attacks
    U64 pawn_attacks = black_pawn_attacks(black_pawns);

    // and together black pieces
    U64 pawn_valid_attacks = pawn_attacks & all_white_pieces;

    // or together moves and attacks and return
    return(both_pawn | pawn_valid_attacks);
}



//KINGS



U64 Engine::king_attacks(U64 kings)
{
    U64 king_mask_file_0 = kings & ~col_mask[0];
    U64 king_mask_file_7 = kings & ~col_mask[7];

    U64 spot_0 = king_mask_file_7 >> 7; // Southeast
    U64 spot_1 = kings >> 8; // South
    U64 spot_2 = king_mask_file_0 >> 9; // Southwest
    U64 spot_3 = king_mask_file_0 >> 1; // West

    U64 spot_4 = king_mask_file_0 << 7; // Northwest
    U64 spot_5 = kings << 8; // North
    U64 spot_6 = king_mask_file_7 << 9; // Northeast
    U64 spot_7 = king_mask_file_7 << 1; // East

    return(spot_0 | spot_1 | spot_2 | spot_3 | spot_4 | spot_5 | spot_6 | spot_7);
}

// Takes in king_rep (bitboad representing that colors king location)
// Takes in same_occupied (bitboard representing all pieces of that color)
// Returns bitboard representing all possible pre_check moves that the king can make
U64 Engine::king_moves(U64 kings, U64 own_occupied)
{
    return(king_attacks(kings) & ~own_occupied);
}

U64 Engine::king_moves(int color)
{
    if(color == 1)
    {
        return(king_moves(pos.white_kings, get_all_white()));
    }
    else
    {
        return(king_moves(pos.black_kings, get_all_black()));
    }
}


//NIGHTS


U64 Engine::night_attacks(U64 nights)
{
    U64 spot_1_clip = ~col_mask[0] & ~col_mask[1];
    U64 spot_2_clip = ~col_mask[0];
    U64 spot_3_clip = ~col_mask[7];
    U64 spot_4_clip = ~col_mask[7] & ~col_mask[6];

    U64 spot_5_clip = ~col_mask[7] & ~col_mask[6];
    U64 spot_6_clip = ~col_mask[7];
    U64 spot_7_clip = ~col_mask[0];
    U64 spot_8_clip = ~col_mask[0] & ~col_mask[1];

    U64 spot_1 = (nights & spot_1_clip) << 6;
    U64 spot_2 = (nights & spot_2_clip) << 15;
    U64 spot_3 = (nights & spot_3_clip) << 17;
    U64 spot_4 = (nights & spot_4_clip) << 10;

    U64 spot_5 = (nights & spot_5_clip) >> 6;
    U64 spot_6 = (nights & spot_6_clip) >> 15;
    U64 spot_7 = (nights & spot_7_clip) >> 17;
    U64 spot_8 = (nights & spot_8_clip) >> 10;

    return(spot_1 | spot_2 | spot_3 | spot_4 | spot_5 | spot_6 | spot_7 | spot_8);
}


// Takes in night_rep (bitboad representing that colors night location)
// Takes in same_occupied (bitboard representing all pieces of that color)
// Returns bitboard representing all possible pre_check moves that that night can make
U64 Engine::night_moves(U64 nights, U64 own_occupied)
{
    return(night_attacks(nights) & ~own_occupied);
}

U64 Engine::night_moves(int color)
{
    if(color == 1)
    {
        return(night_moves(pos.white_nights, get_all_white()));
    }
    else
    {
        return(night_moves(pos.black_nights, get_all_black()));
    }
}


/////BISHOPS//////




// olds
// U64 Engine::one_bishop_attacks_ANTI(U64 bishop, int right_diag)
// {
//     // int* diags = get_diag(get_rank(bishop), get_file(bishop));
//     // int right_diag = get_diag(get_rank(bishop), get_file(bishop)) >> 5;
//     U64 line_mask = ~bishop & diag_right_mask[right_diag]; // excludes square of slider
//     // free(diags);

//     U64 forward = get_all() & line_mask; // also performs the first subtraction by clearing the s in o
//     U64 reverse = vertical_flip(forward); // o'-s'

//     forward = forward - bishop; // o -2s
//     reverse = reverse - vertical_flip(bishop); // o'-2s'
//     forward = forward ^ vertical_flip(reverse);
//     return forward & line_mask;      // mask the line again
// }


// U64 Engine::one_bishop_attacks(U64 bishop)
// {
//     int diag = get_diag(get_rank(bishop), get_file(bishop));
//     int left_diag = diag >> 5;
//     int right_diag = diag & 0x000000000000000F;

//     U64 line_mask = ~bishop & diag_left_mask[left_diag]; // excludes square of slider

//     U64 forward = get_all() & line_mask; // also performs the first subtraction by clearing the s in o
//     U64 reverse = vertical_flip(forward); // o'-s'

//     forward = forward - bishop; // o -2s
//     reverse = reverse - vertical_flip(bishop); // o'-2s'
//     forward = forward ^ vertical_flip(reverse);

//     return one_bishop_attacks_ANTI(bishop, right_diag) | (forward & line_mask);      // mask the line again
// }




// bishops may be missing the ANTI diagonal: https://chessprogramming.wikispaces.com/Hyperbola+Quintessence
U64 Engine::one_bishop_attacks_ANTI(U64 bishop, int square)
{
    U64 line_mask = square_masks[square].right_diag_mask_excluded; // excludes square of slider

    U64 forward = get_all() & line_mask; // also performs the first subtraction by clearing the s in o
    U64 reverse = vertical_flip(forward); // o'-s'

    forward = forward - bishop; // o -2s
    reverse = reverse - vertical_flip(bishop); // o'-2s'
    forward = forward ^ vertical_flip(reverse);
    return forward & line_mask;      // mask the line again
}


U64 Engine::one_bishop_attacks(U64 bishop)
{
    int square = bitboard_to_square(bishop);

    U64 line_mask = square_masks[square].left_diag_mask_excluded; // excludes square of slider

    U64 forward = get_all() & line_mask; // also performs the first subtraction by clearing the s in o
    U64 reverse = vertical_flip(forward); // o'-s'

    forward = forward - bishop; // o -2s
    reverse = reverse - vertical_flip(bishop); // o'-2s'
    forward = forward ^ vertical_flip(reverse);

    return one_bishop_attacks_ANTI(bishop, square) | (forward & line_mask);      // mask the line again
}

U64 Engine::bishop_attacks(U64 bishops)
{
    U64 one_bishop;
    U64 bishop_attacks = 0;
    while(bishops)
    {
        one_bishop = lsb_board(bishops);
        bishops = bishops & ~one_bishop;
        bishop_attacks = bishop_attacks | one_bishop_attacks(one_bishop);
    }
    return bishop_attacks;
}

U64 Engine::one_bishop_moves(U64 bishop, U64 all_occupied, U64 own_occupied)
{
    return one_bishop_attacks(bishop) & ~own_occupied;
}

U64 Engine::bishop_moves(U64 bishops, U64 all_occupied, U64 own_occupied)
{
    U64 one_bishop;
    U64 bishop_moves = 0;
    while(bishops)
    {
        one_bishop = lsb_board(bishops);
        bishops = bishops & ~one_bishop;
        bishop_moves = bishop_moves | one_bishop_moves(one_bishop, all_occupied, own_occupied);
    }
    return bishop_moves;
}

U64 Engine::bishop_moves(int color)
{
    if(color == 1)
    {
        return bishop_moves(pos.white_bishops, color);
    }
    else
    {
        return bishop_moves(pos.black_bishops, color);
    }
}

U64 Engine::bishop_moves(U64 bishop, int color)
{
    if(color == 1)
    {
        return(bishop_moves(bishop, get_all(), get_all_white()));
    }
    else
    {
        return(bishop_moves(bishop, get_all(), get_all_black()));
    }
}



/////ROOKS//////


//old
// U64 Engine::one_rook_attacks(U64 rook)
// {
//     U64 row = get_rank(rook);
//     U64 col = get_file(rook);

//     U64 o = get_all();

//     U64 o_rev = vertical_flip(o);
//     U64 s_rev = vertical_flip(rook);

//     U64 hori = (o - 2*rook) ^ vertical_flip(o_rev - 2*s_rev);
//     printf("hori\n");
//     print_chess_rep(hori);
//     printf("hori flipped\n");
//     print_chess_rep(vertical_flip(hori));
//     hori = hori & row_mask[row];


//     // U64 o_mask = o;
//     U64 o_mask = o & col_mask[col];
//     U64 o_rev_mask = vertical_flip(o_mask);
//     U64 vert = (o_mask - 2*rook) ^ vertical_flip(o_rev_mask - 2*s_rev);
//     vert = vert & col_mask[col];

//     return(hori | vert);
// }

U64 Engine::one_rook_attacks(U64 rook, int square)
{
    U64 o = get_all();
    
    U64 o_rev = reverse_64_bits(o);
    U64 s_rev = reverse_64_bits(rook);

    U64 hori = (o - 2*rook) ^ reverse_64_bits(o_rev - 2*s_rev);
    hori = hori & square_masks[square].rank_mask;

    U64 o_mask = o & square_masks[square].file_mask;
    U64 o_rev_mask = reverse_64_bits(o_mask);
    U64 vert = (o_mask - 2*rook) ^ reverse_64_bits(o_rev_mask - 2*s_rev);
    vert = vert & square_masks[square].file_mask;

    return(hori | vert);
}


// U64 Engine::one_rook_attacks(U64 rook)
// {
//     int square = bitboard_to_square(rook);
//     U64 occ = get_all();

//     printf("masks\n");
//     print_chess_rep(square_masks[square].file_mask_excluded);


//     U64 forward, reverse;
//     forward  = occ & square_masks[square].file_mask_excluded;

//     reverse  = vertical_flip(forward);
//     forward -= rook;
//     printf("forward\n");
//     print_chess_rep(forward);

//     reverse -= vertical_flip(rook);
//     forward ^= vertical_flip(reverse);
//     forward &= square_masks[square].file_mask_excluded;

//     // exit(0);
//     return forward;
// }


U64 Engine::rook_attacks(U64 rooks)
{
    U64 one_rook;
    U64 rook_attacks = 0;
    while(rooks)
    {
        one_rook = lsb_board(rooks);
        rooks = rooks & ~one_rook;
        rook_attacks = rook_attacks | one_rook_attacks(one_rook);
    }
    return rook_attacks;
}

U64 Engine::one_rook_moves(U64 rook, U64 all_occupied, U64 own_occupied)
{
    return one_rook_attacks(rook) & ~own_occupied;
}

U64 Engine::rook_moves(U64 rooks, U64 all_occupied, U64 own_occupied)
{
    U64 one_rook;
    U64 rook_moves = 0;
    while(rooks)
    {
        one_rook = lsb_board(rooks);
        rooks = rooks & ~one_rook;
        rook_moves = rook_moves | one_rook_moves(one_rook, all_occupied, own_occupied);
    }
    return rook_moves;
}

U64 Engine::rook_moves(int color)
{
    if(color == 1)
    {
        return rook_moves(pos.white_rooks, color);
    }
    else
    {
        return rook_moves(pos.black_rooks, color);
    }
}

U64 Engine::rook_moves(U64 rook, int color)
{
    if(color == 1)
    {
        return(rook_moves(rook, get_all(), get_all_white()));
    }
    else
    {
        return(rook_moves(rook, get_all(), get_all_black()));
    }
}



/////QUEENS//////


U64 Engine::one_queen_attacks(U64 queen)
{
    return one_bishop_attacks(queen) |
            one_rook_attacks(queen);
}

U64 Engine::queen_attacks(U64 queens)
{
    U64 one_queen;
    U64 queen_attacks = 0;
    while(queens)
    {
        one_queen = lsb_board(queens);
        queens = queens & ~one_queen;
        queen_attacks = queen_attacks | one_queen_attacks(one_queen);
    }
    return queen_attacks;
}

U64 Engine::one_queen_moves(U64 queen, U64 all_occupied, U64 own_occupied)
{
    return one_queen_attacks(queen) & ~own_occupied;
}

U64 Engine::queen_moves(U64 queens, U64 all_occupied, U64 own_occupied)
{
    U64 one_queen;
    U64 queen_moves = 0;
    while(queens)
    {
        one_queen = lsb_board(queens);
        queens = queens & ~one_queen;
        queen_moves = queen_moves | one_queen_moves(one_queen, all_occupied, own_occupied);
    }
    return queen_moves;
}


U64 Engine::queen_moves(int color)
{
    if(color == 1)
    {
        return queen_moves(pos.white_queens, color);
    }
    else
    {
        return queen_moves(pos.black_queens, color);
    }
}

U64 Engine::queen_moves(U64 queen, int color)
{
    if(color == 1)
    {
        return bishop_moves(queen, get_all(), get_all_white()) | 
            rook_moves(queen, get_all(), get_all_white());
    }
    else
    {
        return bishop_moves(queen, get_all(), get_all_black()) | 
            rook_moves(queen, get_all(), get_all_black());
    }
}




// FLOODS

U64 Engine::vert_flood(U64 rooks)
{
    return(north_flood(rooks, ~get_all()) | south_flood(rooks, ~get_all()));
}

U64 Engine::hori_flood(U64 rooks)
{
    return(east_flood(rooks, ~get_all() & ~col_mask[7]) | west_flood(rooks, ~get_all() & ~col_mask[0]));
}


U64 Engine::north_flood(U64 rooks, U64 prop)
{
    U64 north_flood = 0;

    while(rooks) 
    {
        north_flood = north_flood | rooks;
        rooks = (rooks << 8) & prop;
    }
    return(north_flood << 8);
}

U64 Engine::south_flood(U64 rooks, U64 prop)
{
    U64 south_flood = 0;

    while(rooks) 
    {
        south_flood = south_flood | rooks;
        rooks = (rooks >> 8) & prop;
    }
    return(south_flood >> 8);
}


U64 Engine::east_flood(U64 rooks, U64 prop)
{
    U64 east_flood = 0;
    rooks = rooks & ~col_mask[7];

    while(rooks) 
    {
        east_flood = east_flood | rooks;
        rooks = (rooks << 1) & prop;
    }
    return((east_flood & ~col_mask[7]) << 1);
}

U64 Engine::west_flood(U64 rooks, U64 prop)
{
    U64 west_flood = 0;
    rooks = rooks & ~col_mask[0];

    while(rooks) 
    {
        west_flood = west_flood | rooks;
        rooks = (rooks >> 1) & prop;
    }
    return((west_flood & ~col_mask[0]) >> 1);
}


U64 Engine::left_diag_flood(U64 bishops)
{
    return(north_west_flood(bishops, ~get_all() & ~col_mask[0]) | south_east_flood(bishops, ~get_all() & ~col_mask[7]));
}

U64 Engine::right_diag_flood(U64 bishops)
{
    return(north_east_flood(bishops, ~get_all() & ~col_mask[7]) | south_west_flood(bishops, ~get_all() & ~col_mask[0]));
}


U64 Engine::north_east_flood(U64 bishops, U64 prop)
{
    U64 east_flood = 0;
    bishops = bishops & ~col_mask[7];

    while(bishops) 
    {
        east_flood = east_flood | bishops;
        bishops = (bishops << 9) & prop;
    }
    return((east_flood & ~col_mask[7]) << 9);
}

U64 Engine::south_east_flood(U64 bishops, U64 prop)
{
    U64 east_flood = 0;
    bishops = bishops & ~col_mask[7];

    while(bishops) 
    {
        east_flood = east_flood | bishops;
        bishops = (bishops >> 7) & prop;
    }
    return((east_flood & ~col_mask[7]) >> 7);
}

U64 Engine::south_west_flood(U64 bishops, U64 prop)
{
    U64 west_flood = 0;
    bishops = bishops & ~col_mask[0];

    while(bishops) 
    {
        west_flood = west_flood | bishops;
        bishops = (bishops >> 9) & prop;
    }
    return((west_flood & ~col_mask[0]) >> 9);
}

U64 Engine::north_west_flood(U64 bishops, U64 prop)
{
    U64 west_flood = 0;
    bishops = bishops & ~col_mask[0];

    while(bishops) 
    {
        west_flood = west_flood | bishops;
        bishops = (bishops << 7) & prop;
    }
    return((west_flood & ~col_mask[0]) << 7);
}