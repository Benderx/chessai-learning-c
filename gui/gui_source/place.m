function board = place(board,sq,piece)
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