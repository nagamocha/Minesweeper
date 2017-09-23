#include <iostream>

using namespace std;
/*..........................................................................*/
struct board_object{
    short int no_of_bombs_adjacent_to = 9;
    bool is_bomb = false;
    bool is_visible = false;
};

class Minesweeper{
public:
    Minesweeper(const unsigned int u_no_of_bombs = 20);
    
    //int untouched_pieces = ((board_rows*board_cols) - no_of_pieces_played)
    bool done() const {return no_of_bombs == ((board_rows*board_cols) - no_of_pieces_played);};
    void display(const bool display_bombs) const;
    bool validRow (const int row) const {return (0 <= row && row < board_rows);};
    bool validCol (const int col) const {return (0 <= col && col < board_cols);};
    bool isVisible(const int row, const int col) const {return board[row][col].is_visible;};
    bool play(const int row, const int col);
private:
    void set_bombs(const int no_of_bombs);
    void set_count_of_bombs();
    short int total_count_of_adjacent_bombs (const int row, const int col) const;
    
    void ripple_effect(const int row, const int col);

    

    const unsigned int no_of_bombs;
    unsigned int no_of_pieces_played = 0;
    
    const int board_rows = 10;
    const int board_cols = 10;
    board_object board[10][10];

};
/*..........................................................................*/
/*..........................................................................*/
//                      CONSTRUCTOR METHOD & ASSOCIATED METHODS
Minesweeper::Minesweeper(const unsigned int u_no_of_bombs):
    no_of_bombs(u_no_of_bombs){
    set_bombs(no_of_bombs);
    set_count_of_bombs();
}

void Minesweeper::set_bombs(const int no_of_bombs_to_be_set){
    //check if greater than or less than total size of board
    if (no_of_bombs_to_be_set > board_rows * board_cols ||
        no_of_bombs_to_be_set < 0){
        cout << "invalid number of bombs" << endl;
        exit(1);
    }

    int row, col;
    for (int nth_bomb = 1; nth_bomb < no_of_bombs_to_be_set; nth_bomb++){
        row = rand() % board_rows;//random no of range [0,board_rows)
        col = rand() % board_cols;//random no of range [0,board_cols)
        
        //if bomb was already placed in that slot, we go back and generate
        //new number
        if (board[row][col].is_bomb){
            nth_bomb--;
            continue;
        }
        //set is_bomb field to true
        board[row][col].is_bomb = true;
        
    }
    
}

//all fields that are bombs will have a count of 9
//the rest shall be updated to the count of adjacent bombs
void Minesweeper::set_count_of_bombs(){
    for (int row = 0; row < board_rows; row++){
        for (int col = 0; col < board_cols; col++){
            if (board[row][col].is_bomb) continue;//value left at default 9
            short int no_of_adjacent_bombs = total_count_of_adjacent_bombs(row, col);
            board[row][col].no_of_bombs_adjacent_to = no_of_adjacent_bombs;
        }
    }
}

short int Minesweeper::total_count_of_adjacent_bombs (const int row, const int col) const{
    short int count = 0;
    
    for (int adj_row = row - 1; adj_row <= row + 1; adj_row++){
        if(!validRow(adj_row)) continue; //ie outside board
        for (int adj_col = col - 1; adj_col <= col + 1; adj_col ++){
            if(!validCol(adj_col)) continue;
            if (board[adj_row][adj_col].is_bomb){
                count++;
            }
        }
    }
    
    return count;
}

/*..........................................................................*/
/*..........................................................................*/
//                      DISPLAY METHOD
/*
 Display method takes in a bool argument display_bombs.
 If display_bombs is set to true, ideally when the game is over,
    the function will check if the underlying board object member is
    a bomb via the respective data_field and if so, it prints out
    'b'.
 
 If display_bombs is set to false, ideally during actual gameplay,
    then bombs are masked, i.e '*' instead of 'b' is printed out
 
 For the rest of the board objects, if is_visible is set to true,
    the value printed shall be the no_of bombs the board_object is
    adjacent. Otherwise, '*' is printed out
 
 */
