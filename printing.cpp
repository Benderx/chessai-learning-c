#include "bitboard.hpp"
#include <stdlib.h>
#include <fstream>

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

void Engine::print_masks()
{
    printf("printing diag_left_mask\n");
    for(int i = 0; i < 15; i++)
    {
        print_chess_rep(diag_left_mask[i]);
        printf("\n");
    }

    printf("printing diag_right_mask\n");
    for(int i = 0; i < 15; i++)
    {
        printf("%i\n", i);
        print_chess_rep(diag_right_mask[i]);
        printf("\n");
    }
    printf("\n");
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

//Takes in a file number
//Writes the boardstate to file
//Consider open and close functions
void Engine::write_move_to_file(int file_num)
{
    std::string file_name = ".\\games\\game" + std::to_string(file_num) + ".txt";
    std::ofstream file;

    file.open(file_name, std::fstream::in | std::fstream::out | std::fstream::app);

    file << pos.white_pawns << ", ";
    file << pos.white_rooks << ", ";
    file << pos.white_nights << ", ";
    file << pos.white_bishops << ", ";
    file << pos.white_queens << ", ";
    file << pos.white_kings << ", ";
    file << pos.black_pawns << ", ";
    file << pos.black_rooks << ", ";
    file << pos.black_nights << ", ";
    file << pos.black_bishops << ", ";
    file << pos.black_queens << ", ";
    file << pos.black_kings << std::endl;

    file.close();
}

void Engine::delete_file_if_present(int file_num)
{
    std::string file_name_string = ".\\games\\game" + std::to_string(file_num) + ".txt";
    char const *file_name_char_arr = file_name_string.c_str();

    std::remove(file_name_char_arr);
}

void Engine::load_in_string(std::string const rep)
{
    int index = 63;
    unsigned long long wp = 0, bp = 0;
    unsigned long long wr = 0, br = 0;
    unsigned long long wn = 0, bn = 0;
    unsigned long long wb = 0, bb = 0;
    unsigned long long wq = 0, bq = 0;
    unsigned long long wk = 0, bk = 0;

    for(unsigned int i = 0; i < rep.length(); i++)
    {
        switch(rep[i])
        {
            case 'P':
                wp = wp | 1ULL << index;
                break;
            case 'R':
                wr = wr | 1ULL << index;
                break;
            case 'N':
                wn = wn | 1ULL << index;
                break;
            case 'B':
                wb = wb | 1ULL << index;
                break;
            case 'Q':
                wq = wq | 1ULL << index;
                break;
            case 'K':
                wk = wk | 1ULL << index;
                break;
            case 'p':
                bp = bp | 1ULL << index;
                break;
            case 'r':
                br = br | 1ULL << index;
                break;
            case 'n':
                bn = bn | 1ULL << index;
                break;
            case 'b':
                bb = bb | 1ULL << index;
                break;
            case 'q':
                bq = bq | 1ULL << index;
                break;
            case 'k':
                bk = bk | 1ULL << index;
                break;
            case '-':
                //lol
                break;
            default:
                index++;
        }
        index--;
    }

    pos.white_pawns = horizontal_flip(wp);
    pos.white_rooks = horizontal_flip(wr); 
    pos.white_nights = horizontal_flip(wn);     
    pos.white_bishops = horizontal_flip(wb);
    pos.white_queens = horizontal_flip(wq);
    pos.white_kings = horizontal_flip(wk);

    pos.black_pawns = horizontal_flip(bp); 
    pos.black_rooks = horizontal_flip(br); 
    pos.black_nights = horizontal_flip(bn); 
    pos.black_bishops = horizontal_flip(bb);
    pos.black_queens = horizontal_flip(bq);
    pos.black_kings = horizontal_flip(bk);
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