#pragma once

#include "types.h"

constexpr Bitboard A_FILE = 0x0101010101010101ULL;
constexpr Bitboard B_FILE = 0x0202020202020202ULL;
constexpr Bitboard C_FILE = 0x0404040404040404ULL;
constexpr Bitboard D_FILE = 0x0808080808080808ULL;
constexpr Bitboard E_FILE = 0x1010101010101010ULL;
constexpr Bitboard F_FILE = 0x2020202020202020ULL;
constexpr Bitboard G_FILE = 0x4040404040404040ULL;
constexpr Bitboard H_FILE = 0x8080808080808080ULL;

constexpr Bitboard RANK_1 = 0xFF00000000000000ULL;
constexpr Bitboard RANK_2 = 0x00FF000000000000ULL;
constexpr Bitboard RANK_3 = 0x0000FF0000000000ULL;
constexpr Bitboard RANK_4 = 0x000000FF00000000ULL;
constexpr Bitboard RANK_5 = 0x00000000FF000000ULL;
constexpr Bitboard RANK_6 = 0x0000000000FF0000ULL;
constexpr Bitboard RANK_7 = 0x000000000000FF00ULL;
constexpr Bitboard RANK_8 = 0x00000000000000FFULL;

constexpr Bitboard DARK_SQS = 0x55AA55AA55AA55AAULL;

constexpr int WHITE_KS = 0x8;
constexpr int WHITE_QS = 0x4;
constexpr int BLACK_KS = 0x2;
constexpr int BLACK_QS = 0x1;

inline constexpr int fileOf(int sq) { return sq & 7; }
inline constexpr int rankOf(int sq) { return sq >> 3; }

inline constexpr int makePiece(int pt, int c) { return (pt << 1) + c; }
inline constexpr int pieceType(int pc) { return pc >> 1; }

inline constexpr Bitboard bit(int sq) { return 1ULL << sq; }

inline int lsb(Bitboard bb) { return __builtin_ctzll(bb); }
inline int msb(Bitboard bb) { return 63 ^ __builtin_clzll(bb); }

inline int popLsb(Bitboard& bb) {
    int sq = lsb(bb);
    bb &= bb - 1;
    return sq;
}

inline void flipBit(Bitboard& bb, int sq) { bb ^= bit(sq); }
inline void flipBits(Bitboard& bb, int sq1, int sq2) { bb ^= bit(sq1) ^ bit(sq2); }
