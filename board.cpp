#include <algorithm>
#include <cmath>
#include <cctype>
#include <iostream>
#include <stdexcept>

#include "include/board.h"
#include "include/pst_tables.h"

int square_to_index(const std::string& square) {
    if (square.size() != 2) {
        throw std::invalid_argument("square length should be 2 characters");
    }

    int file = square[0] - 'a';
    int rank = square[1] - '1';
    return rank * 8 + file;
}

std::string index_to_square(int index) {
    if (index < 0 || index > 63) {
        throw std::invalid_argument("index must be between 0 and 63");
    }

    int file = index % 8;
    int rank = index / 8;
    std::string square;
    square += static_cast<char>('a' + file);
    square += static_cast<char>('1' + rank);

    return square;
}

void Board::init_board() {
    side_to_move = WHITE;

    castling_rights = CASTLE_WK | CASTLE_WQ | CASTLE_BK | CASTLE_BQ;

    board.fill(EMPTY);

    for (int i = 8; i < 16; i++) board[i] = W_PAWN;
    for (int i = 48; i < 56; i++) board[i] = B_PAWN;

    board[0] = W_ROOK;   board[56] = B_ROOK;
    board[1] = W_KNIGHT; board[57] = B_KNIGHT;
    board[2] = W_BISHOP; board[58] = B_BISHOP;
    board[3] = W_QUEEN;  board[59] = B_QUEEN;
    board[4] = W_KING;   board[60] = B_KING;
    board[5] = W_BISHOP; board[61] = B_BISHOP;
    board[6] = W_KNIGHT; board[62] = B_KNIGHT;
    board[7] = W_ROOK;   board[63] = B_ROOK;
}

char Board::get_piece_char(Piece p) {
    static const char piece_chars[] = {
        'P', 'R', 'N', 'B', 'Q', 'K',
        'p', 'r', 'n', 'b', 'q', 'k',
        '.'
    };

    return piece_chars[p];
}

Piece Board::get_piece_from_char(char c) {
    static const std::string piece_chars = "PRNBQKprnbqk.";

    static const Piece pieces[] = {
        W_PAWN, W_ROOK, W_KNIGHT, W_BISHOP, W_QUEEN, W_KING,
        B_PAWN, B_ROOK, B_KNIGHT, B_BISHOP, B_QUEEN, B_KING,
        EMPTY
    };

    for (int i = 0; i < static_cast<int>(piece_chars.length()); i++) {
        if (piece_chars[i] == c) return pieces[i];
    }

    return EMPTY;
}

int Board::get_piece_value(Piece p) {
    static const int piece_values[] = {
        100, 500, 320, 330, 900, 20000,
        100, 500, 320, 330, 900, 20000,
        0
    };
    return piece_values[p];
}

int Board::evaluate() {
    int total = 0;

    for (int i = 0; i < 64; i++) {
        if (W_PAWN <= board[i] && board[i] <= W_KING) total += get_piece_value(board[i]);
        else total -= get_piece_value(board[i]);

        if (board[i] == W_PAWN) total += pawn_table[i];
        else if (board[i] == B_PAWN) total -= pawn_table[63 - i];

        if (board[i] == W_KNIGHT) total += knight_table[i];
        else if (board[i] == B_KNIGHT) total -= knight_table[63 - i];
    }

    return total;
}

void Board::print_board() {
    // index = (rank * 8) + file
    for (int rank = 7; rank >= 0; rank--) {
        std::cout << (rank + 1) << "  ";
        for (int file = 0; file < 8; file++) {
            std::cout << get_piece_char(board[rank * 8 + file]) << " ";
        }
        std::cout << "\n";
    }
    std::cout << "   a b c d e f g h\n\n";
}

bool Board::is_white_turn() {
    return side_to_move == WHITE;
}

