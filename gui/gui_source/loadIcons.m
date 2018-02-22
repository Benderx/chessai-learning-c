function loadIcons()
%loadIcons loads in chess icons
%   Takes nothing
%   Returns noting
%   Loads in all the chess icons and sends them to the calling workspace
icon_path = '..\gui_chess_icons\';

load([icon_path 'whiteKing.MAT']);
assignin('caller','whiteKing',whiteKing);
load([icon_path 'blackKing.MAT']);
assignin('caller','blackKing',blackKing);

load([icon_path 'whiteQueen.MAT']);
assignin('caller','whiteQueen',whiteQueen);
load([icon_path 'blackQueen.MAT']);
assignin('caller','blackQueen',blackQueen);

load([icon_path 'whiteRook.MAT']);
assignin('caller','whiteRook',whiteRook);
load([icon_path 'blackRook.MAT']);
assignin('caller','blackRook',blackRook);

load([icon_path 'whiteBishop.MAT']);
assignin('caller','whiteBishop',whiteBishop);
load([icon_path 'blackBishop.MAT']);
assignin('caller','blackBishop',blackBishop);

load([icon_path 'whiteNight.MAT']);
assignin('caller','whiteNight',whiteNight);
load([icon_path 'blackNight.MAT']);
assignin('caller','blackNight',blackNight);

load([icon_path 'whitePawn.MAT']);
assignin('caller','whitePawn',whitePawn);
load([icon_path 'blackPawn.MAT']);
assignin('caller','blackPawn',blackPawn);
end

