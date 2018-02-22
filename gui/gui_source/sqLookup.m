function [xRange,yRange,color] = sqLookup(sq)
%sqLookup Returns number ranges corresponding to a square number
%   Takes in a square number between 1-64
%   Returns:
row = 7-double(idivide(sq-1,int8(8)));
col = mod(sq-1,8);
color = mod(row+col,2);

xRange = [1:100] + row.*100;
yRange = [1:100] + col.*100;
end