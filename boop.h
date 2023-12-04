/**
*    @file: boop.h
*  @author: Brendan Smyers
*    @date: December 03, 2023
*   @brief: Creates, Manages, and Provides functions for AI class to interact with and play Boop
*
*/

#ifndef BOOP_H
#define BOOP_H

#include "AI.h"
#include <queue>
#include <string>
using namespace std;

const string row_divider = "+-------+-------+-------+-------+-------+-------+";
const string letters_bar = "    A       B       C       D       E       F    ";
const string readout =  "+--------------+ Type \"b\" or \"r\" +--------------+\n"
                        "|   Player 1   | and column-row  |   Player 2   |\n"
                        "|  Bunnies: X  |  (e.g., \"bc4\")  |  Bunnies: X  |\n"
                        "|  Rabbits: X  | to move a bunny |  Rabbits: X  |\n"
                        "+--------------+   or rabbit.    +--------------+\n";
const string none[3]  = { "       ", "       ", "       " };
const string bunny[3]  = { " (\\_/) ", " (•_•) ", " / ><\\ " };
const string rabbit[3] = { "(\\(\\   ", "(-.-)  ", "_(\")(\")" };

class Boop {
    public:
        static const int SIZE = 6;
        enum PieceType { NONE, P1_KIT, P1_CAT, P2_KIT, P2_CAT };
        enum MoveState { MAKE_MOVE, REMOVE_THREE, REMOVE_ONE };
        enum who { P1, NEUTRAL, P2 };

        // Constructor(s) & Deconstructor
        Boop();
        Boop(AI* Player1, AI* Player2, double think_ms);
        Boop(const Boop& other);
        Boop& operator = (const Boop& other);
        

        struct Game_Results {
            // Game results
            Boop::who winner = Boop::NEUTRAL;
            double think_time = 100;
            int num_moves = 0;
            double duration = 0;

            // AI results
            double P1_avg_think_time = 0;
            double P2_avg_think_time = 0;
        };


        Game_Results play() {
            Game_Results results;
            results.think_time = think_time_ms;
            string AI_Move;
            Timer timer(think_time_ms);
            int turn_count = 0;
            double duration = 0;

            while(!is_game_over()) {
                queue<string> moves;
                compute_moves(moves);

                timer.start();
                AI_Move = (next_mover() == P1 ? P1_AI->think(moves, timer) : P2_AI->think(moves, timer));
                timer.stop();

                results.duration += timer.elapsedMilliseconds();

                if(next_mover() == P1) {
                    results.P1_avg_think_time = (results.P1_avg_think_time + timer.elapsedMilliseconds()) / 2;
                } else {
                    results.P2_avg_think_time = (results.P2_avg_think_time + timer.elapsedMilliseconds()) / 2;
                }

                make_move(AI_Move);

                if(++turn_count >= turn_limit*2) { 
                    results.num_moves = turn_limit;
                    return results;
                }
            }

            results.num_moves = turn_count / 2;
            results.winner = winning();

            return results;
        }

        void make_move(const string& move);
        
        // Accessible with Game Ref
        /**
         * @brief The number of rounds played
         * 
         * @return An integer representing the number of rounds played
        */
        int moves_completed( ) const;
        
        /**
         * @brief The last player that moved
         * 
         * @return A who enum representing Player 1 or Player 2
        */
        who last_mover( ) const;
        
        /**
         * @brief The current player to move
         * 
         * @return A who enum representing Player 1 or Player 2
        */
        who next_mover( ) const;

        /**
         * @brief The other player
         * @param player A who enum representing the player
         * 
         * @return A who enum representing the opposing team
        */
        who opposite(who player) const;

        /**
         * @brief Who is currently winning based on my evaluate function
         * 
         * @return A who enum representing Player 1 or Player 2
        */
        who winning( ) const;

        /**
         * @brief Clones the current boop game
         * 
         * @return A boop pointer to a new boop game object
         * 
         * @warning Dynamically allocates pointers, You are responsible for memory management
        */
        Boop* clone() const;

        /**
         * @brief Clones the current boop game board
         * 
         * @param board A 6x6 board to copy the game board to
        */
        void clone_board(Boop::PieceType board[][SIZE]) const;

        /**
         * @brief Generates all possible legal moves for the given board state
         * @param moves A queue reference for the function to fill with legal moves
        */
        void compute_moves(queue<string>& moves) const;

        /**
         * @brief Is the game over
         * 
         * @return A bool representing if the current game is over
        */
        bool is_game_over() const;