UndoInfo Board::make_move(Move move) {

    UndoInfo undo;
    undo.prev_castling_rights = castling_rights;
    undo.prev_en_passant_square = en_passant_square;
    undo.moved_piece = board[move.from];
    undo.captured_square = move.to;
    undo.captured_piece = board[move.to];

    // if a rook is captured, update castling rights
    if (move.to == square_to_index("h1")) castling_rights &= ~CASTLE_WK;
    if (move.to == square_to_index("a1")) castling_rights &= ~CASTLE_WQ;
    if (move.to == square_to_index("h8")) castling_rights &= ~CASTLE_BK;
    if (move.to == square_to_index("a8")) castling_rights &= ~CASTLE_BQ;

    Piece p = board[move.from];    

    board[move.from] = EMPTY;
    board[move.to] = p;

    

    if (p == W_KING) { // lose castling rights on both sides
        int distance = move.to - move.from;

        if (distance == 2) { // kingside castle
            if (board[square_to_index("h1")] == W_ROOK) {
                board[square_to_index("h1")] = EMPTY;
                board[square_to_index("f1")] = W_ROOK;
            }
        } else if (distance == -2) { // queenside castle
            if (board[square_to_index("a1")] == W_ROOK) {
                board[square_to_index("a1")] = EMPTY;
                board[square_to_index("d1")] = W_ROOK;
            }
        }

        castling_rights &= ~CASTLE_WK;
        castling_rights &= ~CASTLE_WQ;
    }

    if (p == W_ROOK) {
        if (move.from == square_to_index("h1")) castling_rights &= ~CASTLE_WK;
        if (move.from == square_to_index("a1")) castling_rights &= ~CASTLE_WQ;
    }

    if (p == B_KING) {
        int distance = move.to - move.from;

        if (distance == 2) {
            if (board[square_to_index("h8")] == B_ROOK) {
                board[square_to_index("h8")] = EMPTY;
                board[square_to_index("f8")] = B_ROOK;
            }
        } else if (distance == -2) {
            if (board[square_to_index("a8")] == B_ROOK) {
                board[square_to_index("a8")] = EMPTY;
                board[square_to_index("d8")] = B_ROOK;
            }
        }
        castling_rights &= ~CASTLE_BK;
        castling_rights &= ~CASTLE_BQ;
    }

    if (p == B_ROOK) {
        if (move.from == square_to_index("h8")) castling_rights &= ~CASTLE_BK;
        if (move.from == square_to_index("a8")) castling_rights &= ~CASTLE_BQ;
    }

    int new_en_passant = -1;

    if (p == W_PAWN) {
        if (move.to / 8 == 7) board[move.to] = W_QUEEN;
        if (move.to == en_passant_square) {
            undo.captured_square = move.to - 8;
            undo.captured_piece = board[move.to - 8];
            board[move.to - 8] = EMPTY;
        }
        if (move.to - move.from == 16) new_en_passant = move.from + 8;
    }

    if (p == B_PAWN) {
        if (move.to / 8 == 0) board[move.to] = B_QUEEN;
        if (move.to == en_passant_square) {
            undo.captured_square = move.to + 8;
            undo.captured_piece = board[move.to + 8];
            board[move.to + 8] = EMPTY;
        }
        if (move.from - move.to == 16) new_en_passant = move.from - 8;
    }

    en_passant_square = new_en_passant;
    side_to_move = (side_to_move == WHITE) ? BLACK : WHITE;

    return undo;
}

