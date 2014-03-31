#ifndef MAIN_H
#define MAIN_H


//    Function declarations
int main(int argc, char* argv[]);
void init(void);

// Place your glut callback declarations here:
void reshape(int w, int h);
void display(void);
void alarm(int alarmnumber);
void keyfunc(unsigned char key, int x, int y);
void mousefunc(int button, int state,  int x, int y);


//global functions for gameplay
//clears 'doubles' and 'jumps' and 'activecell' for when a cell is deselected or a move is made
bool nextPlayer();//move to the next player
void clearMoves();
//disable the highlights on the cells
void hideMoves();
//set the highlights on all cells mentioned in the 'doubles' and 'jumps' lists
void showMoves(unsigned char playerNr);
//end the game and show the winner
void endGame();
#endif
