#pragma once

#include "type.hpp"
#include "position.hpp"
#include "bitboard.hpp"
#include <iostream>









namespace Eyra::MoveGen {



// 2 overloads, one returns a list, another writes into a prexisting list
void GenerateMoves (const Position& pos, MoveList& list);
MoveList GenerateMoves (const Position& pos);




} // namespace Eyra::MoveGen





