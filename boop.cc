/**
*    @file: boop.cc
*  @author: Brendan Smyers
*    @date: November 20, 2023
*   @brief: The boop class definitions file that handles all the game logic
*
*/

#include "boop.h"
#include <iostream>
#include <algorithm>
#include <cmath>
using namespace std;

// Helper Functions
bool is_upper(char letter) {
    return letter >= 'A' && letter <= 'Z'; 
}

// Returns the distance between two points
double distance(int x1, int y1, int x2, int y2) {
    return sqrt((y2-y1)*(y2-y1)+(x2-x1)*(x2-x1));
}

// Returns a string representation of the row/column pair of the desired (x,y) board position (ie. "c4")
string str_rep(int x, int y) {
    string out;
    out.push_back((char) y + 'a');
    out.push_back((char) x + '1');
    return out;
}

/// PUBLIC FUNCTIONS
// Constructor(s) & Deconstructor
Boop::Boop() {
    restart();
}

Boop::Boop(AI* Player1, AI* Player2, double think_ms) {
    restart();
    P1_AI = Player1;
    P2_AI = Player2;
    think_time_ms = think_ms;

    // Circular AI/Game Dependency
    P1_AI->set_game(this);
    P2_AI->set_game(this);
}

Boop::Boop(const Boop& other) {
    restart();
    // Game State Items
    for(int y = 0; y < SIZE; ++y) {
        for(int x = 0; x < SIZE; ++x) {
            this->board[x][y] = other.board[x][y];
        }
    }
    this->move_state = other.move_state;
    this->move_number = other.move_number;
    this->P1_kit_pieces = other.P1_kit_pieces;
    this->P1_cat_pieces = other.P1_cat_pieces;
    this->P2_kit_pieces = other.P2_kit_pieces;
    this->P2_cat_pieces = other.P2_cat_pieces;

    // AI Items
    this->P1_AI = other.P1_AI;
    this->P2_AI = other.P2_AI;
    this->think_time_ms = other.think_time_ms;
}

Boop& Boop::operator = (const Boop& other) {
    if(this == &other) {
        return *this;
    }
    
    restart();
    // Game State Items
    for(int y = 0; y < SIZE; ++y) {
        for(int x = 0; x < SIZE; ++x) {
            this->board[x][y] = other.board[x][y];
        }
    }
    this->move_state = other.move_state;
    this->P1_kit_pieces = other.P1_kit_pieces;
    this->P1_cat_pieces = other.P1_cat_pieces;
    this->P2_kit_pieces = other.P2_kit_pieces;
    this->P2_cat_pieces = other.P2_cat_pieces;

    // AI Items
    this->P1_AI = other.P1_AI;
    this->P2_AI = other.P2_AI;
    this->think_time_ms = other.think_time_ms;

    return *this;
}

void Boop::make_move(const string& move) {
    int move_y = -1;
    int move_x = -1;


    /// If move_state == MAKE_MOVE
    if(move_state == MAKE_MOVE) {     
        char type  = is_upper(move[0]) ? move[0] + 32  : move[0]; // To lowercase
        move_y = is_upper(move[1]) ? move[1] - 'A' : move[1] - 'a';
        move_x = (int) move[2] - '1';

        /// Add piece to board / Subtract piece from reserve
        if(next_mover() == P1) { // P1
            board[move_x][move_y] = type == 'b' ? P1_KIT : P1_CAT;
            if(type == 'b') { P1_kit_pieces--; } 
            else { P1_cat_pieces--; }
        } else { // P2
            board[move_x][move_y] = type == 'b' ? P2_KIT : P2_CAT;
            if(type == 'b') { P2_kit_pieces--; }
            else { P2_cat_pieces--; }
        }
        
        /// Boop adjacent pieces
        boop_adjacent_pieces(type, move_x, move_y);
        /// Check for three in a row (for CURRENT player only)
        /// If current player makes three in row for opponent, the opponent starts turn by placing a piece, THEN removes three
        if(count_type_in_row(3) > 0) {
            move_state = REMOVE_THREE;
            return;
        }
        /// Check for no remaining bunnies or rabbits
        if((next_mover() == P1 && P1_kit_pieces == 0 && P1_cat_pieces == 0) ||
            (next_mover() == P2 && P2_kit_pieces == 0 && P2_cat_pieces == 0)) {
            move_state = REMOVE_ONE;
            return;
        }
    } else if (move_state == REMOVE_THREE) {
        /// Remove pieces and add rabbits to reserve.
        for(int i = 0; i <= 6; i += 3) {
            move_y = is_upper(move[i]) ? move[i] - 'A' : move[i] - 'a';
            move_x = (int) move[i+1] - '1';

            return_piece(move_x, move_y, true);
        }

    } else if (move_state == REMOVE_ONE) {
        /// Remove the piece and add it to their pool
        move_y = is_upper(move[0]) ? move[0] - 'A' : move[0] - 'a';
        move_x = (int) move[1] - '1';

        return_piece(move_x, move_y, true);
    }
    
    move_state = MAKE_MOVE;
    move_number++;
}

