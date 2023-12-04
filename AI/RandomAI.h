#ifndef RANDOMAI_H
#define RANDOMAI_H

#include "../AI.h"

#include <cstdlib>

/**
 * Goal of the AI:
 *      Play random moves
*/

class Random_AI : public AI {
    public:
        Random_AI() { };
        std::string think(std::queue<std::string> moves, Timer& timer) override;
};

std::string Random_AI::think(std::queue<std::string> moves, Timer& timer) {
    // Change seed each time to increase randomness
    srand(rand()%(~(unsigned int)0) * time(NULL));

    for(int i = 0; i < rand() % moves.size() && !timer.times_up(); ++i)
        moves.pop();
    return moves.front(); // Returns a random move
}

#endif