#include "zobrist.h"
#include "bitboard.h"
#include "move.h"
#include "random.h"

std::array<std::array<uint64_t, 64>, 12> Zobrist::pieces;
std::array<uint64_t, 64> Zobrist::epKeys;
std::array<uint64_t, 16> Zobrist::castleKeys;
uint64_t Zobrist::sideKey;

void Zobrist::init() {
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 64; j++) {
            pieces[i][j] = prng.rand64();
        }
    }
    for (int i = 0; i < 64; i++) {
        epKeys[i] = prng.rand64();
    }
    for (int i = 0; i < 16; i++) {
        castleKeys[i] = prng.rand64();
    }
    sideKey = prng.rand64();
}

uint64_t Zobrist::hash(const Position& pos) {
    uint64_t h = 0;
    for (int piece = WHITE_PAWN; piece <= BLACK_KING; piece++) {
        Bitboard pcs = pos.pieces(piece);
        while (pcs) {
            h ^= pieces[piece][popLsb(pcs)];
        }
    }
    if (pos.epSquare()) {
        h ^= epKeys[pos.epSquare()];
    }
    h ^= castleKeys[pos.castling()];
    if (pos.stm() == BLACK) {
        h ^= sideKey;
    }
    return h;
}

uint64_t Zobrist::pawnHash(const Position& pos) {
    uint64_t h = 0;
    for (int piece = WHITE_PAWN; piece <= BLACK_PAWN; piece++) {
        Bitboard pcs = pos.pieces(piece);
        while (pcs) {
            h ^= pieces[piece][popLsb(pcs)];
        }
    }
    if (pos.stm() == BLACK) {
        h ^= sideKey;
    }
    return h;
}

uint64_t Zobrist::keyAfter(const Position& pos, Move move) {
    if (!move) return pos.zobrist() ^ sideKey;

    int from = fromSq(move);
    int to = toSq(move);
    int moving = movingPiece(move);

    uint64_t newKey = pos.zobrist() ^ sideKey ^ pieces[moving][from] ^ pieces[moving][to];

    int captured = pos.pieceAt(to);
    if (captured != NO_PIECE) {
        newKey ^= pieces[captured][to];
    }
    return newKey;
}
