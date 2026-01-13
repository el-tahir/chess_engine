#include <iostream>
#include <array>
#include <vector>
#include <cmath>

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
};

struct Board {
    std::array<Piece, 64> board;

    Color side_to_move;

    void init_board() {
        
        side_to_move = WHITE; 

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

    char get_piece_char(Piece p) {
        static const char piece_chars[] = {
            'P', 'R', 'N', 'B', 'Q', 'K',
            'p', 'r', 'n', 'b', 'q', 'k',
            '.'
        };

        return piece_chars[p];
    }

    void print_board() {
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

    bool is_white_turn() {
        return side_to_move == WHITE;
    }

    void make_move(Move move) {
        Piece p = board[move.from];
        board[move.from] = EMPTY;
        board[move.to] = p;
        if (side_to_move == WHITE) side_to_move = BLACK;
        else side_to_move = WHITE;
    }

    std::vector<Move> generate_moves() {
        std::vector<Move> moves;

        for (int i = 0; i < board.size(); i++) {

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

            if (board[i] == W_PAWN && side_to_move == WHITE) {
                if (i + 8 < board.size() && board[i + 8] == EMPTY) {
                    moves.push_back({i, i + 8});
                }

                if (file != 0 && i + 7 < board.size() &&  B_PAWN <= board[i + 7] && board[i + 7] <= B_KING) {
                    moves.push_back({i, i + 7});
                }
                if (file != 7 && i + 9 < board.size() && B_PAWN <= board[i + 9] && board[i + 9] <= B_KING) {
                    moves.push_back({i, i + 9});
                }

            }

            if (board[i] == B_PAWN && side_to_move == BLACK) {
                if (i - 8 >= 0 && board[i - 8] == EMPTY) {
                    moves.push_back({i, i - 8});
                }

                if (file != 0 && i - 9 >= 0 && W_PAWN <= board[i - 9] && board[i - 9] <= W_KING) {
                    moves.push_back({i, i - 9});
                }
                if (file != 7 && i - 7 >= 0 && W_PAWN <= board[i - 7] && board[i - 7] <= W_KING) {
                    moves.push_back({i, i - 7});
                }
            }

            if ((board[i] == W_KNIGHT && side_to_move == WHITE) || (board[i] == B_KNIGHT && side_to_move == BLACK)) {
                std::vector<int> knight_offsets = {-17, -15, -10, -6, 6, 10, 15, 17};

                int start_file = i % 8;

                for (int offset : knight_offsets) {
                    int target = i + offset;
                    int target_file = target % 8;

                    if (target < 0 || target >= board.size()) continue;
                    if (std::abs(start_file - target_file) > 2) continue;
                    if (side_to_move == WHITE && W_PAWN <= board[target] && board[target] <= W_KING) continue;
                    if (side_to_move == BLACK && B_PAWN <= board[target] && board[target] <= B_KING) continue;

                    moves.push_back({i, target});
                }
            }

            if (!offsets.empty()) {

                for (int offset : offsets) {
                    int target = i;

                    while (true) {
                        target += offset;

                        if (target < 0 || target >= board.size()) break;

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
};


int main() {

    Board board = Board();

    board.board.fill(EMPTY);
    board.board[7] = W_KNIGHT;
    board.print_board();


    std::cout << board.generate_moves().size() << std::endl;


    return 0;
}

