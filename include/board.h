#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <string>
#include <vector>

#include "types.h"

int square_to_index(const std::string& square);
std::string index_to_square(int index);

struct Board {
    std::array<Piece, 64> board;
    Color side_to_move;
    int castling_rights;
    int en_passant_square = -1;

    void init_board();
    char get_piece_char(Piece p);
    Piece get_piece_from_char(char c);
    int get_piece_value(Piece p);
    int evaluate();
    void print_board();
    bool is_white_turn();
    UndoInfo make_move(Move move);
    void unmake_move(Move move, const UndoInfo& state);
    int find_king(Color side);
    std::vector<Move> generate_pseudo_moves();
    std::vector<Move> generate_moves();
    int search(int depth, int alpha, int beta);
    Move get_best_move(int depth);
    Move parse_move(std::string input);
    bool is_square_attacked(int square, Color side_attacking);
    void load_fen(std::string fen);
};


struct UndoInfo {
    Piece moved_piece; // piece from move.from before moving
    Piece captured_piece; // EMPTY if none
    int captured_square;  // move.to normally, or en passant pawn square
    int prev_castling_rights;
    int prev_en_passant_square;
};

#endif
