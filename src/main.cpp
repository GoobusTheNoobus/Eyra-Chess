#include <iostream>
#include <filesystem>
#include <fstream>
#include <unistd.h>
#include "util/misc.hpp"
#include "core/bitboard.hpp"
#include "util/uci.hpp"
#include "engine/engine.hpp"


using namespace Eyra;

int main () 
{

    Bitboards::Init();

    std::cout << ENGINE_NAME << ENGINE_VERSION << " by " << ENGINE_AUTHOR << std::endl;

    


    UCI::Loop();

    return 0;
}


