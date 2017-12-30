#ifndef CONSTANTS_HXX_
#define CONSTANTS_HXX_

// Chess pieces
const int EMPTY = 0;
const int KING = 1;
const int QUEEN = 2;
const int BISHOP = 3;
const int KNIGHT = 4;
const int ROOK = 5;
const int PAWN = 6;

// Board cell
const int BOARDCELL = 7;

// Players
const int USER = 1;
const int AI = -1;

// Shaders
const int BLACK_BORDER = 10;
const int CEL_SHADING = 11;
const int COLOR_PICKING = 12;
const int SHADOW_MAPPING = 13;

// Process ids
const int PARENT_PROCESS_ID = 20;
const int CHILD_PROCESS_ID = 21;

// ShadowMapping
const int SHADOWMAPPING_HIGH = 1024;
const int SHADOWMAPPING_LOW = 512;
const int SHADOWMAPPING_VERYLOW = 256;

// Antialiasing level
const int ANTIALIASING_HIGH = 4;
const int ANTIALIASING_LOW = 2;
const int ANTIALIASING_NONE = 0;

#endif