        /**
         * @brief Determines if the move provided is legal
         * @param move A string reference of the move to check
         * 
         * @return A bool indicating move legality
        */
        bool is_legal(const string& move) const;

        /**
         * @brief The number of kitten pieces a player has
         * @param player A who enum representing the player to check
        */
        int kittens(Boop::who player) const;

        /**
         * @brief The number of cat pieces a player has
         * @param player A who enum representing the player to check
        */
        int cats(Boop::who player) const;

        /**
         * @brief The type of move the current player needs to make
         * 
         * @return A MoveState enum representing the current move state (MAKE_MOVE, REMOVE_THREE, or REMOVE_ONE)
        */
        Boop::MoveState move_type() const;

        /**
         * @brief Determines if the board index is a friendly piece
         * @param x The x index
         * @param y The y index
         * 
         * @return A bool indicating if the piece is yours
        */
        bool is_friend(int x, int y) const;

        /**
         * @brief Checks if a given (x,y) INDEX is inside the board bounds
         * @param x The x index
         * @param y The y index
         * 
         * @return A bool indicating if the point is in bounds
        */
        bool in_bounds(int x, int y) const;

        /**
         * @brief Checks if the given player has all eight of their cat pieces on the board (Winning State)
         * @param player A who enum representing the player to check
         * 
         * @return A bool indicating if the player has all eight cats on board
        */
        bool has_eight_cat_down(Boop::who player) const;

        /**
         * @brief Searches the board and counts the number of occurences of a PieceType in a row of at least
         *        len_of_row long. If NONE PieceType is provided, it will search the board for the number of
         *        occurences of FRIENDLY pieces in a row of at least len_of_row long.
         * @param len_of_row How long of a row you want to match
         * @param type A PieceType enum indicating what type of piece to match, matches friendly pieces if NONE
         * 
         * @return An integer of the number of occurences.
         * 
         * @note This function counts a occurence as soon as the criteria is met, so 1 'three in a row' can also be counted as 2 'two in a row' (overlapping)
        */
        int count_type_in_row(int len_of_row, Boop::PieceType type = NONE) const;

        /**
         * @brief Searches the board and counts the number of occurences of a 'tri-pattern'.
         *        A Tri-Pattern is met when at least three corners of a 3x3 subset grid of 
         *        the board grid are of the desired type.
         * @param type A PieceType enum indicating what type of piece to match, matches friendly piece if NONE
         * 
         * @return An integer of the number of occurences.
        */
        int count_tri_pattern(PieceType type = NONE) const;

    private:
        friend class AI;

        const int CENTER_INCENTIVE[SIZE][SIZE] ={{ 1, 2, 4, 4, 2, 1},
                                                { 2, 5, 7, 7, 5, 2},
                                                { 4, 7,10,10, 7, 4},
                                                { 4, 7,10,10, 7, 4},
                                                { 2, 5, 7, 7, 5, 2},
                                                { 1, 2, 4, 4, 2, 1}};
        const string ALL_MOVES[72] = {"ba1","ba2","ba3","ba4","ba5","ba6",
                                      "bb1","bb2","bb3","bb4","bb5","bb6",
                                      "bc1","bc2","bc3","bc4","bc5","bc6",
                                      "bd1","bd2","bd3","bd4","bd5","bd6",
                                      "be1","be2","be3","be4","be5","be6",
                                      "bf1","bf2","bf3","bf4","bf5","bf6",
                                      "ra1","ra2","ra3","ra4","ra5","ra6",
                                      "rb1","rb2","rb3","rb4","rb5","rb6",
                                      "rc1","rc2","rc3","rc4","rc5","rc6",
                                      "rd1","rd2","rd3","rd4","rd5","rd6",
                                      "re1","re2","re3","re4","re5","re6",
                                      "rf1","rf2","rf3","rf4","rf5","rf6" };

        // Game State Items
        PieceType board[SIZE][SIZE];
        MoveState move_state = MAKE_MOVE;
        int move_number;
        int P1_kit_pieces;
        int P1_cat_pieces;
        int P2_kit_pieces;
        int P2_cat_pieces;

        // AI Items
        AI* P1_AI = nullptr;
        AI* P2_AI = nullptr;
        double think_time_ms;
        const int turn_limit = 1000;

        // Private functions
        void restart();
        int evaluate() const;

        // Helper Functions
        bool can_boop(char type, PieceType enum_type) const;
        void return_piece(int x, int y, bool promote = false);
        void boop_adjacent_pieces(char type, int x, int y);
};

#endif