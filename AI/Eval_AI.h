#ifndef EVAL_AI_H
#define EVAL_AI_H

#include "../AI.h"

/**
 * Goal of the AI:
 *      Look SEARCH_LEVELS deep into the future and playing-
 *      the best move according the evaluate function
*/

class Eval_AI : public AI {
    public:
        Eval_AI() { }
        std::string think(std::queue<std::string> moves, Timer& timer) override;
    private:
        const int SEARCH_LEVELS = 2;

        int evaluate(const Boop* position);
        int eval_with_lookahead(int look_ahead, int beat_this, Boop* board);
};

std::string Eval_AI::think(std::queue<std::string> moves, Timer& timer) {
    std::string best_move;
    // Check timer.times_up() between loops as to not go over the time limit
    int value;
    int best_value;
    Boop* future;
    
    // Evaluate each possible legal move, saving the index of the best
    // in best_index and saving its value in best_value.
    best_value = -999999;
    while (!moves.empty( ))
    {
        future = game->clone();
        future -> make_move(moves.front( ));
        value = eval_with_lookahead(SEARCH_LEVELS, best_value, future);
        delete future;
        if (value >= best_value)
        {
            best_value = value;
            best_move = moves.front( );
        }
        moves.pop( );
        if(timer.times_up()) { return best_move; }
    }

    return best_move;
}

int Eval_AI::evaluate(const Boop* position) {
    // Return neg if human is winning
    // Return pos if computer is winning
    int eval = 0;

    /// MATERIAL ADVANTAGE EVALUATION
    
    // 16-240
    eval -= ((position->kittens(position->next_mover()) * 2) + (position->cats(position->next_mover()) * 40));
    eval += ((position->kittens(position->last_mover()) * 2) + (position->cats(position->last_mover()) * 40));

    /// POSITIONAL ADVANTAGE EVALUATION

    Boop::who turn = position->last_mover();

    // Check for Tri-Patterns
    // Rabbits
    eval -= (position->count_tri_pattern(Boop::P1_CAT) * 10 * (turn == Boop::P1 ? 8 : 1));
    eval += (position->count_tri_pattern(Boop::P2_CAT) * 10 * (turn == Boop::P2 ? 8 : 1));
    // Bunnies
    eval -= (position->count_tri_pattern(Boop::P1_KIT) * 15 * (turn == Boop::P1 ? 8 : 1));
    eval += (position->count_tri_pattern(Boop::P2_KIT) * 15 * (turn == Boop::P2 ? 8 : 1));
    // Miss match of bunnies and rabbits
    eval -= (position->count_tri_pattern() * 5 * (turn == Boop::P1 ? 4 : 1));
    eval += (position->count_tri_pattern() * 5 * (turn == Boop::P2 ? 4 : 1));

    // Check for regular patterns
    // This is to take into account that all threes create two twos.
    int P1_Bunny_Threes = position->count_type_in_row(3, Boop::P1_KIT);
    int P2_Bunny_Threes = position->count_type_in_row(3, Boop::P2_KIT);
    // Check for TWO bunny in row (40/5-200/25)
    eval -= (position->count_type_in_row(2, Boop::P1_KIT) * 5 * (turn == Boop::P1 ? 8 : 1));
    eval += (position->count_type_in_row(2, Boop::P2_KIT) * 5 * (turn == Boop::P2 ? 8 : 1));
    // Check for THREE bunny in row (80/10-400/50)
    eval -= (P1_Bunny_Threes * 10 * (turn == Boop::P1 ? 8 : 1));
    eval += (P2_Bunny_Threes * 10 * (turn == Boop::P2 ? 8 : 1));
    // Check for TWO rabbit in row (120/15-960/120)
    eval -= (position->count_type_in_row(2, Boop::P1_CAT) * 15 * (turn == Boop::P1 && position->cats(position->next_mover()) > 0 ? (position->cats(position->last_mover()) > 0 ? 4 : 8) * (position->cats(position->next_mover()) != 0 ? 0.25 : 1) : 1));
    eval += (position->count_type_in_row(2, Boop::P2_CAT) * 15 * (turn == Boop::P2 && position->cats(position->last_mover()) > 0 ? (position->cats(position->next_mover()) > 0 ? 4 : 8) * (position->cats(position->last_mover()) != 0 ? 0.25 : 1) : 1));

    // Winning Conditions - Give huge rewards
    // Check for THREE rabbit in row
    eval = (position->count_type_in_row(3, Boop::P1_CAT) > 0 ? -9999 : eval);
    eval = (position->count_type_in_row(3, Boop::P2_CAT) > 0 ?  9999 : eval);
    // Check for all eight rabbits on the board at the same time
    eval = (position->has_eight_cat_down(Boop::P1)    ? -9999 : eval);
    eval = (position->has_eight_cat_down(Boop::P2) ?  9999 : eval);

    return eval;
}

int Eval_AI::eval_with_lookahead(int look_ahead, int beat_this, Boop* board) {
    std::queue<std::string> moves;   // All possible opponent moves
    int value;             // Value of a board position after opponent moves
    int best_value;        // Evaluation of best opponent move
    Boop* future;          // Pointer to a future version of this game

    // Base case:
    if (look_ahead == 0 || board->is_game_over( ))
    {
        if (board->last_mover( ) == Boop::P2)
            return evaluate(board);
        else
            return -evaluate(board);
    }

    // Recursive case:
    // The level is above 0, so try all possible opponent moves. Keep the
    // value of the best of these moves from the opponent's perspective.
    board->compute_moves(moves);
    // assert(!moves.empty( ));
    best_value = -999999;
    while (!moves.empty( ))
    {
        future = board->clone();
        future -> make_move(moves.front( ));
        value = eval_with_lookahead(look_ahead - 1, best_value, future);
        delete future;
        if (value > best_value)
        {
            best_value = value;
        }
        moves.pop( );
    }

    // The value was calculated from the opponent's perspective.
    // The answer we return should be from player's perspective, so multiply times -1:
    return -best_value;
}

#endif