/// Accessible With AI 'Game Reference'
int Boop::moves_completed() const { return move_number; }

Boop::who Boop::last_mover() const { return (move_number % 2 == 1 ? P1 : P2); }

Boop::who Boop::next_mover() const { return (move_number % 2 == 0 ? P1 : P2); }

Boop::who Boop::opposite(Boop::who player) const { return (player == P1) ? P2 : P1; }

Boop::who Boop::winning() const { return (evaluate() > 0 ? P2 : P1); }

Boop* Boop::clone() const {
    return new Boop(*this);
}

void Boop::clone_board(Boop::PieceType board[][SIZE]) const {
    for(int y = 0; y < SIZE; ++y) {
        for(int x = 0; x < SIZE; ++x) {
            board[x][y] = this->board[x][y];
        }
    }
}

void Boop::compute_moves(queue<string>& moves) const {
    if(is_game_over()) { return; }

    if (move_state == MAKE_MOVE) {
        who player = next_mover();
        int num_moves = ((player == P1 && P1_cat_pieces > 0) || (player == P2 && P2_cat_pieces > 0) ? 72 : 36);
        for(int i = 0; i < num_moves; ++i) {
            if(is_legal(ALL_MOVES[i])) { moves.push(ALL_MOVES[i]); }
        }
    } else if (move_state == REMOVE_THREE) {
        for(int y = 0; y < SIZE; ++y) {
            for(int x = 0; x < SIZE; ++x) {
                if(board[x][y] == NONE || !is_friend(x,y)) { continue; }

                if(in_bounds(x+1,y) && in_bounds(x+2,y) && is_friend(x+1,y) && is_friend(x+1,y)) { // Check E
                    moves.push(str_rep(x,y) + ' ' + str_rep(x+1,y) + ' ' + str_rep(x+2,y));
                }
                if(in_bounds(x+1,y-1) && in_bounds(x+2,y-2) && is_friend(x+1,y-1) && is_friend(x+2,y-2)) { // Check SE
                    moves.push(str_rep(x,y) + ' ' + str_rep(x+1,y-1) + ' ' + str_rep(x+2,y-2));
                }
                if(in_bounds(x,y-1) && in_bounds(x,y-2) && is_friend(x,y-1) && is_friend(x,y-2)) { // Check S
                    moves.push(str_rep(x,y) + ' ' + str_rep(x,y-1) + ' ' + str_rep(x,y-2));
                }
                if(in_bounds(x-1,y-1) && in_bounds(x-2,y-2) && is_friend(x-1,y-1) && is_friend(x-2,y-2)) { // Check SW
                    moves.push(str_rep(x,y) + ' ' + str_rep(x-1,y-1) + ' ' + str_rep(x-2,y-2));
                }
            }
        }
    } else if (move_state == REMOVE_ONE) {
        for(int y = 0; y < SIZE; ++y) {
            for(int x = 0; x < SIZE; ++x) {
                if(board[x][y] == NONE || !is_friend(x,y)) { continue; }
                moves.push(str_rep(x,y)); // Add all squares with our pieces on them be possible moves
            }
        }
    }
}

