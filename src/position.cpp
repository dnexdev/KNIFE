#include "position.h"
#include "zobrist.h"
#include <cstdio>
#include <iostream>
#include <string>

void Position::clear() {
    pieces_.fill(0);
    occupancies_.fill(0);

    for (int i = 0; i < 64; i++) {
        squares_[i] = NO_PIECE;
    }

    piecesCounts_ = 0ULL;
    zobrist_ = 0ULL;
    pawnZobrist_ = 0ULL;

    stm_ = WHITE;
    xstm_ = BLACK;

    epSquare_ = 0;
    castling_ = 0;
    histPly_ = 0;
    moveNo_ = 1;
    fmr_ = 0;
    nullply_ = 0;
}

void Position::parseFen(std::string_view fenStr) {
    clear();
    const char* fen = fenStr.data();

    for (int i = 0; i < 64; i++) {
        if ((*fen >= 'a' && *fen <= 'z') || (*fen >= 'A' && *fen <= 'Z')) {
            int piece = CHAR_TO_PIECE[static_cast<unsigned char>(*fen)];
            pieces_[piece] |= bit(i);
            squares_[i] = piece;

            if (*fen != 'K' && *fen != 'k') {
                piecesCounts_ += (1ULL << (4 * piece));
            }
        } else if (*fen >= '0' && *fen <= '9') {
            int offset = *fen - '1';
            i += offset;
        } else if (*fen == '/') {
            i--;
        }
        fen++;
    }
    fen++;

    stm_ = (*fen++ == 'w' ? WHITE : BLACK);
    xstm_ = !stm_;

    castling_ = 0;
    cr_.fill(-1);

    int whiteKing = lsb(pieces(WHITE_KING));
    int blackKing = lsb(pieces(BLACK_KING));

    Bitboard whiteRooks = pieces(WHITE_ROOK) & RANK_1;
    Bitboard blackRooks = pieces(BLACK_ROOK) & RANK_8;

    while (*(++fen) != ' ') {
        if (*fen == 'K') {
            castling_ |= WHITE_KS;
            cr_[0] = msb(whiteRooks);
        } else if (*fen == 'Q') {
            castling_ |= WHITE_QS;
            cr_[1] = lsb(whiteRooks);
        } else if (*fen >= 'A' && *fen <= 'H') {
            castling_ |= ((*fen - 'A') > fileOf(whiteKing) ? WHITE_KS : WHITE_QS);
            cr_[(*fen - 'A') > fileOf(whiteKing) ? 0 : 1] = A1 + (*fen - 'A');
        } else if (*fen == 'k') {
            castling_ |= BLACK_KS;
            cr_[2] = msb(blackRooks);
        } else if (*fen == 'q') {
            castling_ |= BLACK_QS;
            cr_[3] = lsb(blackRooks);
        } else if (*fen >= 'a' && *fen <= 'h') {
            castling_ |= ((*fen - 'a') > fileOf(blackKing) ? BLACK_KS : BLACK_QS);
            cr_[(*fen - 'a') > fileOf(blackKing) ? 2 : 3] = A8 + (*fen - 'a');
        }
    }

    for (int i = 0; i < 64; i++) {
        castlingRights_[i] = castling_;

        if (i == whiteKing) castlingRights_[i] ^= (WHITE_KS | WHITE_QS);
        else if (i == blackKing) castlingRights_[i] ^= (BLACK_KS | BLACK_QS);
        else if (i == cr_[0]) castlingRights_[i] ^= WHITE_KS;
        else if (i == cr_[1]) castlingRights_[i] ^= WHITE_QS;
        else if (i == cr_[2]) castlingRights_[i] ^= BLACK_KS;
        else if (i == cr_[3]) castlingRights_[i] ^= BLACK_QS;
    }

    fen++;
    if (*fen != '-') {
        int f = fen[0] - 'a';
        int r = 8 - (fen[1] - '0');
        epSquare_ = r * 8 + f;
    } else {
        epSquare_ = 0;
    }

    while (*fen && *fen != ' ') fen++;
    sscanf(fen, " %d %d", &fmr_, &moveNo_);

    occupancies_[WHITE] = occupancies_[BLACK] = occupancies_[BOTH] = 0;
    for (int i = WHITE_PAWN; i <= BLACK_KING; i++) {
        occupancies_[i & 1] |= pieces_[i];
    }
    occupancies_[BOTH] = occupancies_[WHITE] | occupancies_[BLACK];

    setSpecialPieces();
    setThreats();

    zobrist_ = Zobrist::hash(*this);
    pawnZobrist_ = Zobrist::pawnHash(*this);
}

