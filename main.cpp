#include <algorithm>
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
        Move move = parse_move(tokens[i]);
        make_move(board, move);
    }
}

static void run_uci_loop(Board& board) {
    constexpr int kDefaultDepth = 3;
    constexpr int kMaxSearchDepth = 4;

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;

        auto tokens = split_tokens(line);
        if (tokens.empty()) continue;

        const std::string& cmd = tokens[0];

        if (cmd == "uci") {

            std::cout << "id name simple_engine" << std::endl;
            std::cout << "id author el-tahir" << std::endl;
            std::cout << "option name Hash type spin default 16 min 1 max 1024" << std::endl;
            std::cout << "uciok" << std::endl;

        } else if (cmd == "isready") {

            std::cout << "readyok" << std::endl;
        } else if (cmd == "ucinewgame") {
            init_board(board);


        } else if (cmd == "position") {

            if (tokens.size() >= 2 && tokens[1] == "startpos") {
                init_board(board);
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
                load_fen(board, fen);
                size_t move_idx = 8;
                if (move_idx < tokens.size() && tokens[move_idx] == "moves") {
                    apply_moves(board, tokens, move_idx + 1);
                }
            }
        } else if (cmd == "setoption") {
            // option values are currently ignored, but command support keeps UCI clients happy.
            continue;
        } else if (cmd == "go") {
            int depth = kDefaultDepth;
            for (size_t i = 1; i + 1 < tokens.size(); i++) {
                if (tokens[i] == "depth") {
                    try {
                        depth = std::stoi(tokens[i + 1]);
                    } catch (...) {
                        depth = kDefaultDepth;
                    }
                    break;
                }
            }
            depth = std::clamp(depth, 1, kMaxSearchDepth);

            auto moves = generate_moves(board);
            if (moves.empty()) {
                std::cout << "bestmove 0000" << std::endl;
            } else {
                Move best = get_best_move(board, depth);
                std::cout << "bestmove " << index_to_square(best.from)
                          << index_to_square(best.to) << std::endl;
            }
        } else if (cmd == "quit") {
            break;
        }
    }
}

static void run_cli_loop(Board& board) {
    init_board(board);

    while (true) {
        print_board(board);

        auto moves = generate_moves(board);
        if (moves.empty()) {
            int king_sq = find_king(board, board.side_to_move);
            Color attacker = (board.side_to_move == WHITE) ? BLACK : WHITE;
            if (is_square_attacked(board, king_sq, attacker)) {
                std::cout << "Checkmate." << std::endl;
            } else {
                std::cout << "Stalemate." << std::endl;
            }
            return;
        }

        if (is_white_turn(board)) {
            std::cout << "Your move (e2e4, 'fen ...', 'startpos', or quit): ";
            std::string input;
            if (!std::getline(std::cin, input)) return;
            if (input == "quit") return;
            if (input == "startpos") {
                init_board(board);
                continue;
            }
            if (input.rfind("fen ", 0) == 0) {
                load_fen(board, input.substr(4));
                continue;
            }
            if (input.size() < 4) {
                std::cout << "Invalid move format." << std::endl;
                continue;
            }

            Move move = parse_move(input);
            bool legal = false;
            for (const auto& m : moves) {
                if (m == move) {
                    legal = true;
                    break;
                }
            }
            if (!legal) {
                std::cout << "Illegal move." << std::endl;
                continue;
            }

            make_move(board, move);
        } else {
            moves = generate_moves(board);
            if (moves.empty()) {
                int king_sq = find_king(board, board.side_to_move);
                Color attacker = (board.side_to_move == WHITE) ? BLACK : WHITE;
                if (is_square_attacked(board, king_sq, attacker)) {
                    std::cout << "Checkmate." << std::endl;
                } else {
                    std::cout << "Stalemate." << std::endl;
                }
                return;
            }
            Move best = get_best_move(board, 3);
            std::cout << "Engine plays "
                      << index_to_square(best.from)
                      << index_to_square(best.to) << std::endl;
            make_move(board, best);
        }
    }
}

int main(int argc, char** argv) {
    Board board;
    init_board(board);

    if (argc > 1 && std::string(argv[1]) == "cli") {
        run_cli_loop(board);
        return 0;
    }

    run_uci_loop(board);

    return 0;
}