bool Boop::is_game_over() const {
    return (count_type_in_row(3, P1_CAT) > 0) || (count_type_in_row(3, P2_CAT) > 0) || 
            has_eight_cat_down(P1) || has_eight_cat_down(P2);
}

bool Boop::is_legal(const string& move) const {
    int move_x = -1; // Set to -1 so it will be easier to debug if something does go wrong
    int move_y = -1;

    if (move_state == MAKE_MOVE) {
        if(move.length() != 3) { return false; } // Check Length = "ba1"

        char type = is_upper(move[0]) ? move[0] + 32  : move[0]; // To lowercase
        move_y = is_upper(move[1]) ? move[1] - 'A' : move[1] - 'a';
        move_x = (int) move[2] - '1';

        // Check bounds
        if(!in_bounds(move_x, move_y)) { return false; }

        // Check if type is valid
        if(type != 'r' && type != 'b') { return false; }

        // Check if player has enough
        if(type == 'b' && next_mover() == P1 && P1_kit_pieces <= 0)    { return false; }
        if(type == 'r' && next_mover() == P1 && P1_cat_pieces <= 0)    { return false; }
        if(type == 'b' && next_mover() == P2 && P2_kit_pieces <= 0) { return false; }
        if(type == 'r' && next_mover() == P2 && P2_cat_pieces <= 0) { return false; }


        // Check if space is occupied
        if(board[move_x][move_y] != NONE) { return false; }
        
    } else if (move_state == REMOVE_THREE) {
        if(move.length() != 8) { return false; } // Check Length = "a1 a2 a3"
        
        for(int i = 0; i <= 6; i+=3) {
            // Check each input
            move_y = is_upper(move[i]) ? move[i] - 'A' : move[i] - 'a';
            move_x = (int) move[i+1] - '1';

            // Check if pieces are both in range
            if(!in_bounds(move_x, move_y)) { return false; }
            // Check if pieces is friendly
            if(!is_friend(move_x, move_y)) { return false; }
        }
        
        /* I figured out that a three in a row can only occur if the distance between the 3 points are either
            A) a distance of 1, 1, and 2
            or
            B) a distance of sqrt(2), sqrt(2), and 2*sqrt(2)
            So I just checked for that, It seems simpler in the moment and it isn't order sensitive
        */
        
        int Ax = (int) move[1] - '1';
        int Ay = is_upper(move[0]) ? move[0] - 'A' : move[0] - 'a';
        int Bx = (int) move[4] - '1';
        int By = is_upper(move[3]) ? move[3] - 'A' : move[3] - 'a';
        int Cx = (int) move[7] - '1';
        int Cy = is_upper(move[6]) ? move[6] - 'A' : move[6] - 'a';

        double dists[3] = { distance(Ax, Ay, Bx, By),   // AB
                            distance(Bx, By, Cx, Cy),   // BC
                            distance(Cx, Cy, Ax, Ay) }; // CA

        sort(dists, dists + 3); // Sort the list, least to greatest

        const double SQRT_2 = sqrt(2);

        if ((dists[2] != 2          || dists[1] != 1      || dists[0] != 1) &&
            (dists[2] != 2 * SQRT_2 || dists[1] != SQRT_2 || dists[0] != SQRT_2)) {
            return false;
        }

    } else if (move_state == REMOVE_ONE) {
        if(move.length() != 2) { return false; } // Check Length = "a1"
        move_y = is_upper(move[0]) ? move[0] - 'A' : move[0] - 'a';
        move_x = (int) move[1] - '1';
        if(!in_bounds(move_x, move_y)) { return false; }
        if(!is_friend(move_x, move_y)) { return false; }
    }
    return true;
}

int Boop::kittens(Boop::who player) const { 
    if(player == P1) {
        return P1_kit_pieces;
    } else {
        return P2_kit_pieces;
    }
}

int Boop::cats(Boop::who player) const {
    if(player == P1) {
        return P1_cat_pieces;
    } else {
        return P2_cat_pieces;
    }
}

Boop::MoveState Boop::move_type() const { return move_state; }

