#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <string>
#include <vector>

#include "types.h"

int square_to_index(const std::string& square);
std::string index_to_square(int index);

struct UndoInfo {
    Piece moved_piece; // piece from move.from before moving
    Piece captured_piece; // EMPTY if none
    int captured_square;  // move.to normally, or en passant pawn square
    int prev_castling_rights;
    int prev_en_passant_square;
};

struct Board {
    std::array<Piece, 64> board;
    Color side_to_move;
    int castling_rights;
    int en_passant_square = -1;
};

void init_board(Board& board);
char get_piece_char(Piece p);
Piece get_piece_from_char(char c);
int get_piece_value(Piece p);
int evaluate(const Board& board);
void print_board(const Board& board);
bool is_white_turn(const Board& board);
UndoInfo make_move(Board& board, Move move);
void unmake_move(Board& board, Move move, const UndoInfo& state);
int find_king(const Board& board, Color side);
std::vector<Move> generate_pseudo_moves(const Board& board);
std::vector<Move> generate_moves(const Board& board);
int search(Board& board, int depth, int alpha, int beta);
Move get_best_move(Board& board, int depth);
Move parse_move(const std::string& input);
bool is_square_attacked(const Board& board, int square, Color side_attacking);
void load_fen(Board& board, const std::string& fen);




#endif