void Board::unmake_move(Move move, const UndoInfo &state){

    // restore turn and state

    side_to_move = (side_to_move == WHITE) ? BLACK : WHITE;
    castling_rights = state.prev_castling_rights;
    en_passant_square = state.prev_en_passant_square;

    // move piece back
    board[move.from] = state.moved_piece;

    // clear destination square
    board[move.to] = EMPTY;

    // undo castling
    if (state.moved_piece == W_KING) {
        int distance = move.to - move.from;
        if (distance == 2) { // white kingside
            board[square_to_index("h1")] = W_ROOK;
            board[square_to_index("f1")] = EMPTY;
        }
        else if (distance == -2) { // white queenside
            board[square_to_index("a1")] = W_ROOK;
            board[square_to_index("d1")] = EMPTY;
        }
    } else if (state.moved_piece == B_KING) {
        int distance = move.to - move.from;
        if (distance == 2) { // black kingside
            board[square_to_index("h8")] = B_ROOK;
            board[square_to_index("f8")] = EMPTY;
        }
        else if (distance == -2) { // black queenside
            board[square_to_index("a8")] = B_ROOK;
            board[square_to_index("d8")] = EMPTY;
        }
    }

    // restore captured piece
    if (state.captured_piece != EMPTY) {
        board[state.captured_square] = state.captured_piece;
    }

}

int Board::find_king(Color side) {
    for (int i = 0; i < 64; i++) {
        if (side == WHITE && board[i] == W_KING) return i;
        if (side == BLACK && board[i] == B_KING) return i;
    }

    return -1; // lol should never happen
}