bool Boop::is_friend(int x, int y) const {
    return next_mover() == P1 ? (board[x][y] == P1_KIT || board[x][y] == P1_CAT) : (board[x][y] == P2_KIT || board[x][y] == P2_CAT);
}

bool Boop::in_bounds(int x, int y) const {
    return (x < SIZE && y < SIZE && x >= 0 && y >= 0);
}

bool Boop::has_eight_cat_down(who player) const {
    if(player == P1    && (P1_kit_pieces != 0 || P1_cat_pieces != 0)) { return false; }
    if(player == P2 && (P2_kit_pieces != 0 || P2_cat_pieces != 0)) { return false; }

    for(int y = 0; y < SIZE; ++y) {
        for(int x = 0; x < SIZE; ++x) {
            if(board[x][y] == NONE) { continue; }
            if(player == P1 && board[x][y] == P1_KIT) {
                return false;
            }
            if(player == P2 && board[x][y] == P2_KIT) {
                return false;
            }
        }
    }
    return true;
}

int Boop::count_type_in_row(int len_of_row, PieceType type) const {
    int count = 0;
    for(int y = 0; y < SIZE; ++y) {
        for(int x = 0; x < SIZE; ++x) {
            // If the current type is not NONE and its not the type we are looking for, continue
            if(type != NONE) {
                if(board[x][y] != type) { continue; }
                // The for loop effectively walks in a direction and aslong as the condition is met, then if the for loop exits without any interrupts we add 1
                for(int i = 1; i < len_of_row; ++i) { // Count E
                    if(!in_bounds(x+i,y) || board[x+i][y] != type) { break; }
                    if(i == len_of_row-1) { count++; } // If all pieces in the row where what we wanted, then we can add a count
                }
                for(int i = 1; i < len_of_row; ++i) { // Count SE
                    if(!in_bounds(x+i,y-i) || board[x+i][y-i] != type) { break; }
                    if(i == len_of_row-1) { count++; }
                }
                for(int i = 1; i < len_of_row; ++i) { // Count S
                    if(!in_bounds(x,y-i) || board[x][y-i] != type) { break; }
                    if(i == len_of_row-1) { count++; }
                }
                for(int i = 1; i < len_of_row; ++i) { // Count SW
                    if(!in_bounds(x-i,y-i) || board[x-i][y-i] != type) { break; }
                    if(i == len_of_row-1) { count++; }
                }
            } else { // If we are in NONE mode, then we see if three friendly pieces are in a row
                if(board[x][y] == NONE || !is_friend(x,y)) { continue; }
                for(int i = 1; i < len_of_row; ++i) { // Count E
                    if(!in_bounds(x+i,y) || !is_friend(x+i,y)) { break; }
                    if(i == len_of_row-1) { count++; } // If all pieces in the row where what we wanted, then we can add a count
                }
                for(int i = 1; i < len_of_row; ++i) { // Count SE
                    if(!in_bounds(x+i,y-i) || !is_friend(x+i,y-i)) { break; }
                    if(i == len_of_row-1) { count++; }
                }
                for(int i = 1; i < len_of_row; ++i) { // Count S
                    if(!in_bounds(x,y-i) || !is_friend(x,y-i)) { break; }
                    if(i == len_of_row-1) { count++; }
                }
                for(int i = 1; i < len_of_row; ++i) { // Count SW
                    if(!in_bounds(x-i,y-i) || !is_friend(x-i,y-i)) { break; }
                    if(i == len_of_row-1) { count++; }
                }
            }
        }
    }
    return count;
}

int Boop::count_tri_pattern(PieceType type) const {
    int count = 0;
    for(int y = 0; y < 4; ++y) {
        for(int x = 0; x < 4; ++x) {
            int c = 0;
            if(type == NONE) {
                if(is_friend(x,y) && x != 0 && y != 0) { ++c; }
                if(is_friend(x+2,y) && x+2 != 5 && y != 0) { ++c; }
                if(is_friend(x,y+2) && x != 0 && y+2 != 5) { ++c; }
                if(is_friend(x+2,y+2) && x+2 != 5 && y+2 != 5) { ++c; }
            } else {
                if(board[x][y] == type) { ++c; }
                if(board[x+2][y] == type) { ++c; }
                if(board[x][y-2] == type) { ++c; }
                if(board[x+2][y-2] == type) { ++c; }
            }

            if(c >= 3) { ++count; }
        }
    }
    return count;
}

