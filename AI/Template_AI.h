#ifndef TEMPLATE_AI_H
#define TEMPLATE_AI_H

#include "../AI.h"

/**
 * Goal of the AI:
 *      
*/

class Template_AI : public AI {
    public:
        Template_AI() { }
        std::string think(std::queue<std::string> moves, Timer& timer) override;
};

std::string Template_AI::think(std::queue<std::string> moves, Timer& timer) {
    std::string best_move;
    // Check timer.times_up() between loops as to not go over the time limit
    
    

    return best_move;
}

#endif