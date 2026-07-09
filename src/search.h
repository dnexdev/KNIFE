#pragma once

#include "position.h"
#include <atomic>
#include <memory>
#include <string_view>
#include <thread>
#include <vector>

struct SearchInfo {
    int depth = 0;
    int movetime = 0;
    int wtime = 0, btime = 0;
    int winc = 0, binc = 0;
    int nodes = 0;
    bool quit = false;
    bool stopped = true;
};

class Search {
public:
    Search(Position& pos, SearchInfo& info) : pos_(pos), info_(info) {}

    void start();
    void stop() { info_.stopped = true; }
    void clear();

private:
    int alphabeta(int alpha, int beta, int depth);

    [[maybe_unused]] Position& pos_;
    SearchInfo& info_;
};

class Engine {
public:
    Engine();

    Position pos;
    SearchInfo info;
    std::unique_ptr<Search> search;
    std::thread searchThread;

    void go();
    void stop();
    void setPosition(std::string_view fen);
    void makeMove(Move m);
};