std::vector<Move> Board::generate_pseudo_moves() {
    std::vector<Move> moves;

    for (int i = 0; i < 64; i++) {
        std::vector<int> offsets;

        if ((board[i] == W_ROOK && side_to_move == WHITE) || (board[i] == B_ROOK && side_to_move == BLACK)) {
            offsets = {-8, 8, -1, 1};
        }
        if ((board[i] == W_BISHOP && side_to_move == WHITE) || (board[i] == B_BISHOP && side_to_move == BLACK)) {
            offsets = {-9, -7, 7, 9};
        }
        if ((board[i] == W_QUEEN && side_to_move == WHITE) || (board[i] == B_QUEEN && side_to_move == BLACK)) {
            offsets = {-9, -7, 7, 9, -8, 8, -1, 1};
        }

        int file = i % 8;
        int rank = i / 8;

        // pawns

        if (board[i] == W_PAWN && side_to_move == WHITE) {
            if (i + 8 < 64 && board[i + 8] == EMPTY) {
                moves.push_back({i, i + 8}); // single push

                if (rank == 1 && board[i + 16] == EMPTY) { // double push
                    moves.push_back({i, i + 16});
                }
            }

            // captures
            if (file != 0 && i + 7 < 64 && ((B_PAWN <= board[i + 7] && board[i + 7] <= B_KING) || (i + 7 == en_passant_square))) {
                moves.push_back({i, i + 7});
            }
            if (file != 7 && i + 9 < 64 && ((B_PAWN <= board[i + 9] && board[i + 9] <= B_KING) || (i + 9 == en_passant_square))) {
                moves.push_back({i, i + 9});
            }
        }

        if (board[i] == B_PAWN && side_to_move == BLACK) {
            if (i - 8 >= 0 && board[i - 8] == EMPTY) {
                moves.push_back({i, i - 8});

                if (rank == 6 && board[i - 16] == EMPTY) {
                    moves.push_back({i, i - 16});
                }
            }

            if (file != 0 && i - 9 >= 0 && ((W_PAWN <= board[i - 9] && board[i - 9] <= W_KING) || (i - 9 == en_passant_square))) {
                moves.push_back({i, i - 9});
            }
            if (file != 7 && i - 7 >= 0 && ((W_PAWN <= board[i - 7] && board[i - 7] <= W_KING) || (i - 7 == en_passant_square))) {
                moves.push_back({i, i - 7});
            }
        }

        // knights

        if ((board[i] == W_KNIGHT && side_to_move == WHITE) || (board[i] == B_KNIGHT && side_to_move == BLACK)) {
            std::vector<int> knight_offsets = {-17, -15, -10, -6, 6, 10, 15, 17};

            int start_file = i % 8;

            for (int offset : knight_offsets) {
                int target = i + offset;
                int target_file = target % 8;

                if (target < 0 || target >= 64) continue;
                if (std::abs(start_file - target_file) > 2) continue;
                if (side_to_move == WHITE && W_PAWN <= board[target] && board[target] <= W_KING) continue;
                if (side_to_move == BLACK && B_PAWN <= board[target] && board[target] <= B_KING) continue;

                moves.push_back({i, target});
            }
        }

        // kings

        if ((board[i] == W_KING && side_to_move == WHITE) || (board[i] == B_KING && side_to_move == BLACK)) {
            std::vector<int> king_offsets = {-9, -8, -7, -1, 1, 7, 8, 9};

            int start_file = i % 8;

            for (int offset : king_offsets) {
                int target = i + offset;
                int target_file = target % 8;

                if (target < 0 || target >= 64) continue;
                if (std::abs(start_file - target_file) > 1) continue;
                if (side_to_move == WHITE && W_PAWN <= board[target] && board[target] <= W_KING) continue;
                if (side_to_move == BLACK && B_PAWN <= board[target] && board[target] <= B_KING) continue;

                moves.push_back({i, target});
            }

            if (side_to_move == WHITE) {
                if (castling_rights & CASTLE_WK)  { // white kingside
                    if (board[square_to_index("h1")] == W_ROOK
                     && board[square_to_index("f1")] == EMPTY
                     && board[square_to_index("g1")] == EMPTY
                     && !is_square_attacked(square_to_index("e1"), BLACK)
                     && !is_square_attacked(square_to_index("f1"), BLACK)
                     && !is_square_attacked(square_to_index("g1"), BLACK))
                     moves.push_back({square_to_index("e1"), square_to_index("g1")});
                }

                if (castling_rights & CASTLE_WQ) {
                    if (board[square_to_index("a1")] == W_ROOK
                     && board[square_to_index("b1")] == EMPTY
                     && board[square_to_index("c1")] == EMPTY
                     && board[square_to_index("d1")] == EMPTY
                     && !is_square_attacked(square_to_index("e1"), BLACK)
                     && !is_square_attacked(square_to_index("d1"), BLACK)
                     && !is_square_attacked(square_to_index("c1"), BLACK))
                     moves.push_back({square_to_index("e1"), square_to_index("c1")});
                }
            } else {
                if (castling_rights & CASTLE_BK)  { // black kingside
                    if (board[square_to_index("h8")] == B_ROOK
                     && board[square_to_index("f8")] == EMPTY
                     && board[square_to_index("g8")] == EMPTY
                     && !is_square_attacked(square_to_index("e8"), WHITE)
                     && !is_square_attacked(square_to_index("f8"), WHITE)
                     && !is_square_attacked(square_to_index("g8"), WHITE))
                     moves.push_back({square_to_index("e8"), square_to_index("g8")});
                }

                if (castling_rights & CASTLE_BQ) {
                    if (board[square_to_index("a8")] == B_ROOK
                     && board[square_to_index("b8")] == EMPTY
                     && board[square_to_index("c8")] == EMPTY
                     && board[square_to_index("d8")] == EMPTY
                     && !is_square_attacked(square_to_index("e8"), WHITE)
                     && !is_square_attacked(square_to_index("d8"), WHITE)
                     && !is_square_attacked(square_to_index("c8"), WHITE))
                     moves.push_back({square_to_index("e8"), square_to_index("c8")});
                }
            }
        }

        if (!offsets.empty()) {
            for (int offset : offsets) {
                int target = i;

                while (true) {
                    target += offset;

                    if (target < 0 || target >= 64) break;

                    int target_file = target % 8;
                    if ((offset == -1 || offset == -9 || offset == 7) && target_file == 7) break; //wrap left
                    if ((offset == 1 || offset == 9 || offset == -7 ) && target_file == 0) break; // wrap right

                    if (board[target] == EMPTY) {
                        moves.push_back({i, target});
                        continue;
                    }

                    if (side_to_move == WHITE &&  B_PAWN <= board[target] && board[target] <= B_KING) {
                        moves.push_back({i, target});
                        break;
                    }
                    if (side_to_move == BLACK && W_PAWN <= board[target] && board[target] <= W_KING) {
                        moves.push_back({i, target});
                        break;
                    }

                    break; // friendly piece
                }
            }
        }
    }

    return moves;
}

