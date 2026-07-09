#include "search.h"
#include "zobrist.h"
#include <iostream>

Engine::Engine() {
    Zobrist::init();
    pos.clear();
    info.quit = false;
    info.stopped = true;
}

void Search::start() {
    info_.stopped = false;
    std::cout << "info string search started\n";
    for (int d = 1; d <= info_.depth; d++) {
        if (info_.stopped) break;
        int score = alphabeta(-10000, 10000, d);
        std::cout << "info depth " << d << " score cp " << score << "\n";
    }
    std::cout << "bestmove e2e4\n";
}

void Search::clear() {}

int Search::alphabeta(int /*alpha*/, int /*beta*/, int depth) {
    if (depth == 0 || info_.stopped) {
        return 0;
    }
    return 0;
}

void Engine::go() {
    stop();
    info.stopped = false;
    search = std::make_unique<Search>(pos, info);
    searchThread = std::thread(&Search::start, search.get());
}

void Engine::stop() {
    if (!info.stopped) {
        info.stopped = true;
        if (searchThread.joinable()) {
            searchThread.join();
        }
    }
}

void Engine::setPosition(std::string_view fen) {
    pos.parseFen(fen);
}

void Engine::makeMove(Move m) {
    pos.makeMove(m);
}
