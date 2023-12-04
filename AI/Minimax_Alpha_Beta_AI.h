#ifndef MINIMAX_ALPHA_BETA_AI_H
#define MINIMAX_ALPHA_BETA_AI_H

#include "../AI.h"

#include <limits>
#include <iostream>

/**
 * Goal of the AI:
 *      Usies the Evaluate function from Eval_AI along with Minimax + Alpha Beta Pruning.
 *      Can search several layers deep very effeciently
*/

class Minimax_Alpha_Beta_AI : public AI {
    public:
        Minimax_Alpha_Beta_AI() { }
        std::string think(std::queue<std::string> moves, Timer& timer) override;
    private:
        const int SEARCH_DEPTH = 4; // 4 seems to be the most optimal depth, after that it becomes more unstable
        Timer* timer;
        Boop::who me = Boop::NEUTRAL;
        int minimax_alpha_beta(const Boop* position, int depth, int alpha, int beta) const;
        int evaluate(const Boop* position) const;
};

std::string Minimax_Alpha_Beta_AI::think(std::queue<std::string> moves, Timer& timer) {
    std::string best_move;
    // Check timer.times_up() between loops as to not go over the time limit
    
    this->timer = &timer;

    int alpha = std::numeric_limits<int>::min();
    int beta = std::numeric_limits<int>::max();

    int best_score = std::numeric_limits<int>::min();

    Boop* future;
    me = game->next_mover();

    while(!moves.empty()) {
        future = game->clone();
        future -> make_move(moves.front());
        int score = minimax_alpha_beta(future, SEARCH_DEPTH - 1, alpha, beta);
        delete future;

        if(score > best_score) {
            best_score = score;
            best_move = moves.front();
        }
        alpha = std::max(alpha, score);
        moves.pop();
        if(timer.times_up()) { return best_move; }
    }

    return best_move;
}

int Minimax_Alpha_Beta_AI::minimax_alpha_beta(const Boop* position, int depth, int alpha, int beta) const {
    if (depth == 0 || position->is_game_over()) {
        if (position->next_mover() == me) {
            return -evaluate(position);
        } else {
            return evaluate(position);
        }
    }
    
    int eval;
    Boop* future;
    std::queue<std::string> moves;
    position->compute_moves(moves);

    // If its my turn (Maximize)
    if (position->next_mover() == me) {
        int max_eval = std::numeric_limits<int>::min();
        // For each move
        while(!moves.empty()) {
            future = position->clone();
            future->make_move(moves.front());
            // Evaluate the move
            eval = minimax_alpha_beta(future, depth - 1, alpha, beta);
            delete future;
            
            // If the move was better than our max, it becomes are max evaluation-
            // and out lower bound or 'alpha'
            max_eval = std::max(max_eval, eval);
            alpha = std::max(alpha, max_eval);

            // If the move was worse than our lower bound/best move, cut the branch
            if (beta <= max_eval) {
                break;  // Beta cutoff
            }
            moves.pop();
            if(timer->times_up()) { return max_eval; }
        }
        return max_eval;
    } else {
        int min_eval = std::numeric_limits<int>::max();

        while(!moves.empty()) {
            future = position->clone();
            future->make_move(moves.front());

            eval = minimax_alpha_beta(future, depth - 1, alpha, beta);
            delete future;

            min_eval = std::min(min_eval, eval);
            beta = std::min(beta, min_eval);

            if (min_eval <= alpha) {
                break;  // Alpha cutoff
            }
            moves.pop();
            if(timer->times_up()) { return min_eval; }
        }
        return min_eval;
    }
}

int Minimax_Alpha_Beta_AI::evaluate(const Boop* position) const {
    // Return neg if human is winning
    // Return pos if computer is winning
    int eval = 0;

    /// MATERIAL ADVANTAGE EVALUATION
    
    // 16-240
    eval -= ((position->kittens(position->next_mover()) * 2) + (position->cats(position->next_mover()) * 40));
    eval += ((position->kittens(position->last_mover()) * 2) + (position->cats(position->last_mover()) * 40));

    /// POSITIONAL ADVANTAGE EVALUATION

    Boop::who turn = position->next_mover();

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


#endif