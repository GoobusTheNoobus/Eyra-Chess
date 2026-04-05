#include "util/uci.hpp"
#include "engine/engine.hpp"
#include <thread>






namespace Eyra 
{

    // ======================= Logging Functions =======================

    void UCI::InfoDepth (int depth, int eval, uint64_t nodes, uint64_t elapsed, const std::vector<Move>& pv, int hashfull) 
    {
        std::string score;

        if (std::abs(eval) > MAX_CP) 
        {
            int mate_distance = (MATE_EVAL - std::abs(eval) + 1) / 2;

            if (eval < 0) {
                mate_distance = -mate_distance;
            }

            score = "mate " + std::to_string(mate_distance);
        } else {
            score = "cp " + std::to_string(eval);
        }

        elapsed = std::max<uint64_t>(elapsed, 1);

        std::cout << "info depth " << depth << 
        " score " << score << 
        " nodes " << nodes << 
        " nps " << (nodes * 1000 / elapsed) << // Max by 1 to prevent division by 0
        " time " << elapsed << 
        " hashfull " << hashfull <<
        " pv ";

        for (Move move: pv) 
            std::cout << MoveToString(move) << ' ';
        

        std::cout << std::endl;

    }

    void UCI::InfoString (const std::string& message) {
        std::cout << "info string " << message << std::endl;
    }

    void UCI::BestMove (Move move, Move ponder) {
        std::cout << "bestmove " << MoveToString(move) << std::endl; // TO-DO: Ponder
    }

    // ======================= UCI Input Functions =======================

    namespace 
    {

    std::thread search_thread;

    void Stop() 
    {
        Engine::search_info.stop.store(true, std::memory_order_relaxed);

        if (search_thread.joinable()) {
            search_thread.join();
        }
    }
    
    } // namespace anonymous

    void UCI::ParsePosition (const std::string& command) 
    {

        Stop();

        std::istringstream iss(command);

        std::string token;

        // Skip "position"
        iss >> token;

        iss >> token;

        if (token == "startpos") 
        {
            Engine::position.ParseFEN(STARTING_FEN);

            iss >> token;

            if (token == "moves") 
            {
                while (iss >> token) 
                {
                    Engine::position.MakeMove(token);
                }
            }
        } else if (token == "fen") 
        {
            std::string fen;

            while ((iss >> token) && token != "moves") 
            {
                fen += token + " ";
            }

            try 
            {
                Engine::position.ParseFEN(fen);
            } catch (std::exception e) 
            {
                InfoString("INVALID FEN");
            }

            if (token == "moves") 
            {
                while (iss >> token) {
                    Engine::position.MakeMove(token);
                }
            }
        }
        

    }

    void UCI::ParseGoCommand (const std::string& command) 
    {

        Stop();

        std::istringstream iss(command);
        std::string token;

        int depth = 32; // Max depth if invalid
        int movetime = 0;
        int movestogo = 40;

        int wtime = 0;
        int btime = 0;

        int winc = 0;
        int binc = 0;

        // Skip "go"
        iss >> token;

        while (iss >> token) 
        {
            if (token == "depth") 
                iss >> depth;
            else if (token == "movetime") 
                iss >> movetime;
            else if (token == "wtime") 
                iss >> wtime;
            else if (token == "btime") 
                iss >> btime;
            else if (token == "winc") 
                iss >> winc;
            else if (token == "binc") 
                iss >> binc;
            else if (token == "movestogo")
                iss >> movestogo;
            else if (token == "perft") 
            {
                int perft_depth = 0;
                iss >> perft_depth;

                Engine::PerftDivide(perft_depth);

                return;
            }
        }

        int time_limit = 0;

        if (movetime > 0) 
            time_limit = movetime;
        else if (wtime > 0 || btime > 0) 
        {

            int our_time = Engine::position.SideToMove() == WHITE ? wtime: btime;
            int our_inc  = Engine::position.SideToMove() == WHITE ? winc: binc;

            time_limit = std::min(our_time / 20 + our_inc / 2, our_time);
        }

        // Launch Search in search thread while main thread stays in the loop
        // Allows you to type stuff during search
        search_thread = std::thread([depth, time_limit]()
        {
            Engine::Go(depth, time_limit);
        });
    
    }

    void UCI::PrintPosition() 
    {
        std::cout << Engine::position << std::endl;
    }

    // Main UCI Loop 
    void UCI::Loop() 
    {
        while (true) {
            std::string string;
            std::getline(std::cin, string);

            if (std::all_of(string.begin(), string.end(), [](unsigned char c) 
            {
                return std::isspace(c);
            })) continue;
            


            std::istringstream iss(string);
            std::string command;

            iss >> command;

            if (command == "go") 
                ParseGoCommand(string);
            
            
            else if (command == "position") 
                ParsePosition(string);
            

            else if (command == "stop") 
                Stop();
            
            
            else if (command == "uci") {
                std::cout << "id name EyraChess " << ENGINE_VERSION << "\n";
                std::cout << "id author " << ENGINE_AUTHOR << "\n";
                std::cout << "uciok" << std::endl;
            } 
            
            else if (command == "isready") 
                std::cout << "readyok" << std::endl;
            
            
            else if (command == "ucinewgame") 
            {
                Stop();
                Engine::ResetKillers();
                Engine::tt.Clear();
            } 
            
            else if (command == "d") 
                PrintPosition();

            else if (command == "eval")
            {
                PrintPosition();

                std::cout << "Classical: " << Engine::Evaluate(Engine::position) << std::endl;
                std::cout << "NNUE:       NaN" << std::endl;
            }

            else if (command == "quit") 
            {
                Stop();
                break;
            }
            
            else 
                InfoString("invalid command");
            
        }
        
    }
 
} // namespace Eyra