void Boop::display_status() const {
    // For each row
    for(int y = 0; y < SIZE; ++y) {
        cout << row_divider << endl;

        // For each 3 thick slice
        for(int slice_num = 0; slice_num < 3; ++slice_num) {

            // For each column
            for(int x = 0; x < SIZE; ++x) {
                cout << '|';
                switch(board[5 - y][x]) { // The (5 - y) thingy is so i can interact with the board[x][y] instead of [y][x] in the rest of my code
                    case NONE:
                        cout << none[slice_num];
                        break;
                    case P1_KIT:
                        cout << P1_Color << bunny[slice_num] << RESET;
                        break;
                    case P1_CAT:
                        cout << P1_Color << rabbit[slice_num] << RESET;
                        break;
                    case P2_KIT:
                        cout << P2_Color << bunny[slice_num] << RESET;
                        break;
                    case P2_CAT:
                        cout << P2_Color << rabbit[slice_num] << RESET;
                        break;
                }
            }
            cout << '|';
            if(slice_num == 1) { // Numbering on side
                cout << " " << 6 - y;
            }
            cout << endl;
        }
    }
    cout << row_divider << endl;
    cout << letters_bar << endl;
    
    // Evaluation Bar
    string winning = "  No particular player";
    string winning_color = RESET;
    int eval = evaluate();
    if(eval != 0) { 
        winning = eval < 0 ? "       Player 1" : "       Player 2"; 
        winning_color = eval < 0 ? P1_Color : P2_Color;
    }
    cout << winning_color << winning << " is currently winning (" << eval << ")" << RESET << endl;

    string P1_B = next_mover() == P1 ? P1_Color : RESET;
    string P2_B = next_mover() == P2 ? P2_Color : RESET;

    cout << P1_B << "+--------------+"                    << RESET << " Type \"b\" or \"r\" " << P2_B << "+--------------+" << RESET << endl;
    cout << P1_B << "|   Player 1   |"                    << RESET << " and column-row  "     << P2_B << "|   Player 2   |" << RESET << endl;
    cout << P1_B << "|  Bunnies: " << P1_kit_pieces << "  |" << RESET << "  (e.g., \"bc4\")  "   << P2_B << "|  Bunnies: " << P2_kit_pieces << "  |" << RESET << endl;
    cout << P1_B << "|  Rabbits: " << P1_cat_pieces << "  |" << RESET << " to move a bunny "     << P2_B << "|  Rabbits: " << P2_cat_pieces << "  |" << RESET << endl;
    cout << P1_B << "+--------------+"                    << RESET << "   or rabbit.    "     << P2_B << "+--------------+" << RESET << endl;
}

/// PRIVATE FUNCTIONS
void Boop::restart() {
    // Game State Items
    for(int y = 0; y < SIZE; ++y) {
        for(int x = 0; x < SIZE; ++x) {
            board[y][x] = NONE;
        }
    }
    move_state = MAKE_MOVE;
    move_number = 0;
    P1_kit_pieces = 8;
    P1_cat_pieces = 0;
    P2_kit_pieces = 8;
    P2_cat_pieces = 0;
}

