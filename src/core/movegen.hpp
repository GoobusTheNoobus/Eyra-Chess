#pragma once

#include "core/type.hpp"
#include "core/position.hpp"
#include "core/bitboard.hpp"
#include <iostream>


namespace Eyra::MoveGen 
{

void GenerateMoves (const Position& pos, MoveList& list);

} 





