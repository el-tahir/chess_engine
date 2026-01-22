#ifndef TYPES_H
#define TYPES_H

const int CASTLE_WK = 1; // 0001
const int CASTLE_WQ = 2; // 0010
const int CASTLE_BK = 4; // 0100
const int CASTLE_BQ = 8; // 1000

enum Piece {
    W_PAWN, W_ROOK, W_KNIGHT, W_BISHOP, W_QUEEN, W_KING,
    B_PAWN, B_ROOK, B_KNIGHT, B_BISHOP, B_QUEEN, B_KING,
    EMPTY
};

enum Color {
    WHITE = 0,
    BLACK = 1
};

struct Move {
    int from;
    int to;

    bool operator==(const Move& other) {
        return from == other.from && to == other.to;
    }
};

#endif
