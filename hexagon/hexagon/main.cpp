////////////////////////////////////////////////////////////
//
// File: main.cpp
// Author: Sven Goossens
// 
//	Hexagon, Jeroen van Oorschot 2014
// 
//
///////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include "glut.h"			 // glut header file. Never include glut before stdlib.h!
#include <string>
#include <iostream>
#include <fstream>
#define _USE_MATH_DEFINES	// Signal math.h that we would like defines like M_PI
#include <math.h>			// Might come in usefull for cosine functions and stuff like that

#include "main.h"			// Function declarations and some definitions
#include "dlist.h"			// base class of the drawtools
#include "drawtools.h"      // contains all you need to draw stuff
#include "hex.h"

#define CELL_DEFAULT 0
#define CELL_SELECTED 1
#define CELL_DOUBLE 2
#define CELL_JUMP 3

#define CELL_DISABLED 0
#define CELL_FREE 1
#define CELL_WHITE 2

using namespace std;

//    Global variables
int width = 700, height = 700; //inital window size
char keytext[50];
bool editMode = false, showCellNr=true; //edit mode and show numbers on cells
dlist drawList; //multiple drawlists for independent blocks of items
game* gm; //pointer to the instance of the game
cell* doubles[6];
cell* jumps[12];
text* lines[2];
char activePlayer = 0;
float green[3] = { 0,0.5,0 };
float red[3] = { 0.5, 1, 1 };

cell* activeCell; //selected cell
//players = new player*[numPlayers];

//EDIF++ parser
//this draws the edif file on the screen.
void Eparse(const char* filename){
	FILE* fh;
	fh = fopen(filename, "r");
	if (!fh){
		cout << "File cannot be read" << endl;
		return;
	}
	char str[100];

	float p1[2], p2[2], clr[3];
	int lw;

	while (fgets(str, 1024, fh) != NULL){
		if (sscanf(str, ".line %f %f %f %f %f %f %f %i\n", &p1[0], &p1[1], &p2[0], &p2[1], &clr[0], &clr[1], &clr[2], &lw) == 8){
			new line(&drawList, p1, p2, clr, lw);
		}
		if (sscanf(str, ".pixel %f %f %f %f %f\n", &p1[0], &p1[1], &clr[0], &clr[1], &clr[2]) == 5){
			new pixel(&drawList, p1, clr);
		}
		if (sscanf(str, ".text %f %f %f %f %f \"%[^\n]", &p1[0], &p1[1], &clr[0], &clr[1], &clr[2], &str) == 6){
			str[strlen(str) - 1] = 0;
			new text(&drawList, p1, clr, str);
		}
		if (sscanf(str, ".drawing \"%[^\n]", &str) == 1){
			str[strlen(str) - 1] = 0;
			glutSetWindowTitle(str);

		}
	}
	fclose(fh);

}

//init function
/*
      void init(void)
      This function is called when glut is initialized. Use it to initialize
      variables and setup things that have to be done BEFORE the actual
      picture is drawn.
*/
void init(void)
{
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);	// Double buffering, RGB format
	glutInitWindowSize(width,height);				// 1024 x 500 pixel window
	glutInitWindowPosition(0,0);					// Window appear in the top-left corner of the screen
	glutCreateWindow("Starting hexagon....");		// Window title
	glutDisplayFunc(display);						// Main display function
	glutReshapeFunc(reshape);						// Reshape function, called when the window resizes

	glutKeyboardFunc(keyfunc);
	glutMouseFunc(mousefunc);
	// You might want to remove these after you are done testing with them
	/*glutTimerFunc(1000, alarm, 112); // Example Timer functions
	glutTimerFunc(1500, alarm, 1);
	glutTimerFunc(2000, alarm, 2);*/
	
	//look better by enabling anti-aliasing
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	 //to improve performance, don't draw backsides, because the game is 2d.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glClearColor(1.0, 1.0, 1.0, 1.0);   // white background
	
	/*
		The following funtions setup the view of the screen.
		OpenGL actually creates a 3D world, which is displayed trough a
		viewport(the glut window). The way pixels are projected onto
		this viewport is setup here.
		Keep them like this if you don't need fancy options
	*/
	glMatrixMode(GL_PROJECTION);        // Next lines will adjust the projection matrix
	glLoadIdentity();                   // Reset matrix, returns viewport to origin
											// and undo possible rotations

	// Set the amount of points on the drawing grid
	// Also determines the viewport: we are looing at the rectangle between
	// (0,0) and (WIDTH,HEIGHT)
	gluOrtho2D(0.0, (float)width, 0.0, (float)height);
	glMatrixMode(GL_MODELVIEW);

}