void Minesweeper::display(const bool display_bombs) const {
    for (int row = 0; row < board_rows; row++){
        for (int col = 0; col < board_cols; col++){
            if (display_bombs && board[row][col].is_bomb){
                cout << ' ' << 'b';
                continue;
            }
            if (board[row][col].is_visible)
                cout << ' ' << board[row][col].no_of_bombs_adjacent_to;
            else //ie board object not yet traversed
                cout << ' ' << '*';
        }
        cout << endl;;
    }
}
/*..........................................................................*/
/*..........................................................................*/
//                      GAMEPLAY
//
/*
 NOTE: play supposes the row, col given are valid, it does no error checking
"Takes a row number and a column number, changing the state of the board as appropriate for this 
 move. Returns false if the move results in an explosion" - from prompt
 */
bool Minesweeper::play(const int row, const int col){
    if (board[row][col].is_bomb){
        return false;
    }
    
    no_of_pieces_played++;
    //alter state of board
    //for now, just uncover
    if (board[row][col].no_of_bombs_adjacent_to == 0){
        ripple_effect(row, col);
    }else if (board[row][col].no_of_bombs_adjacent_to > 0){
        board[row][col].is_visible = true;
    }
    return true;
}

//i could use a stack but i'll let recursion take care of the whole mess for me :)
//WARNING, does no error checking, assumes row & col given are valid
//assumes the row,col given is not a bomb and is not adjacent to any bombs ie
//no_of_adjacent_bombs = 0;
void Minesweeper::ripple_effect(const int row, const int col){

    //first things first, set to visible
    board[row][col].is_visible = true;
    
    //if no of adjacent board objects is 0, the board object causes ripple effect
    //if a board object is a bomb, not sent to ripple_effect, is_visible value remains unaltered
    //if it is invalid ie, out of bounds we dont even bother
    // the remaining are set to visible
    //if the no of bombs it is adjacent to is greater than 0
    // we dont send it to ripple effect
    //othewise, the no of adjacent bombs is zero hence we send it to ripple effect
    if (board[row][col].no_of_bombs_adjacent_to == 0){
        for (int adj_row = row - 1; adj_row <= row + 1; adj_row++){
            if(!validRow(adj_row)) continue; //ie outside board
            for (int adj_col = col - 1; adj_col <= col + 1; adj_col ++){
                if(!validCol(adj_col)) continue; //ie outside board
                
                //we dont want to go thru the same tile again
                if(adj_col == col && adj_row == col) continue;
                
                //this means we had already traversed the board object
                //or its a bomb
                if(board[adj_row][adj_col].is_visible ||
                   board[adj_row][adj_col].is_bomb) continue;
                
                //if it is adjacent to a bomb, it doesnt cause a ripple effect
                //however, we reveal how many bombs it's adjacent to
                no_of_pieces_played++;
                if(board[adj_row][adj_col].no_of_bombs_adjacent_to > 0){
                    board[adj_row][adj_col].is_visible = true;
                    continue;
                }
                
                //issa ripple
                ripple_effect(adj_row, adj_col);
            }
        }
    }
}
/*..........................................................................*/
/*..........................................................................*/
int main() {
    srand(rand());
    Minesweeper sweeper;
    // Continue until the only invisible cells are bombs
    
    while (!sweeper.done()) {
        sweeper.display(false); // display the board without bombs
        
        int row_sel = -1, col_sel = -1;
        while (row_sel == -1) {
            // Get a valid move
            int r, c;
            cout << "row? ";
            cin >> r;
            if (!sweeper.validRow(r)) {
                cout << "Row out of bounds\n";
                continue;
            }
            cout << "col? ";
            cin >> c;
            if (!sweeper.validCol(c)) {
                cout << "Column out of bounds\n";
                continue;
            }
            if (sweeper.isVisible(r,c)) {
                cout << "Square already visible\n";
                continue;
            }
            row_sel = r;
            col_sel = c;
        }
        // Set selected square to be visible. May effect other cells.
        if (!sweeper.play(row_sel,col_sel)) {
            cout << "BOOM!!!\n";
            sweeper.display(true);
            // We're done! Should consider ending more "cleanly",
            // eg. Ask user to play another game.
            exit(0);
        }
    }
    // Ah! All invisible cells are bombs, so you won!
    cout << "You won!!!!\n";
    sweeper.display(true); // Final board with bombs shown
     
}
/*..........................................................................*/
