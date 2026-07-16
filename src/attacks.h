#pragma once

#include "bitboard.h"
#include "types.h"
#include <array>
#include <vector>

struct MagicEntry {
    Bitboard mask;
    Bitboard magic;
    Bitboard* attacks;
    int shift;
};

class Attacks {
public:
    static void init();

    static Bitboard pawnAttacks(int sq, int color);
    static Bitboard knightAttacks(int sq);
    static Bitboard bishopAttacks(int sq, Bitboard occ);
    static Bitboard rookAttacks(int sq, Bitboard occ);
    static Bitboard queenAttacks(int sq, Bitboard occ);
    static Bitboard kingAttacks(int sq);

    static Bitboard attacksFrom(int sq, int piece, Bitboard occ);
    static Bitboard attacksByType(int sq, int pt, Bitboard occ);

    static inline std::array<Bitboard, 64> KnightTable;
    static inline std::array<Bitboard, 64> KingTable;
    static inline std::array<std::array<Bitboard, 64>, 2> PawnTable;

private:
    static Bitboard indexMagic(const MagicEntry& entry, Bitboard occ);
    static void initMagics(int pt, MagicEntry entries[64], std::vector<Bitboard>& table);

    static inline std::array<MagicEntry, 64> BishopMagics;
    static inline std::array<MagicEntry, 64> RookMagics;
    static inline std::vector<Bitboard> BishopAttacks;
    static inline std::vector<Bitboard> RookAttacks;
};