//---------------------------------------------------------------------------
/*
void void keyfunc(unsigned char key, int x, int y)
called when a key is pressed
*/
void keyfunc(unsigned char key, int x, int y){
	y = height - y; //remap
	sprintf(keytext, "%c, %d, %d\n", key, x, y);
	//numeric key, a third way to set the content of the cell. Hover over a cell and press a number to change its content
	if (key > 47 && key < 58 && editMode){ 
		if (cell* clickedCell = gm->checkClick(x, y)){ //if an cell is clicked
			key -= 48;
			if (key < (gm->getNumPlayers() + 3)){
				clickedCell->setContent(key);
			}
		}
	}
	else{
		switch (key){
		case 'o': //open a board
		{
					 /* float ln2p[2] = { 20, gm->_boardHeight * 2 * gm->_hexSize + 60 };
					  lines[0] = new text(&drawList, ln2p, green, "Look at your command window");
					  glutPostRedisplay();*/

					  string name;
					  cout << "Please give a filename: " << endl;
					  cin >> name;
					  if (gm->openBoard(name)){
						  cout << "File loaded" << endl;
					  }
					  else{
						  cout << "Failed to load file" << endl;
					  }
					  activePlayer = 0;
					  delete lines[0];
					  break;
		}
			
		case 's': //store a board to a file
		{
					 /* float ln2p[2] = { 20, gm->_boardHeight * 2 * gm->_hexSize + 60 };
					  lines[0] = new text(&drawList, ln2p, red, "Look at your command window");
					  glutPostRedisplay();*/

					  string name;
					  cout << "Please give a filename: " << endl;
					  cin >> name;
					  gm->storeBoard(name);
					  delete lines[0];
					  break;
		}
		case 'n': //make new board with specified size and number of players
		{
					/*  float ln2p[2] = { 20, gm->_boardHeight * 2 * gm->_hexSize + 60 };
					  lines[0] = new text(&drawList, ln2p, red, "Look at your command window");
					  glutPostRedisplay();*/
					  unsigned short bs, bh, np;
					  cout << "board size: " << endl;
					  cin >> bs;
					  cout << "board heigth:" << endl;
					  cin >> bh;
					  cout << "number of players: " << endl;
					  cin >> np;
					  gm->newBoard(bs, bh, np);
					  activePlayer = 0;
					  delete lines[0];
					  break;
		}
			case 'c': //clear the board, set all content to off (0)
				gm->newBoard(gm->_boardSize, gm->_boardHeight, gm->_numPlayers);
				activePlayer = 0;
				break;
		case 'p': //pass. 
			nextPlayer();
			break;
		case 'a': //automated move
		{
		    cell* neighborList[6];
			cell* jumpSrc; //if the move was a jump, store the source of the jump here

			cell* clickedCell = gm->getPlayer(activePlayer)->checkMoves(neighborList, &jumpSrc); //get the best cell to move to
			if (clickedCell == nullptr){ //no possible moves, possibly end of game
				cout << "Not possible to automate move" << endl;
			}
			else{
				//make the move and take the cells over
				clickedCell->takeOver(neighborList, activePlayer, jumpSrc); //take them over
				//if the user already clicked a cell before pressing A, clear those moves now
				hideMoves();
				clearMoves();
				//let the next player play
				nextPlayer();
			}
			break;
		}
		case 'e':{ //enable or disable editmode
					 editMode = !editMode;
					 if (editMode){ //show the editmode text on top.
						 float ln1p[2] = { 20, gm->_boardHeight * 2 * gm->_hexSize + 20 };
						 lines[1] = new text(&drawList, ln1p, green, "Edit mode ON");
					 }
					 else{
						 delete lines[1];
					 }
					 break;
		}
		case 'q': //end this game (quit)
		{
					  endGame();
					  break;
		}
		case 'd':  //open default board
		{
					   gm->openBoard("default");
					   activePlayer = 0;
					   break;
		}
		 case 'i': //show index numbers on cells
		 {
					showCellNr = !showCellNr;
					gm->showNr(showCellNr);
					break;
		 }
		default:
			cout << "no action binded to this key." << endl;
			break;
		}
	}
	glutPostRedisplay();
}

