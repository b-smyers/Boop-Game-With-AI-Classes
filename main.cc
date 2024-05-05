#include <iomanip>
#include "boop.h"
#include "Timer.h"
#include "AI/RandomAI.h"
#include "AI/Winning_AI.h"
#include "AI/Eval_AI.h"
#include "AI/Boopy_AI.h"
#include "AI/Boopy_Alpha_Beta.h"     // BEST AI YET
#include "AI/Minimax_Alpha_Beta_AI.h"
#include "AI/Human_AI.h"  // USE HUMAN AI TO PLAY AGAINST ANOTHER AI

int main() {
    int P1_Wins = 0;
    int P2_Wins = 0;
    int Ties = 0;

    Boop::Game_Results results;

    AI* AI1 = new Random_AI;
    AI* AI2 = new Minimax_Alpha_Beta_AI;
    double think_time = 100; // ms
    Boop mygame(AI1, AI2, think_time);

    int num_games = 100;
    double average_duration = 0;

    for(int i = 1; i <= num_games; ++i) {
        results = mygame.play();

        if(results.winner == Boop::P1) { 
            P1_Wins++; 
        } else if(results.winner == Boop::P2) { 
            P2_Wins++; 
        } else { 
            Ties++; 
        }

        average_duration = (average_duration + results.duration)/2;

        cout << std::fixed << std::setprecision(1) << (double) i*100/num_games << "% |";
        cout << std::fixed << std::setprecision(2) << " ETA: "<< (double) (average_duration * (num_games - i))/1000 << " sec |";
        cout << " W: " << (results.winner == Boop::P1 ? "P1 " : (results.winner == Boop::P2 ? "P2 " : "Tie"));
        cout << " | T: " << results.num_moves;
        cout << std::fixed << std::setprecision(2) << " | Avg Think (ms) [P1: " << results.P1_avg_think_time << "] [P2: " << results.P2_avg_think_time << "]\n";
    }

    cout << "Player 1 Won: " << P1_Wins << " games\n";
    cout << "Player 2 Won: " << P2_Wins << " games\n";
    cout << "        Ties: " << Ties << " games\n"; 
    cout << (P1_Wins > P2_Wins ? "Player 1" : "Player 2") << " is ~" << (P1_Wins > P2_Wins ? ((double) (P1_Wins-P2_Wins)/P2_Wins)*100 : ((double) (P2_Wins-P1_Wins)/P1_Wins)*100) << "% better\n";

   return 0;

}
