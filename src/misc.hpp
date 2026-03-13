#pragma once

namespace Eyra {

constexpr const char* ENGINE_NAME    = "Eyra Chess Engine ";
constexpr const char* ENGINE_VERSION = "v0.2.0";
constexpr const char* ENGINE_AUTHOR  = "GoobusTheNoobus";

constexpr const char* STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

constexpr const char* UCI_OPTIONS[] = {};

constexpr int MAX_CP = 10000;
constexpr int INF    = 30001;
constexpr int MATE_EVAL = 30000;



} // namespace Eyra