//called when mousebutton pressed
void mousefunc(int button, int state, int x, int y){
	y = height - y; //remap 
	if (button == 0){ //left mouse button
		if (state == 0){ //when pressed
			if (cell* clickedCell = gm->checkClick(x, y)){ //if an cell is clicked
				if (editMode){  //edit mode
					clickedCell->setContent((clickedCell->getContent() + 1) % (gm->getNumPlayers() + 3)); //set content to next possibility
				}				//play mode
				else{ //not edit mode, play the game
					if (clickedCell == activeCell){ //clicked second time, deselect the cell
						hideMoves();  //hide the highlights on the cell
						clearMoves(); //clear the list of possible moves
					}
					else if (activeCell){ //cell does not belong to this user, check if a move can be made to here
						cell* neighborList[6];
						int addedCells = 0; //number of cells added to this user due to this move
						for (int i = 0; i < 6; i++){
							if (clickedCell == doubles[i]){ //cell can be doubled to this cell
								//make the move
								clickedCell->getNeighbors(neighborList);  //get all his neighbors
								addedCells = clickedCell->checkTakeOver(neighborList, activePlayer); //check who of the neighbors can be taken over
								clickedCell->takeOver(neighborList, activePlayer, nullptr); //take them over

								//set next player
								nextPlayer();

								goto endMove; //use a goto to prevent the second loop from executing
							}
						}
						for (int i = 0; i < 12; i++){
							if (clickedCell == jumps[i]){ //cell can be doubled to this cell
								//make the move
								clickedCell->getNeighbors(neighborList);  //get all his neighbors
								addedCells = clickedCell->checkTakeOver(neighborList, activePlayer); //check who of the neighbors can be taken over
								clickedCell->takeOver(neighborList, activePlayer, activeCell); //take them over

								//set next player
								nextPlayer();
			
								break;
							}
						}
					endMove:
						//deselect. (After a move or after an wrong click.)
						hideMoves(); //disable the borders
						clearMoves();//clear 'doubles' and 'jumps' and activeCell

					}
					else if (clickedCell->getContent() - 3 == activePlayer){ // if the cell belongs to the active user
						activeCell = clickedCell; //set this cell as active

						clickedCell->getDoubles(doubles); //fill the 'doubles' list
						clickedCell->getJumps(jumps);     //fill the 'jumps' list
						showMoves(activePlayer); //set the highlights of the clickedCell, jumps and doubles to the apropriate values
					}

					else{
						//clicked in an not-allowed area inside the board.
					}
				}
			}
		}
	}
	else if (button == 2 && state == 0 && editMode){//right mouse
		if (cell* clickedCell = gm->checkClick(x, y)){ //if an cell is clicked. Toggle between disabled and free
			if (clickedCell->getContent() == 0){
				clickedCell->setContent(CELL_FREE);
			}
			else{
				clickedCell->setContent(CELL_DISABLED);
			}
		}
	}
	//redraw all changes
	glutPostRedisplay();
}

//next player, return false if no players are left
bool nextPlayer(){
	if (!editMode){
		char oldPlayer = activePlayer;
		while (true)
		{
			cout << "Moving to next player" << endl;
			gm->getPlayer(activePlayer)->setActive(0);
			activePlayer = (activePlayer + 1) % gm->getNumPlayers(); //next player
			if (activePlayer == oldPlayer){//back to the start. All players are skipped for some reason
				cout << "No player can move" << endl;
				endGame(); //game over, no player can move
				return false; //end of game, no players with points or possible moves left;
			}
			if (gm->getPlayer(activePlayer)->getPoints() == 0){ //if player is dead
				continue;
			}
			
			cell* neighborList[6] = {}; //not used var
			cell* jump; //not used var
			if (gm->getPlayer(activePlayer)->checkMoves(neighborList, &jump) == nullptr){//if player can't move
				cout << "Player " << activePlayer + 1 << " can't move. Next player." << endl;
				continue;
			}
			gm->getPlayer(activePlayer)->setActive(1); //show that this player is active
			return true;
		}
	}
	else{
		cout << "Can't play in edit mode. Press e to go to play mode" << endl;
	}
	return false;
}

