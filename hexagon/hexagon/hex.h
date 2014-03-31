////////////////////////////////////////////////////////////
//
// File: hex.h
// Author: Jeroen van Oorschot, 0803331
// Purpose:
//      Class that contains the hexagon game
//      
//      
//
///////////////////////////////////////////////////////////

#ifndef HEX_H // this avoids multiple inclusion of this file
#define HEX_H

//#include "dlist.h"  // to include the dlist class

#include <string>
#include "drawtools.h"
#include "coord.h"
#include "dlist.h"
using namespace std;

class game;          // forward: the classes in this file
class cell;
class player;
/*
*/
// class declarations
///////////////////////////////////////////////////////
//
//  game class.
//
///////////////////////////////////////////////////////

class game
{
	friend class cell;// player; //let cell access all members like the boardsize 
	friend class player;
public:
	// constructors. Specify the list of the item, postion and color
	game(dlist* drawList);
	~game(void);
	
	unsigned char getNumPlayers() const;
	player* getPlayer(const unsigned char playerNr) const;
	void showNr(bool show); //show cellnumber on cells
	cell* checkClick( const int x, const int y) const;
	
	//store board functions
	int storeBoard(const string name) const;//store a board to a file
	int openBoard(const string name);//open a board from a given file
	void newBoard(const unsigned short bs, const unsigned char bh, const unsigned char np); //new board with bs cells and bh cell in a column. np is the number of players in the game

	//global scaling numbers
	unsigned char _hexSize = 25;
	unsigned char _lineWidth = 3;
	unsigned char _playerSize = 100;


	unsigned short _boardSize;// = 163;
	unsigned char _boardHeight = 12;
	unsigned char _numPlayers;

	//global drawing list
	dlist* _drawList;

private:
	//board
	cell** _board;

	//players
	player** _players;
};
///////////////////////////////////////////////////////
//
//  cell class.
//
///////////////////////////////////////////////////////
class cell
{
public:
	cell(unsigned char cellNumber, game* game);
	~cell(void);

	
	void getRC(unsigned char RC[2]) const;  //get the row and column where this cell is located
	int getCellNumber(void) const;			//get the index number of this cell
	drawCell* getDrawID(void) const;		//get the pointer to the drawCell that draws this cell
	unsigned char getContent() const;		//get the content of this cell

	void setContent(const unsigned char contents);//set the content
	void setHighlight(const unsigned char type, const unsigned char playerNr);  //set the highlight


	coord getCellLocation(coord p[6]) const; //return center and fill p[6] with border points
	bool checkHexClick(const int x, const int y) const; //check whether this cell covers {x, y}

	void getNeighbors(cell* neighborList[6]) const; //fill the neighborlist with cell pointesr
	void getFarNeighbors(cell* farNeighborList[12]) const; //fill the supplied array with the cells that are 1 cell away from current cell

	//game functions
	void cell::getDoubles(cell* doubles[6]) const;
	void cell::getJumps(cell* doubles[6]) const;
	unsigned char checkTakeOver(cell* neighborList[6], const unsigned char who) const; //count how many cells can be taken over
	void takeOver(cell* takeOverList[6], unsigned char who, cell* src); //take neighbouring cells over from other user and claim this cell

private: //private

	/*
	content: 
	0: not drawable
	1: drawable, empty
	2: white, will be overtaken by any user
	3: owned by user1
	4: owned by user2
	5: 
	*/
	unsigned char _content; //unsigned char _content;
	unsigned char _highlight;
	unsigned char _cellNumber; // unsigned char _cellNumber;
	game* _game; //game* _game;
	drawCell* _drawID;
	/*
	
	*/
};
///////////////////////////////////////////////////////
//
//  player class.
//
///////////////////////////////////////////////////////
class player
{
public:
	player(const unsigned char nr, const float color[3], game* game);
	~player();
	void draw(void) const; //draw the player at the left side of the screen.
	void getColor(float clr[3]) const; //return this players color

	void setPoints(int points); //set the players points. 0 resets the points, !=0 sets the relative points
	unsigned char getPoints() const; //return this players points
	void setActive(const unsigned short active); //mark a player as active, not active or won
	cell* checkMoves(cell* takeOverList[6], cell** jumpSrc) const; //return the cellpointer to move with most gains

	unsigned char _playerNumber;
private:
	unsigned char _points;
	game* _game;
	drawPlayer* _drawID;
};
#endif
