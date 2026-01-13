#include <iostream>
#include <array>

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
};




int main() {

    Board board = Board();

    board.init_board();
    board.print_board();

    board.make_move({12, 28});
    board.print_board();

    return 0;
}