std::vector<Move> Board::generate_moves() {
    std::vector<Move> all_moves = generate_pseudo_moves();
    std::vector<Move> legal_moves;

    for (Move move : all_moves) {
        Board temp = *this;
        temp.make_move(move);

        int king_sq = temp.find_king(this->side_to_move);

        if (!temp.is_square_attacked(king_sq, temp.side_to_move)) {
            legal_moves.push_back(move);
        }
    }

    return legal_moves;
}

int Board::search(int depth, int alpha, int beta) {
    if (depth == 0) return evaluate();

    std::vector<Move> moves = generate_moves();

    if (moves.empty()) {
        int king_sq = find_king(side_to_move);

        if (is_square_attacked(king_sq, side_to_move == WHITE ? BLACK : WHITE)) {
            return (side_to_move == WHITE) ? -99999 : 99999; // checkmate
        }

        return 0;
    }

    const bool maximizing = (side_to_move == WHITE);
    int best_score = maximizing ? -99999 : 99999;

    for (auto move : moves) {
        UndoInfo undo = make_move(move);
        int score = search(depth - 1, alpha, beta);
        unmake_move(move, undo);

        if (maximizing) {
            if (score > best_score) best_score = score;
            if (score > alpha) alpha = score;
            if (alpha >= beta) break;
        }

        else {
            if (score < best_score) best_score = score;
            if (score < beta) beta = score;
            if (alpha >= beta) break;
        }
    }

    return best_score;
}

Move Board::get_best_move(int depth) {
    Move best_move = {0, 0};

    std::vector<Move> moves = generate_moves();

    if (moves.empty()) return best_move;

    const bool maximizing = (side_to_move == WHITE);

    {
        UndoInfo undo = make_move(moves[0]);
        int score = search(depth - 1, -99999, 99999);
        unmake_move(moves[0], undo);
        best_move = moves[0];
        int best_score = score;

        for (size_t i = 1; i < moves.size(); i++) {
            Move move = moves[i];
            UndoInfo undo2 = make_move(move);
            int score2 = search(depth - 1, -99999, 99999);
            unmake_move(move, undo2);

            if (maximizing) {
                if (score2 > best_score) {
                    best_score = score2;
                    best_move = move;
                }
            } else {
                if (score2 < best_score) {
                    best_score = score2;
                    best_move = move;
                }
            }
        }
    }

    return best_move;
}

Move Board::parse_move(std::string input) {
    Move move;

    move.from = (input[0] - 'a') + ((input[1] - '1') * 8);
    move.to = (input[2] - 'a') + ((input[3] - '1') * 8);

    return move;
}

