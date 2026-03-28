#include <iostream>
#include <filesystem>
#include <fstream>
#include <unistd.h>
#include "util/misc.hpp"
#include "core/bitboard.hpp"
#include "util/uci.hpp"
#include "engine/engine.hpp"
#include "nnue/probe.h"


using namespace Eyra;

void init_nnue() 
{
    Engine::InitNNUE("nn-b1a57edbea57.nnue", "nn-baff1ede1f90.nnue");
}

int main () 
{

    init_nnue();
    Bitboards::Init();

    std::cout << ENGINE_NAME << ENGINE_VERSION << " by " << ENGINE_AUTHOR << std::endl;

    int pieces[32] = {
        6, 14
    };

    int squares[32] = {
        E1, E8
    };


    UCI::Loop();

    return 0;
}


