#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "include/board.h"

static std::vector<std::string> split_tokens(const std::string& line) {
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string tok;
    while (iss >> tok) tokens.push_back(tok);
    return tokens;
}

static void apply_moves(Board& board, const std::vector<std::string>& tokens, size_t start) {
    for (size_t i = start; i < tokens.size(); i++) {
        if (tokens[i].size() < 4) continue;
        Move move = board.parse_move(tokens[i]);
        board.make_move(move);
    }
}

int main() {
    Board board;
    board.init_board();

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;

        auto tokens = split_tokens(line);
        if (tokens.empty()) continue;

        const std::string& cmd = tokens[0];

        if (cmd == "uci") {

            std::cout << "id name simple_engine" << std::endl;
            std::cout << "id author el-tahir" << std::endl;
            std::cout << "uciok" << std::endl;

        } else if (cmd == "isready") {

            std::cout << "readyok" << std::endl;
        } else if (cmd == "ucinewgame") {
            board.init_board();


        } else if (cmd == "position") {
            
            if (tokens.size() >= 2 && tokens[1] == "startpos") {
                board.init_board();
                size_t move_idx = 2;
                if (move_idx < tokens.size() && tokens[move_idx] == "moves") {
                    apply_moves(board, tokens, move_idx + 1);
                }
            } else if (tokens.size() >= 8 && tokens[1] == "fen") {
                std::string fen;
                for (size_t i = 2; i < 8; i++) {
                    if (i > 2) fen += " ";
                    fen += tokens[i];
                }
                board.load_fen(fen);
                size_t move_idx = 8;
                if (move_idx < tokens.size() && tokens[move_idx] == "moves") {
                    apply_moves(board, tokens, move_idx + 1);
                }
            }
        } else if (cmd == "go") {
            int depth = 3;
            for (size_t i = 1; i + 1 < tokens.size(); i++) {
                if (tokens[i] == "depth") {
                    try {
                        depth = std::stoi(tokens[i + 1]);
                    } catch (...) {
                        depth = 3;
                    }
                    break;
                }
            }

            auto moves = board.generate_moves();
            if (moves.empty()) {
                std::cout << "bestmove 0000" << std::endl;
            } else {
                Move best = board.get_best_move(depth);
                std::cout << "bestmove " << index_to_square(best.from)
                          << index_to_square(best.to) << std::endl;
            }
        } else if (cmd == "quit") {
            break;
        }
    }

    return 0;
}