//clears 'doubles' and 'jumps' and 'activecell' for when a cell is deselected or a move is made
void clearMoves(){
	if (activeCell){
		activeCell = nullptr;
	}
		for (int i = 0; i < 6; i++){
			if (doubles[i] != nullptr){
				doubles[i] = nullptr;
			}
		}
		for (int i = 0; i < 12; i++){
			if (jumps[i] != nullptr){
				jumps[i] = nullptr;
			}
		}
}
//disable the highlights on the cells
void hideMoves(){
	if (activeCell){
		activeCell->setHighlight(CELL_DEFAULT, 0);
	}
	for (int i = 0; i < 6; i++){
		if (doubles[i] != nullptr){
			doubles[i]->setHighlight(CELL_DEFAULT, 0);
		}
	}
	for (int i = 0; i < 12; i++){
		if (jumps[i] != nullptr){
			jumps[i]->setHighlight(CELL_DEFAULT, 0);
		}
	}
}


//set the highlights on all cells mentioned in the 'doubles' and 'jumps' lists
void showMoves(unsigned char playerNr){
	if (activeCell){
		activeCell->setHighlight(CELL_SELECTED, playerNr); 
	}
	for (int i = 0; i < 6; i++){
		if (doubles[i] != nullptr){
			doubles[i]->setHighlight(CELL_DOUBLE, playerNr);
		}
	}
	for (int i = 0; i < 12; i++){
		if (jumps[i] != nullptr){
			jumps[i]->setHighlight(CELL_JUMP, playerNr);
		}
	}
}

//end the game and show who won
void endGame(){
	int highest=0;
	for (int i = 0; i < gm->_numPlayers; i++){ //find highest number of points
		short thisPoints = gm->getPlayer(i)->getPoints();
		if (thisPoints>highest){
			highest = thisPoints;
		}
	}
	for (int i = 0; i < gm->_numPlayers; i++){ //finds who got the highest number (can be multiple)
		short thisPoints = gm->getPlayer(i)->getPoints();
		if (thisPoints==highest){
			cout << "Player " << gm->getPlayer(i) << " has won the game with "<<highest << " points." << endl;
			gm->getPlayer(i)->setActive(2);
		}
	}
}

//---------------------------------------------------------------------------
//	reshape: Handle window resizing (reshaping) events
//	The reshape callback is called when the window is resized or moved
//	In the current implementation, the drawing grid is extended or truncated 
//	when the window is reshaped
void reshape(int w, int h) 
{	
	glViewport(0.0f, 0.0f, (GLfloat)w, (GLfloat)h);
	glMatrixMode(GL_PROJECTION);  
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
	//remember width and especially height to remap mouse position
	height = h;
	width = w;
}
//---------------------------------------------------------------------------
/*
	void display(void)
	The main display callback. This callback is called when the screen has
	to be redrawn. This is when:
	-The screen first becomes active
	-The screen is resized
	-The screen is repositioned
*/
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);  // clear the backbuffer
	item* itemptr = 0;
	dlist::ITER iter(&drawList);

	while ((itemptr = iter.next()) != 0){
		itemptr->draw();
	}

	glFlush();			// Execute all commands waiting to be executed
	glutSwapBuffers();	// Swap the backbuffer and frontbuffer,
						// display what has been drawn
 }
//-----------------------
void startGame(){
	//create a new instance of game.
	gm = new game(&drawList);

}
int main(int argc, char* argv[])
{
	// standard GLUT initialization 
	glutInit(&argc,argv);
	init();						// Call init function
	startGame();
	printf("starting Glut Loop...\n");
	glutMainLoop();				// Start drawing proceidure
	
	return 0;

}
//---------------------------------------------------------------------------