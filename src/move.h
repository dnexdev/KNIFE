#pragma once

#include "bitboard.h"
#include "types.h"

constexpr Move NO_MOVE = 0;
constexpr Move NULL_MOVE = 0;

constexpr int QUIET_FLAG = 0b0000;
constexpr int CASTLE_FLAG = 0b0001;
constexpr int CAPTURE_FLAG = 0b0100;
constexpr int EP_FLAG = 0b0110;
constexpr int PROMO_FLAG = 0b1000;
constexpr int KNIGHT_PROMO_FLAG = 0b1000;
constexpr int BISHOP_PROMO_FLAG = 0b1001;
constexpr int ROOK_PROMO_FLAG = 0b1010;
constexpr int QUEEN_PROMO_FLAG = 0b1011;

inline constexpr Move buildMove(int from, int to, int piece, int flags) {
    return static_cast<Move>(from | (to << 6) | (piece << 12) | (flags << 16));
}

inline constexpr int fromTo(Move move) { return static_cast<int>(move) & 0x00fff; }
inline constexpr int fromSq(Move move) { return static_cast<int>(move) & 0x0003f; }
inline constexpr int toSq(Move move) { return (static_cast<int>(move) & 0x00fc0) >> 6; }
inline constexpr int movingPiece(Move move) { return (static_cast<int>(move) & 0x0f000) >> 12; }
inline constexpr int flags(Move move) { return (static_cast<int>(move) & 0xf0000) >> 16; }

inline constexpr bool isCap(Move move) { return (flags(move) & CAPTURE_FLAG) != 0; }
inline constexpr bool isEP(Move move) { return flags(move) == EP_FLAG; }
inline constexpr bool isCas(Move move) { return flags(move) == CASTLE_FLAG; }
inline constexpr bool isPromo(Move move) { return (flags(move) & PROMO_FLAG) != 0; }

inline constexpr int promoPT(Move move) { return (flags(move) & 0x3) + KNIGHT; }
inline constexpr int promoPiece(Move move, int stm) { return makePiece(promoPT(move), stm); }
