#ifndef HUMAN_AI_H
#define HUMAN_AI_H

#include "../AI.h"

/**
 * Goal of the AI:
 *      Lets humans fight against the AI by passing this AI to the game
*/

class Human_AI : public AI {
    public:
        Human_AI() { }
        std::string think(std::queue<std::string> moves, Timer& timer) override;
};

std::string Human_AI::think(std::queue<std::string> moves, Timer& timer) {
    std::string best_move;
    // Check timer.times_up() between loops as to not go over the time limit
    
    timer.stop();

    game->display_status();


    if(game->move_type() == Boop::REMOVE_THREE) {
        cout << "[ Remove Three ] type three cords (\"c3 d4 e5\")\n";
    } else if(game->move_type() == Boop::REMOVE_ONE) {
        cout << "[ Remove one ] type in one cord (\"c3\")\n";
    }
    cout << "Enter your move: ";
    std::getline(std::cin >> ws, best_move);
    while (!game->is_legal(best_move))
    {
        std::cout << "Illegal move.\n";
        if(game->move_type() == Boop::REMOVE_THREE) {
            cout << "[ Remove Three]\n";
        } else if(game->move_type() == Boop::REMOVE_ONE) {
            cout << "[ Remove one ]\n";
        }
        cout << "Enter your move: ";
        std::getline(std::cin >> ws, best_move);
    }


    return best_move;
}

#endif