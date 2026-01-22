#include <iostream>

#include "include/board.h"

int main() {
    Board board;

    board.load_fen("rnbqkbnr/ppp1pppp/8/3pP3/8/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 1");

    board.print_board();

    for (auto move : board.generate_moves()) {
        std::cout << "(" << index_to_square(move.from) << " -> " << index_to_square(move.to)  << "), ";
    }

    std::cout << std::endl;

    board.make_move({square_to_index("e5"), square_to_index("d6")});

    board.print_board();

    return 0;
}
