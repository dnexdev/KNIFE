#pragma once

#include "position.h"
#include "types.h"
#include <array>
#include <cstdint>

class Zobrist {
public:
    static void init();
    static uint64_t hash(const Position& pos);
    static uint64_t pawnHash(const Position& pos);
    static uint64_t keyAfter(const Position& pos, Move move);

    static std::array<std::array<uint64_t, 64>, 12> pieces;
    static std::array<uint64_t, 64> epKeys;
    static std::array<uint64_t, 16> castleKeys;
    static uint64_t sideKey;
};
