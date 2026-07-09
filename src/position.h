#pragma once

#include "types.h"
#include "bitboard.h"
#include "move.h"
#include <array>
#include <string>
#include <string_view>

struct BoardHistory {
    int castling;
    int ep;
    int fmr;
    int nullply;
    uint64_t zobrist;
    uint64_t pawnZobrist;
    Bitboard checkers;
    Bitboard pinned;
    Bitboard threatened;
    std::array<Bitboard, 6> threatenedBy;
    int capture;
};

class Position {
public:
    Position() { clear(); }

    void clear();
    void parseFen(std::string_view fen);
    std::string toFen() const;
    void print() const;

    void makeMove(Move move);
    void undoMove(Move move);
    void makeNullMove();
    void undoNullMove();

    bool isLegal(Move move) const;
    bool isPseudoLegal(Move move) const;
    bool doesMoveCheck(Move move) const;

    bool isDraw() const;
    bool isRepetition() const;
    bool isMaterialDraw() const;
    bool isFiftyMoveRule() const;

    int stm() const { return stm_; }
    int xstm() const { return xstm_; }
    Bitboard pieces(int pc) const { return pieces_[pc]; }
    Bitboard occupancies(int c) const { return occupancies_[c]; }
    int pieceAt(int sq) const { return squares_[sq]; }
    int epSquare() const { return epSquare_; }
    int castling() const { return castling_; }
    uint64_t zobrist() const { return zobrist_; }

private:
    void setSpecialPieces();
    void setThreats();
    void makeMoveUpdate(Move move, int update);

    int castling_;
    int epSquare_;
    int fmr_;
    int nullply_;
    uint64_t zobrist_;
    uint64_t pawnZobrist_;
    Bitboard checkers_;
    Bitboard pinned_;
    Bitboard threatened_;
    std::array<Bitboard, 6> threatenedBy_;
    int stm_;
    int xstm_;
    int histPly_;
    int moveNo_;
    uint64_t piecesCounts_;

    std::array<int, 64> squares_;
    std::array<Bitboard, 3> occupancies_;
    std::array<Bitboard, 12> pieces_;
    std::array<int, 4> cr_;
    std::array<int, 64> castlingRights_;
    std::array<BoardHistory, 300> history_;
};
