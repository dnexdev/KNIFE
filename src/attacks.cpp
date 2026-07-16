#include "attacks.h"
#include "random.h"
#include <algorithm>
#include <cstring>

static constexpr int BishopDirs[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
static constexpr int RookDirs[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

static Bitboard slidingAttacks(int sq, Bitboard occ, const int dirs[][2], int n) {
    Bitboard attacks = 0;
    int r = rankOf(sq);
    int f = fileOf(sq);

    for (int d = 0; d < n; d++) {
        int nr = r + dirs[d][0];
        int nf = f + dirs[d][1];
        while (nr >= 0 && nr < 8 && nf >= 0 && nf < 8) {
            int tsq = nr * 8 + nf;
            attacks |= bit(tsq);
            if (occ & bit(tsq)) break;
            nr += dirs[d][0];
            nf += dirs[d][1];
        }
    }
    return attacks;
}

Bitboard Attacks::indexMagic(const MagicEntry& entry, Bitboard occ) {
    occ &= entry.mask;
    return entry.attacks[(occ * entry.magic) >> entry.shift];
}

void Attacks::initMagics(int pt, MagicEntry entries[64], std::vector<Bitboard>& table) {
    const int (*dirs)[2] = (pt == BISHOP) ? BishopDirs : RookDirs;
    int permutations[64];
    size_t total = 0;

    for (int sq = 0; sq < 64; sq++) {
        Bitboard edges = ((RANK_1 | RANK_8) & ~rankBB(sq)) | ((A_FILE | H_FILE) & ~fileBB(sq));
        Bitboard mask = slidingAttacks(sq, 0, dirs, 4) & ~edges;
        int bits = popcount(mask);
        permutations[sq] = 1 << bits;
        entries[sq].mask = mask;
        entries[sq].shift = 64 - bits;
        total += permutations[sq];
    }

    table.resize(total);
    size_t offset = 0;

    for (int sq = 0; sq < 64; sq++) {
        Bitboard mask = entries[sq].mask;
        int bits = popcount(mask);
        int perms = permutations[sq];

        entries[sq].attacks = table.data() + offset;

        bool found = false;
        for (int attempt = 0; attempt < 100000 && !found; attempt++) {
            uint64_t magic = prng.randMagic();
            if (popcount((mask * magic) >> 56) < 6) continue;

            std::vector<Bitboard> used(perms, 0);
            bool ok = true;

            for (int i = 0; i < perms; i++) {
                Bitboard occ = 0;
                Bitboard m = mask;
                for (int b = 0; b < bits; b++) {
                    if (i & (1 << b)) occ |= bit(lsb(m));
                    m &= m - 1;
                }

                int idx = static_cast<int>((occ * magic) >> entries[sq].shift);
                Bitboard atk = slidingAttacks(sq, occ, dirs, 4);

                if (used[idx] == 0) used[idx] = atk;
                else if (used[idx] != atk) { ok = false; break; }
            }

            if (ok) {
                entries[sq].magic = magic;
                for (int i = 0; i < perms; i++) {
                    Bitboard occ = 0;
                    Bitboard m = mask;
                    for (int b = 0; b < bits; b++) {
                        if (i & (1 << b)) occ |= bit(lsb(m));
                        m &= m - 1;
                    }
                    int idx = static_cast<int>((occ * magic) >> entries[sq].shift);
                    entries[sq].attacks[idx] = slidingAttacks(sq, occ, dirs, 4);
                }
                found = true;
            }
        }

        if (!found) {
            entries[sq].magic = 0;
            entries[sq].shift = 0;
        }

        offset += perms;
    }
}

void Attacks::init() {
    prng.seedRandom(0x123456789ABCDEFULL);

    for (int sq = 0; sq < 64; sq++) {
        int r = rankOf(sq);
        int f = fileOf(sq);

        Bitboard knight = 0;
        for (int dr = -2; dr <= 2; dr++) {
            for (int df = -2; df <= 2; df++) {
                if (std::abs(dr) + std::abs(df) == 3) {
                    int nr = r + dr, nf = f + df;
                    if (nr >= 0 && nr < 8 && nf >= 0 && nf < 8) {
                        knight |= bit(nr * 8 + nf);
                    }
                }
            }
        }
        KnightTable[sq] = knight;

        Bitboard king = 0;
        for (int dr = -1; dr <= 1; dr++) {
            for (int df = -1; df <= 1; df++) {
                if (dr == 0 && df == 0) continue;
                int nr = r + dr, nf = f + df;
                if (nr >= 0 && nr < 8 && nf >= 0 && nf < 8) {
                    king |= bit(nr * 8 + nf);
                }
            }
        }
        KingTable[sq] = king;

        Bitboard wp = 0, bp = 0;
        if (r > 0) {
            if (f > 0) wp |= bit((r - 1) * 8 + f - 1);
            if (f < 7) wp |= bit((r - 1) * 8 + f + 1);
        }
        if (r < 7) {
            if (f > 0) bp |= bit((r + 1) * 8 + f - 1);
            if (f < 7) bp |= bit((r + 1) * 8 + f + 1);
        }
        PawnTable[WHITE][sq] = wp;
        PawnTable[BLACK][sq] = bp;
    }

    BishopAttacks.clear();
    RookAttacks.clear();
    initMagics(BISHOP, BishopMagics.data(), BishopAttacks);
    initMagics(ROOK, RookMagics.data(), RookAttacks);
}

Bitboard Attacks::pawnAttacks(int sq, int color) {
    return PawnTable[color][sq];
}

Bitboard Attacks::knightAttacks(int sq) {
    return KnightTable[sq];
}

Bitboard Attacks::bishopAttacks(int sq, Bitboard occ) {
    const MagicEntry& e = BishopMagics[sq];
    if (!e.magic) return slidingAttacks(sq, occ, BishopDirs, 4);
    return indexMagic(e, occ);
}

Bitboard Attacks::rookAttacks(int sq, Bitboard occ) {
    const MagicEntry& e = RookMagics[sq];
    if (!e.magic) return slidingAttacks(sq, occ, RookDirs, 4);
    return indexMagic(e, occ);
}

Bitboard Attacks::queenAttacks(int sq, Bitboard occ) {
    return bishopAttacks(sq, occ) | rookAttacks(sq, occ);
}

Bitboard Attacks::kingAttacks(int sq) {
    return KingTable[sq];
}

Bitboard Attacks::attacksByType(int sq, int pt, Bitboard occ) {
    switch (pt) {
        case PAWN:   return 0;
        case KNIGHT: return knightAttacks(sq);
        case BISHOP: return bishopAttacks(sq, occ);
        case ROOK:   return rookAttacks(sq, occ);
        case QUEEN:  return queenAttacks(sq, occ);
        case KING:   return kingAttacks(sq);
        default:     return 0;
    }
}

Bitboard Attacks::attacksFrom(int sq, int piece, Bitboard occ) {
    int pt = pieceType(piece);
    if (pt == PAWN) return pawnAttacks(sq, piece & 1);
    return attacksByType(sq, pt, occ);
}
