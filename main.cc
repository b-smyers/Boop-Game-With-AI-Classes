#include "boop.h"
#include "RandomAI.h"
#include "Winning_AI.h"
#include "Eval_AI.h"

int main() {
    int P1_Wins = 0;
    int P2_Wins = 0;

    AI* AI1 = new Random_AI;
    AI* AI2 = new Eval_AI;
    double think_time = 100; // ms

    for(int i = 0; i < 100; ++i) {
        Boop mygame(AI1, AI2, think_time);
        Boop::who winner = mygame.play();
        if(winner == Boop::P1) { P1_Wins++; } else { P2_Wins++; }
    }

    cout << "Player 1 Won: " << P1_Wins << " games\n";
    cout << "Player 2 Won: " << P2_Wins << " games\n";
    cout << (P1_Wins > P2_Wins ? "Player 1" : "Player 2") << " is ~" << (P1_Wins > P2_Wins ? ((double) (P1_Wins-P2_Wins)/P2_Wins)*100 : ((double) (P2_Wins-P1_Wins)/P1_Wins)*100) << "% better\n";

   return 0;

}