std::string Position::toFen() const {
    char fenBuffer[120];
    char* fen = fenBuffer;

    for (int r = 0; r < 8; r++) {
        int c = 0;
        for (int f = 0; f < 8; f++) {
            int sq = 8 * r + f;
            int piece = squares_[sq];
            if (piece != NO_PIECE) {
                if (c) *fen++ = static_cast<char>(c + '0');
                *fen++ = PIECE_TO_CHAR[piece];
                c = 0;
            } else {
                c++;
            }
        }
        if (c) *fen++ = static_cast<char>(c + '0');
        *fen++ = (r == 7) ? ' ' : '/';
    }

    *fen++ = stm_ ? 'b' : 'w';
    *fen++ = ' ';

    if (castling_) {
        if (castling_ & WHITE_KS) *fen++ = 'K';
        if (castling_ & WHITE_QS) *fen++ = 'Q';
        if (castling_ & BLACK_KS) *fen++ = 'k';
        if (castling_ & BLACK_QS) *fen++ = 'q';
    } else {
        *fen++ = '-';
    }

    *fen++ = ' ';
    snprintf(fen, 20, "%s %d %d", epSquare_ ? SQ_TO_COORD[epSquare_] : "-", fmr_, moveNo_);
    return std::string(fenBuffer);
}

void Position::print() const {
    for (int r = 0; r < 8; r++) {
        printf("+-------+-------+-------+-------+-------+-------+-------+-------+\n");
        printf("|");
        for (int f = 0; f < 16; f++) {
            if (f == 8) printf("\n|");
            int sq = r * 8 + (f > 7 ? f - 8 : f);

            if (f < 8) {
                if (squares_[sq] == NO_PIECE) printf("       |");
                else printf("   %c   |", PIECE_TO_CHAR[squares_[sq]]);
            } else {
                printf("       |");
            }
        }
        printf("\n");
    }
    printf("+-------+-------+-------+-------+-------+-------+-------+-------+\n");
    printf("\n FEN: %s\n\n", toFen().c_str());
}

void Position::setSpecialPieces() {}
void Position::setThreats() {}

void Position::makeMoveUpdate(Move move, int /*update*/) {
    int from = fromSq(move);
    int to = toSq(move);
    int piece = movingPiece(move);
    int captured = isEP(move) ? makePiece(PAWN, xstm_) : squares_[to];

    history_[histPly_].capture = captured;

    fmr_++;
    nullply_++;

    pieces_[piece] ^= (bit(from) | bit(to));
    occupancies_[stm_] ^= (bit(from) | bit(to));
    occupancies_[BOTH] ^= (bit(from) | bit(to));

    squares_[from] = NO_PIECE;
    squares_[to] = piece;
}

void Position::makeMove(Move move) {
    makeMoveUpdate(move, 1);
}

void Position::undoMove(Move /*move*/) {}
void Position::makeNullMove() {}
void Position::undoNullMove() {}
bool Position::isLegal(Move /*move*/) const { return true; }
bool Position::isPseudoLegal(Move /*move*/) const { return true; }
bool Position::doesMoveCheck(Move /*move*/) const { return false; }
bool Position::isDraw() const { return false; }
bool Position::isRepetition() const { return false; }
bool Position::isMaterialDraw() const { return false; }
bool Position::isFiftyMoveRule() const { return false; }