bool Board::is_square_attacked(int square, Color side_attacking) {
    int file = square % 8;
    int rank = square / 8;

    // pawn checks
    if (side_attacking == WHITE) {
        // white pawns attack diagonally up,
        // so we check squares below

        if (file != 0 && square - 9 >= 0) { // down-left
            if (board[square - 9] == W_PAWN) return true;
        }

        if (file != 7 && square - 7 >= 0) { // down-right
            if (board[square - 7] == W_PAWN) return true;
        }
    } else {
        // black pawns attack diagonally down,
        // so we check squares above

        if (file != 0 && square + 7 < 64 ) { // up-left
            if (board[square + 7] == B_PAWN) return true;
        }

        if (file != 7 && square + 9 < 64) { // up-right
            if (board[square + 9] == B_PAWN) return true;
        }
    }

    // knight checks

    std::vector<int> knight_offsets = {-17, -15, -10, -6, 6, 10, 15, 17};

    for (int offset : knight_offsets) {
        int target = square + offset;
        int target_file = target % 8;

        if (target < 0 || target >= 64) continue;
        if (std::abs(file - target_file) > 2) continue; // wrap around check

        if (side_attacking == WHITE && board[target] == W_KNIGHT) return true;
        if (side_attacking == BLACK && board[target] == B_KNIGHT) return true;
    }

    // sliding pieces

    // check for rook / queen

    std::vector<int> rook_offsets = {-8, 8, -1, 1};

    for (int offset : rook_offsets) {
        int target = square;

        while (true) {
            target += offset;

            if (target < 0 || target >= 64) break;

            int target_file = target % 8;

            // wrap check for horizontal movement

            if (offset == -1 && target_file == 7) break; // wrap left
            if (offset == 1 && target_file == 0) break; // wrap right

            if (board[target] == EMPTY) continue;

            // hit a piece

            if (side_attacking == WHITE) {
                if (board[target] == W_ROOK || board[target] == W_QUEEN) return true;
            } else {
                if (board[target] == B_ROOK || board[target] == B_QUEEN) return true;
            }

            break; // hit a piece thats not attacking, stop sliding
        }
    }

    // check for bishop / queen

    std::vector<int> bishop_offsets = {-9, -7, 7, 9};

    for (int offset : bishop_offsets) {
        int target = square;

        while (true) {
            target += offset;

            if (target < 0 || target >= 64) break;

            int target_file = target % 8;

            if ((offset == -9 || offset == 7) && target_file == 7) break;
            if ((offset == -7 || offset == 9) && target_file == 0) break;

            if (board[target] == EMPTY) continue;

            if (side_attacking == WHITE) {
                if (board[target] == W_BISHOP || board[target] == W_QUEEN) return true;
            } else {
                if (board[target] == B_BISHOP || board[target] == B_QUEEN) return true;
            }

            break;
        }
    }

    std::vector<int> king_offsets = {-9, -8, -7, -1, 1, 7, 8, 9};

    for (int offset : king_offsets) {
        int target = square + offset;

        int target_file = target % 8;

        if (target < 0 || target >= 64) continue;
        if (std::abs(file - target_file) > 1) continue;

        if (side_attacking == WHITE && board[target] == W_KING) return true;
        if (side_attacking == BLACK && board[target] == B_KING) return true;
    }

    return false;
}

void Board::load_fen(std::string fen) {
    board.fill(EMPTY);

    int rank = 7;
    int file = 0;

    int curr = 0;

    castling_rights = 0;

    while(fen[curr] != ' ') {
        if (fen[curr] == '/' ) {
            rank--;
            file = 0;
        }

        else if (std::isdigit(static_cast<unsigned char>(fen[curr]))) {
            file += fen[curr] - '0';
        }

        else if (std::isalpha(static_cast<unsigned char>(fen[curr]))) {
            board[(rank * 8) + file] = get_piece_from_char(fen[curr]);
            file++;
        }

        curr++;
    }

    curr++; // skip the space

    if (fen[curr] == 'w') {
        side_to_move = WHITE;
    } else if (fen[curr] == 'b') {
        side_to_move = BLACK;
    }

    curr++; // move past 'w' or 'b'

    curr++; // skip another space

    while (fen[curr] != ' ') { // castling rights
        if (fen[curr] == 'K') castling_rights |= CASTLE_WK;
        else if (fen[curr] == 'Q') castling_rights |= CASTLE_WQ;
        else if (fen[curr] == 'k') castling_rights |= CASTLE_BK;
        else if (fen[curr] == 'q') castling_rights |= CASTLE_BQ;

        curr++;
    }

    curr++;

    if (fen[curr] == '-') {
        en_passant_square = -1;
    }
    else {
        std::string en_passant = "";
        en_passant += fen[curr];
        curr++;
        en_passant += fen[curr];

        en_passant_square = square_to_index(en_passant);
    }
}
