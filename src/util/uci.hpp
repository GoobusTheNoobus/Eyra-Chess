#pragma once
#include "core/type.hpp"
#include "util/misc.hpp"
#include "core/move.hpp"
#include <sstream>
#include <iostream>

namespace Eyra::UCI {

    void InfoDepth  (int depth, int score, uint64_t nodes, uint64_t elasped, const std::vector<Move>& pv);
    void InfoString (const std::string& message);

    void BestMove (Move move, Move ponder=0);

    void Loop ();

    void ParsePosition (const std::string& command);
    void ParseGoCommand(const std::string& command);

    void PrintPosition ();
    
} // namespace Eyra::UCI
