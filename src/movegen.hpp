#pragma once

#include "type.hpp"
#include "position.hpp"

namespace Eyra::MoveGen {
    
MoveList GenerateMoves (const Position& pos);
void GenerateMoves (const Position& pos, MoveList list);

void GeneratePawnMoves (const Position& pos, MoveList list);

} // namespace Eyra
