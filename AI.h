#ifndef AI_H
#define AI_H

#include "Timer.h"
#include "boop.h"

#include <queue>
#include <string>

class Boop;

class AI {
    public:
        // Constructor & Deconstructor
        AI() { }

        virtual ~AI() { }

        /// Completely Virtual Functions
        /**
         * @brief Decides what move the AI wants to make, MUST BE IMPLEMENTED
         * @param moves A queue of all the possible legal moves
         * @param timer A timer object to keep track of how much time your AI has left to think.
         *              Use the "timer.times_up()" function to check if your AI is out of time
         * @return Returns the string of the desired move the AI wants to make to the Boop game
        */
        virtual std::string think(std::queue<std::string> moves, Timer& timer) = 0;

        /// Internal Boop Usage Only
        /**
         * @brief A function used internally within the Boop constructor to support circular dependency
         * @param game The pointer to the Boop game the AI is currently playing
        */
        void set_game(Boop* game) { this->game = game; }

    protected:
        // Game reference available to use provied helper functions
        const Boop* game = nullptr;
};

#endif