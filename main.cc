#include "iomanip"
#include "boop.h"
#include "Timer.h"
#include "AI/RandomAI.h"
#include "AI/Winning_AI.h"
#include "AI/Eval_AI.h"
#include "AI/Boopy_AI.h"

int main() {
    int P1_Wins = 0;
    int P2_Wins = 0;
    int Ties = 0;

    Timer timer(0);
    double elapsed_time = 0;

    AI* AI1 = new Boopy_AI;
    AI* AI2 = new Boopy_AI;
    double think_time = 50; // ms

    int num_games = 100;

    for(int i = 1; i <= num_games; ++i) {
        Boop mygame(AI1, AI2, think_time);
        timer.start();
        Boop::who winner = mygame.play();
        timer.stop();
        if(winner == Boop::P1) { 
            P1_Wins++; 
        } else if(winner == Boop::P2) { 
            P2_Wins++; 
        } else { 
            Ties++; 
        }
        elapsed_time += timer.elapsedMilliseconds();

        // Time remaining
        cout << "ETA: " << std::fixed << std::setprecision(2) << (double) ((elapsed_time / i) * (num_games - i))/1000 << " sec   Progress: " << (double) i*100/num_games << "%   Winner: Player " << (winner == Boop::P1 ? "1" : "2") << "\n";
    }

    cout << "Player 1 Won: " << P1_Wins << " games\n";
    cout << "Player 2 Won: " << P2_Wins << " games\n";
    cout << "        Ties: " << Ties << " games\n"; 
    cout << (P1_Wins > P2_Wins ? "Player 1" : "Player 2") << " is ~" << (P1_Wins > P2_Wins ? ((double) (P1_Wins-P2_Wins)/P2_Wins)*100 : ((double) (P2_Wins-P1_Wins)/P1_Wins)*100) << "% better\n";

   return 0;

}