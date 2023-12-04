#ifndef BOOPY_AI_H
#define BOOPY_AI_H

#include "../AI.h"

/** 
 * Goal of the AI: 
 *      Play the moves that create the most change in board state.
 *      Play the moves that boop the most pieces.
 */
class Boopy_AI : public AI {
    public:
        Boopy_AI() { }
        std::string think(std::queue<std::string> moves, Timer& timer) override;
    private:
        Boop::PieceType board[Boop::SIZE][Boop::SIZE];
        // Count the number of different squares from the current board state
        int board_difference(Boop* future);
};

std::string Boopy_AI::think(std::queue<std::string> moves, Timer& timer) {
    std::string best_move;
    // Check timer.times_up() between loops as to not go over the time limit
    int boops;
    int most_boops = -1;
    Boop* future;

    game->clone_board(board);

    while(!moves.empty()) {
        future = game->clone();
        future -> make_move(moves.front( ));
        boops = board_difference(future);
        delete future;

        if(boops > most_boops) {
            most_boops = boops;
            best_move = moves.front();
        }

        moves.pop( );
        if(timer.times_up()) { return best_move; }
    }

    return best_move;
}

int Boopy_AI::board_difference(Boop* future) {
    int c = 0;
    Boop::PieceType future_board[6][6];
    future->clone_board(future_board);

    for(int y = 0; y < Boop::SIZE; ++y) {
        for(int x = 0; x < Boop::SIZE; ++x) {
            if(board[x][y] != future_board[x][y]) { ++c; }
        }
    }

    return c;
}

#endif