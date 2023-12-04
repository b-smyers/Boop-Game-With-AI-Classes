#ifndef WINNING_AI_H
#define WINNING_AI_H

#include "AI.h"
#include "boop.h"

#include <cstdlib>

class Winning_AI : public AI {
    public:
        Winning_AI() { }
        std::string think(std::queue<std::string> moves, Timer& timer) override;
};

std::string Winning_AI::think(std::queue<std::string> moves, Timer& timer) {
    std::string best_move;
    // Check timer.times_up() between loops as to not go over the time limit

    Boop::who me = game->next_mover();

    string winning_moves[72];
    string losing_moves[72];
    int used1 = 0;
    int used2 = 0;

    while(!moves.empty()) {
        Boop* copy = game->clone();
        copy->make_move(moves.front());
        if(game->winning() == me) {
            winning_moves[used1] = moves.front();
            ++used1;
        } else { 
            losing_moves[used2] = moves.front();
            ++used2;
        }
        delete copy;
        moves.pop();
        if(timer.times_up()) { return best_move; }
    }

    srand(rand() % (~(unsigned int)0) * time(NULL)); // Change the seed to ensure a purely random selection

    // If we have winning moves pick a random one, otherwise pick a losing move
    best_move = used1 != 0 ? winning_moves[rand() % used1] : losing_moves[rand() % used2];

    return best_move;
}

#endif