int Boop::evaluate() const {
    // Return neg if P1 is winning
    // Return pos if P2 is winning
    int eval = 0;

    int P1_bunnies_on_board = 0;
    int P1_rabbits_on_board = 0;
    int P2_bunnies_on_board = 0;
    int P2_rabbits_on_board = 0;

    /// CENTER CONTROL ADVANTAGE EVALUATION
    for(int y = 0; y < SIZE; ++y) {
        for(int x = 0; x < SIZE; ++x) {
            // If the position is NONE continue
            if(board[x][y] == NONE) { continue; }
            
            switch(board[x][y]) {
                case P1_KIT:
                    P1_bunnies_on_board++;
                    eval -= CENTER_INCENTIVE[x][y];
                    break;
                case P1_CAT:
                    P1_rabbits_on_board++;
                    eval -= CENTER_INCENTIVE[x][y];
                    break;
                case P2_KIT:
                    P2_bunnies_on_board++;
                    eval += CENTER_INCENTIVE[x][y];
                    break;
                case P2_CAT:
                    P2_rabbits_on_board++;
                    eval += CENTER_INCENTIVE[x][y];
                    break;
            }
        }
    }

    /// MATERIAL ADVANTAGE EVALUATION
    
    // 16-240
    eval -= ((P1_kit_pieces * 2) + (P1_cat_pieces * 40));
    eval += ((P2_kit_pieces * 2) + (P2_cat_pieces * 40));

    // 64-360
    eval -= ((P1_bunnies_on_board * 3) + (P1_rabbits_on_board * 45));
    eval += ((P2_bunnies_on_board * 3) + (P2_rabbits_on_board * 45));

    /// POSITIONAL ADVANTAGE EVALUATION

    who turn = last_mover();

    // Check for Tri-Patterns
    // Rabbits
    eval -= (count_tri_pattern(P1_CAT) * 10 * (turn == P1    ? 8 : 1));
    eval += (count_tri_pattern(P2_CAT) * 10 * (turn == P2 ? 8 : 1));
    // Bunnies
    eval -= (count_tri_pattern(P1_KIT) * 15 * (turn == P1    ? 8 : 1));
    eval += (count_tri_pattern(P2_KIT) * 15 * (turn == P2 ? 8 : 1));
    // Miss match of bunnies and rabbits
    eval -= (count_tri_pattern() * 5 * (turn == P1    ? 4 : 1));
    eval += (count_tri_pattern() * 5 * (turn == P2 ? 4 : 1));

    // Check for regular patterns
    // This is to take into account that all threes create two twos.
    int P1_Bunny_Threes = count_type_in_row(3, P1_KIT);
    int P2_Bunny_Threes = count_type_in_row(3, P2_KIT);
    // Check for TWO bunny in row (40/5-200/25)
    eval -= (count_type_in_row(2, P1_KIT) * 5 * (turn == P1    ? 8 : 1));
    eval += (count_type_in_row(2, P2_KIT) * 5 * (turn == P2 ? 8 : 1));
    // Check for THREE bunny in row (80/10-400/50)
    eval -= (P1_Bunny_Threes * 10 * (turn == P1    ? 8 : 1));
    eval += (P2_Bunny_Threes * 10 * (turn == P2 ? 8 : 1));
    // Check for TWO rabbit in row (120/15-960/120)
    eval -= (count_type_in_row(2, P1_CAT) * 15 * (turn == P1    && P1_cat_pieces > 0 ? (P2_cat_pieces > 0 ? 4 : 8) * (P1_cat_pieces != 0 ? 0.25 : 1) : 1));
    eval += (count_type_in_row(2, P2_CAT) * 15 * (turn == P2 && P2_cat_pieces > 0 ? (P1_cat_pieces > 0 ? 4 : 8) * (P2_cat_pieces != 0 ? 0.25 : 1) : 1));

    // Winning Conditions - Give huge rewards
    // Check for THREE rabbit in row
    eval = (count_type_in_row(3, P1_CAT) > 0 ? -9999 : eval);
    eval = (count_type_in_row(3, P2_CAT) > 0 ?  9999 : eval);
    // Check for all eight rabbits on the board at the same time
    eval = (has_eight_cat_down(P1)    ? -9999 : eval);
    eval = (has_eight_cat_down(P2) ?  9999 : eval);

    return eval;
}


/// PRIVATE HELPER FUNCTIONS

// Returns true if the 'piece' type is greater than or equal to 'other_piece' otherwise it returns false
bool Boop::can_boop(char piece, PieceType other_piece) const {
    if(other_piece == NONE) { return false; } // Cant push nothing
    if(piece == 'r') { return true; } // Rabbits can push everything
    if(other_piece == P1_KIT || other_piece == P2_KIT) { return true; } // Bunnies can be pushed by anything
    return false; // Bunnies cannot push rabbits
}

