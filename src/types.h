#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <string_view>

enum PieceType : int {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};

enum Piece : int {
    WHITE_PAWN,
    BLACK_PAWN,
    WHITE_KNIGHT,
    BLACK_KNIGHT,
    WHITE_BISHOP,
    BLACK_BISHOP,
    WHITE_ROOK,
    BLACK_ROOK,
    WHITE_QUEEN,
    BLACK_QUEEN,
    WHITE_KING,
    BLACK_KING,
    NO_PIECE
};

enum Square : int {
    A8, B8, C8, D8, E8, F8, G8, H8,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A1, B1, C1, D1, E1, F1, G1, H1
};

enum Color : int {
    WHITE,
    BLACK,
    BOTH
};

constexpr int NO_SQ = -1;

using Bitboard = uint64_t;
using Move = uint32_t;

extern const std::array<int, 128> CHAR_TO_PIECE;
extern const char* PIECE_TO_CHAR;
extern const char* SQ_TO_COORD[64];

int squareFromCoord(std::string_view coord);
std::string moveToString(Move move);
