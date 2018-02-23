close all; clear; 

%% Create Board
units = false;
piece_show = true;
square_test = false;

white = ones(100);
black = white.*13/16;

u = [white black;black white];
two_row = [u u u u];
board = [two_row; two_row; two_row; two_row];

if units

figure
imshow(u);
title('unit cell')

figure 
imshow(two_row);
title('top two ranks')

figure 
imshow(board);
title('full board')

end

%% Load in icons
loadIcons()

%% Place
if piece_show
    figure 
    board = place(board,1,whiteKing);
    board = place(board,2,whiteKing);
    board = place(board,3,blackKing);
    board = place(board,4,blackKing);
    board = place(board,9,whiteQueen);
    board = place(board,10,whiteQueen);
    board = place(board,11,blackQueen);
    board = place(board,12,blackQueen);
    board = place(board,17,whiteRook);
    board = place(board,18,whiteRook);
    board = place(board,19,blackRook);
    board = place(board,20,blackRook);
    board = place(board,25,whiteBishop);
    board = place(board,26,whiteBishop);
    board = place(board,27,blackBishop);
    board = place(board,28,blackBishop);
    board = place(board,33,whiteNight);
    board = place(board,34,whiteNight);
    board = place(board,35,blackNight);
    board = place(board,36,blackNight);
    board = place(board,41,whitePawn);
    board = place(board,42,whitePawn);
    board = place(board,43,blackPawn);
    board = place(board,44,blackPawn);
    imshow(board);
end
if square_test
    for i = 1:64
        i
        board = place(board,i,whiteKing);
        imshow(board);
    end
end
imshow(board);
title('Placed Board')