// Returns a piece at (x, y) to the players pool, and promotes it if desired
void Boop::return_piece(int x, int y, bool promote) {
    switch(board[x][y]) {
        case P1_KIT:
            if(promote) { P1_cat_pieces++; } else { P1_kit_pieces++; }
            break;
        case P1_CAT:
            P1_cat_pieces++;
            break;
        case P2_KIT:
            if(promote) { P2_cat_pieces++; } else { P2_kit_pieces++; }
            break;
        case P2_CAT:
            P2_cat_pieces++;
            break;
    }
    board[x][y] = NONE; // Empty the square
}

// Given an origin square and type, it will move all legal pieces one space away and return them to the owners pool if they fall off
void Boop::boop_adjacent_pieces(char type, int x, int y) {
    // Check if there is a piece in a given direction
    if(in_bounds(x, y + 1)     && can_boop(type, board[x][y+1]))   { // Check N
        // If next square out is NONE or out of bounds then we can boop
        if(!in_bounds(x, y + 2)) { // If the next square out is out of bounds
            // return the piece at board[x][y+1] to the owners reserve
            return_piece(x, y+1);
        } else if (board[x][y+2] == NONE) { // The next square is at least not occupied
            // Move the boopable piece to the target square and remove it from the origin square
            board[x][y+2] = board[x][y+1];
            board[x][y+1] = NONE;
        }
    }
    if(in_bounds(x + 1, y + 1) && can_boop(type, board[x+1][y+1])) { // Check NE
        if(!in_bounds(x + 2, y + 2)) {
            return_piece(x+1, y+1);
        } else if (board[x+2][y+2] == NONE) {
            board[x+2][y+2] = board[x+1][y+1];
            board[x+1][y+1] = NONE;
        }
    }
    if(in_bounds(x + 1, y)     && can_boop(type, board[x+1][y]))   { // Check E
        if(!in_bounds(x + 2, y)) {
            return_piece(x+1, y);
        } else if (board[x+2][y] == NONE) {
            board[x+2][y] = board[x+1][y];
            board[x+1][y] = NONE;
        }
    }
    if(in_bounds(x + 1, y - 1) && can_boop(type, board[x+1][y-1])) { // Check SE
        if(!in_bounds(x + 2, y - 2)) {
            return_piece(x+1, y-1);
        } else if (board[x+2][y-2] == NONE) {
            board[x+2][y-2] = board[x+1][y-1];
            board[x+1][y-1] = NONE;
        }
    }
    if(in_bounds(x, y - 1)     && can_boop(type, board[x][y-1]))   { // Check S
        if(!in_bounds(x, y - 2)) {
            return_piece(x, y-1);
        } else if (board[x][y-2] == NONE) {
            board[x][y-2] = board[x][y-1];
            board[x][y-1] = NONE;
        }
    }
    if(in_bounds(x - 1, y - 1) && can_boop(type, board[x-1][y-1])) { // Check SW
        if(!in_bounds(x - 2, y - 2)) {
            return_piece(x-1, y-1);
        } else if (board[x-2][y-2] == NONE) {
            board[x-2][y-2] = board[x-1][y-1];
            board[x-1][y-1] = NONE;
        }
    }
    if(in_bounds(x - 1, y)     && can_boop(type, board[x-1][y]))   { // Check W
        if(!in_bounds(x - 2, y)) {
            return_piece(x-1, y);
        } else if (board[x-2][y] == NONE) {
            board[x-2][y] = board[x-1][y];
            board[x-1][y] = NONE;
        }
    }
    if(in_bounds(x - 1, y + 1) && can_boop(type, board[x-1][y+1])) { // Check NW
        if(!in_bounds(x - 2, y + 2)) {
            return_piece(x-1, y+1);
        } else if (board[x-2][y+2] == NONE) {
            board[x-2][y+2] = board[x-1][y+1];
            board[x-1][y+1] = NONE;
        }
    }
}
