#include "types.h"
#include "move.h"
#include <cctype>

int squareFromCoord(std::string_view coord) {
    if (coord.size() < 2) return NO_SQ;
    int f = coord[0] - 'a';
    int r = 8 - (coord[1] - '0');
    if (f < 0 || f > 7 || r < 0 || r > 7) return NO_SQ;
    return r * 8 + f;
}

std::string moveToString(Move move) {
    if (!move) return "0000";
    std::string s = SQ_TO_COORD[fromSq(move)];
    s += SQ_TO_COORD[toSq(move)];
    if (isPromo(move)) {
        switch (promoPT(move)) {
            case KNIGHT: s += 'n'; break;
            case BISHOP: s += 'b'; break;
            case ROOK:   s += 'r'; break;
            case QUEEN:  s += 'q'; break;
            default: break;
        }
    }
    return s;
}
