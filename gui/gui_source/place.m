function board = place(board,sq,piece)
%place Returns number ranges corresponding to a square number
%   Takes in a base board
%   Takes in a square number between 1-64
%   Takes a 100x100 pixel piece image
%   Returns: A new board identical to the passed in board except with the
%   given piece placed on the given square
[xVals,yVals, bk] = sqLookup(sq);

piece = double(rgb2gray(piece));

if bk
    piece(piece == 244) = 0.8125;
    piece(piece == 73) = 0.8125;
else
    piece(piece == 244) = 73;
end

board(xVals,yVals) = piece;
end