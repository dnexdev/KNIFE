#include "uci.h"
#include "move.h"
#include <iostream>
#include <sstream>

void UCI::initialize_uci() const {
    std::cout << "id name KNIFE\n";
    std::cout << "id author JustListenToTheSong\n";
    std::cout << "uciok\n";
}

void UCI::parse_position() {
    if (tokens.size() < 2) return;

    size_t index = 1;
    if (tokens[index] == "startpos") {
        engine->setPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        index++;
    } else if (tokens[index] == "fen") {
        std::string fen;
        index++;
        while (index < tokens.size() && tokens[index] != "moves") {
            fen += tokens[index] + " ";
            index++;
        }
        engine->setPosition(fen);
    }

    if (index < tokens.size() && tokens[index] == "moves") {
        index++;
        while (index < tokens.size()) {
            // TODO: Add real move parsing
            Move dummy = buildMove(A2, A4, WHITE_PAWN, QUIET_FLAG);
            engine->makeMove(dummy);
            index++;
        }
    }
}

void UCI::parse_go() {
    engine->info.depth = 5;
    for (size_t i = 1; i < tokens.size(); i++) {
        if (tokens[i] == "depth" && i + 1 < tokens.size()) {
            engine->info.depth = std::stoi(tokens[i + 1]);
        }
    }
    engine->go();
}

void UCI::uci_loop() {
    std::string line;
    while (std::getline(std::cin, line)) {
        tokens.clear();
        std::stringstream ss(line);
        std::string token;
        while (ss >> token) {
            tokens.push_back(token);
        }

        if (tokens.empty()) continue;

        if (tokens[0] == "uci") {
            initialize_uci();
        } else if (tokens[0] == "isready") {
            std::cout << "readyok\n";
        } else if (tokens[0] == "ucinewgame") {
            engine->setPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        } else if (tokens[0] == "position") {
            parse_position();
        } else if (tokens[0] == "go") {
            parse_go();
        } else if (tokens[0] == "stop") {
            engine->stop();
        } else if (tokens[0] == "quit") {
            engine->stop();
            break;
        } else if (tokens[0] == "d") {
            engine->pos.print();
        }
    }
}
