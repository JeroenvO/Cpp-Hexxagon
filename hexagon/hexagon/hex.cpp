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
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string>
#include <iostream>
#include <fstream>
//#include "glut.h"//temp
#include "drawtools.h"
#include "hex.h"
#include "coord.h"
#include "glut.h"

using namespace std;
// class definitions:
///////////////////////////////////////////////////////
//
//  game class.
//
///////////////////////////////////////////////////////

//constructor of game class
game::game(dlist* drawList)
{
	_drawList = drawList;
	//load default board
	openBoard("default");
	//set the window title
	glutSetWindowTitle("Hexagon by Jeroen van Oorschot");
}

//get number of players
unsigned char game::getNumPlayers() const{
	return _numPlayers;
}

//show the numbers of the cell on the cells or not
void game::showNr(bool show){
	if (!show){
		for (int i = 0; i < _boardSize; i++){
			_board[i]->getDrawID()->setNr(-1);
		}
	}
	else{
		for (int i = 0; i < _boardSize; i++){
			_board[i]->getDrawID()->setNr(_board[i]->getCellNumber());
		}
	}
}

//get a player from the game
player* game::getPlayer(const unsigned char playerNr) const{
	return _players[playerNr];
}

//check if there is a clickable cell at {x,y} and if so, return its cellpointer
cell* game::checkClick(int x, int y) const{
	unsigned char max[2], bh = this->_boardHeight;
	_board[_boardSize-1]->getRC(max);
	coord smallest = _board[_boardHeight+2]->getDrawID()->getCenter();
	coord largest  = _board[_boardSize-_boardHeight-3]->getDrawID()->getCenter();
	if (x > smallest.x && y > smallest.y && x < largest.x && y < largest.y){ //if inside board
		int firstnr = 2 * _boardHeight+1; //first two columns inaccesable
		int lastnr = _boardSize - 2 * _boardHeight - 1;//last two columns inaccesable
		for (int cellNr = firstnr; cellNr <= lastnr; cellNr++){//iterate over all hexagons
			unsigned char RC[2];
			_board[cellNr]->getRC(RC);
			if (RC[0]<2 || (RC[0]>bh-2 && RC[0] < bh+3) || RC[0] > bh*2-2){ //first two rows and last two rows inaccesable
				//cout << "forbidden cells" << endl;
				continue;
			}

			if (_board[cellNr]->checkHexClick(x, y)) //check if this hexagon was clicked
			{
				//cout << "found!!" << endl;
				return _board[cellNr];
			}
		}
	}
	else{
		//cout << "outside board" << endl;
	}
	return nullptr;
}

//store a board to a file
int game::storeBoard(const string name) const{
	ofstream file;
	file.open(name + ".brd", ios::out);
	if (file.is_open()){
		file << (int)_boardSize << endl << (int)_boardHeight << " " << (int)_numPlayers << endl;
		for (int i = 0; i < _boardSize; i++){
			file << i << " " << (int) _board[i]->getContent() << endl;
		}
		file.close();
		return 1;
	}
	return 0;
}

//new board with bs cells and bh cell in a column
void game::newBoard(unsigned short bs, unsigned char bh, unsigned char np){
	if (bs != _boardSize || bh != _boardHeight){ //make new board if the size is different
		if (bs >= 4 * bh && bs<=500 && bh>4 && bh <= 20){
			//deleteBoard
			for (int i = 0; i < _boardSize; i++){
				//cout << "deleting " << i << " number: " << _board[i]->getCellNumber() << endl;
				delete _board[i]; //drawCell is destructed in cell destructor
				_board[i] = nullptr;
			}
			delete[] _board;
			//_board = nullptr;
			_boardSize = bs;
			_boardHeight = bh;
			_board = new cell*[_boardSize];
			for (int i = 0; i < _boardSize; i++){
				_board[i] = new cell(i, this);
			}
		}
		else{
			cout << "Invalid board values: Use boardSize > 4*BoardHeight, BoardSize <= 500,  4 < BoardHeigth <= 20 "<< endl;
			return;
		}
	}
	for (int i = 0; i < _boardSize; i++){
		_board[i]->setContent(0);
	}
	//fill board
	
	if (np < 7) { //max 6 players
		//remove old players
		for (int i = 0; i < _numPlayers; i++){
			delete _players[i];
			_players[i] = nullptr;
		}
		delete[] _players;
		_numPlayers = np;
		_players = new player*[_numPlayers];
		//fill the list
		float clr[3];
		for (int i = 0; i < _numPlayers; i++){
			for (int j = 0; j < 3; j++) {
				clr[j] = ((i + 1)&(1 << j)) ? 1 : 0;
			}
			_players[i] = new player(i, clr, this); //make player and drawPlayer object
			_players[i]->setPoints(0);
		}
	}
	else{
		cout << "Too much players; 6 is the max." << endl;
	}
	_players[0]->setActive(1);

	//resize the window to approximatily fit this boardsize
	int x, y;
	y = 2 * _hexSize*_boardHeight + 200;
	x = 2 * _hexSize*_boardSize / _boardHeight;
	glutReshapeWindow(x, y);

	//redisplay
	glutPostRedisplay();
}
//open a board from a given file
int game::openBoard(string name){
	ifstream file;
	name = name + ".brd";
	file.open(name, ios::in);
	if (file.is_open()){
		int bs, bh,np;
		char str[10];  //input buffer

		file.getline(&str[0], 10);
		if (sscanf(&str[0], "%i\n", &bs) != 1){
			return 0;
		}
		file.getline(&str[0], 10);
		if (sscanf(&str[0], "%i %i", &bh, &np) != 2){
			return 0;
		}

		newBoard(bs, bh, np); //make the board and the players
		
		//fill the board.
		int content, ln;
		for (int i = 0; i < _boardSize; i++){
			file.getline(&str[0], 10);
			if (sscanf(&str[0], "%i %i", &ln, &content) != 2){
				return 0;
			}
			if (ln != i){
				return 0;
			}
			_board[i] = new cell(ln, this);
			_board[i]->setContent(content);
		}
		file.close();
		glutPostRedisplay();
		return 1;
	}
	cout << "Opening board " << name << " failed" << endl;
	return 0;
}
///////////////////////////////////////////////////////
//
//  cell class.
//
///////////////////////////////////////////////////////
//make new cell
cell::cell(unsigned char cellNumber, game* game){
	//assert(cellNumber);
	_cellNumber = cellNumber;
	_content = 0;
	_highlight = 0;
	_game = game;
	coord p[6];
	coord center = getCellLocation(p);
	_drawID = new drawCell(_game->_drawList, center, p, _game->_lineWidth, cellNumber);

	setHighlight(0,false);
}
cell::~cell(){
	delete _drawID;//remove drawclass
}

//return the row and column of the board where this cell is located
void cell::getRC(unsigned char RC[2]) const{
	int column = ceil(_cellNumber / (2 * this->_game->_boardHeight + 1));//(0-no columns)
	int height = _cellNumber % (2 * this->_game->_boardHeight + 1); //(0-_boardHeight*2)
	RC[0] = height;
	RC[1] = column;
}

//return the index number of this cell
int cell::getCellNumber() const{
	return _cellNumber;
}
//return the drawID of this cell. The pointer to the object that draws this cell.
drawCell* cell::getDrawID() const{
	return _drawID;
}

//get the content of this cell
unsigned char cell::getContent() const{
	return _content;
}

//set the content of the cell, like the gem of a player
void cell::setContent(unsigned char content){
	assert(content < 9);
	//unsigned char contentOld = _content;
	if (_content>2){//if cell was from a player
		_game->_players[_content - 3]->setPoints(-1);
	}
	float clr[3];
	_content = content;
	switch (content){
	case 0: //disabled
		clr[0] = 0.2;  //r
		clr[1] = 0.2;  //g
		clr[2] = 0.2;  //b
		break;
	case 1: //free
		clr[0] = 0.7;  //r
		clr[1] = 0.7;  //g
		clr[2] = 0.7;  //b
		break;
	case 2: //extra 1, white
		clr[0] = 0.9;  //r
		clr[1] = 0.9;  //g
		clr[2] = 0.9;  //b
		break;
	default: //
		//player
		int player = content - 3;
		_game->_players[player]->setPoints(1);//points +1
		_game->_players[player]->getColor(clr); //color
		break;
	}
	_drawID->setFill(clr);
}

//set a highlight
void cell::setHighlight(const unsigned char type, const unsigned char playerNr){
	_highlight = type;
	float clr[3];
	bool stipple;
	switch (type){
	case 0: //default
		stipple = false;
		clr[0] = 1;  //r
		clr[1] = 1;  //g
		clr[2] = 1;  //b
		break;
	case 1: //selected
		stipple = false;
		clr[0] = 0.5;  //r
		clr[1] = 0.5;  //g
		clr[2] = 0.5;  //b
		break;
	case 2: //double
		stipple = false;
		_game->getPlayer(playerNr)->getColor(clr);
/*		clr[0] = 0.7;  //r
		clr[1] = 0.7;  //g
		clr[2] = 0.7;  //b*/
		break;
	case 3: //jump
		_game->getPlayer(playerNr)->getColor(clr);
		stipple = true;
		//clr[0] = 0.55;  //r
		//clr[1] = 0.55;  //g
		//clr[2] = 0.55;  //b
		break;
	default: //
		cout << "invalid setHighlight" << endl;
		return;
		break;
	}
	_drawID->setHighlight(clr,stipple);
}

//return the center of the cell in {x,y} and fill the list with the cornerpoints of the cell.
coord cell::getCellLocation(coord p[6]) const{
	double s3 = sqrt(3) / 2;
	int hs = this->_game->_hexSize;
	int bh = this->_game->_boardHeight;
	double hss = hs * s3; //small side of hex
	double x = hs;//center points
	double y = hss;
	unsigned char RC[2], height, column;
	this->getRC(RC);//row and column belonging to this number
	height = RC[0]; //(0 - _boardHeight * 2)
	column = RC[1]; //(0-_boardSize)

	const int lw = _game->_lineWidth; //num pixels between cells === linewidth
	const int offsetX = _game->_playerSize + 2 * _game->_lineWidth;
	if (height > bh){ //2nd row
		y -= hss;
		height -= bh;
		x += 1.5* hs;
		x += lw / 2;
		y -= lw / 2;
	}

	y += height * 2 * hss;

	x += column*hs * 3;
	x += column*lw + offsetX;
	y += height*lw;
	coord center = { x, y };

	for (int i = 0; i < 6; i++){
		p[i].x = hs*cos(1.04719*i) + x;
		p[i].y = hs*sin(1.04719*i) + y;
	}
	return center;
}

//check wheter this cell covers {x,y}
bool cell::checkHexClick(const int x, const int y) const{
	coord p[6];
	_drawID->getPoints(p);
	double r3 = 1.73205;

	if (x > p[3].x && x < p[0].x && y < p[1].y && y > p[5].y){ // inside large square around hexagon
		if (x <= p[1].x && x >= p[2].x){ //inside area 2
			return true;
		}
		else if (x <= p[2].x){//area 1
			if (y >= p[3].y && (x-p[3].x) > ((y-p[3].y) / r3) /*1a*/ || y <= p[3].y && (x-p[3].x) > (-(y-p[3].y) / r3) /*1b*/){
				return true;
			}
			return false;
		}
		else if(x >= p[1].x){ //area 3
			if (y >= p[0].y && (x - p[0].x) < (-(y - p[0].y) / r3) /*3a*/ || y <= p[0].y && (x - p[0].x) < ((y-p[3].y) / r3) /*3b*/){
				return true;
			}
			return false;
		}
	}
	return false;
}


//fill the supplied array with the neighbors of the current cell.
void cell::getNeighbors(cell* neighborList[6]) const{

	unsigned char cn = this->_cellNumber;
	unsigned char bh = _game->_boardHeight;
	if (cn < (2 * bh) || cn>(_game->_boardSize - 2 * bh)){
		cout << "Invalid cn: " << cn << endl;
		return;
	}
	neighborList[0] = _game->_board[cn - bh - 1];
	neighborList[1] = _game->_board[cn - bh];
	neighborList[2] = _game->_board[cn - 1];
	neighborList[3] = _game->_board[cn + 1];
	neighborList[4] = _game->_board[cn + bh];
	neighborList[5] = _game->_board[cn + bh + 1];
}

//fill the supplied array with the cells that are 1 cell away from current cell
void cell::getFarNeighbors(cell* farNeighborList[12]) const{
	unsigned char cn = this->_cellNumber;
	unsigned char bh = _game->_boardHeight;
	farNeighborList[0] = _game->_board[cn - 2*bh - 2];
	farNeighborList[1] = _game->_board[cn - 2*bh - 1];
	farNeighborList[2] = _game->_board[cn - 2*bh    ];
	farNeighborList[3] = _game->_board[cn -   bh + 1];
	farNeighborList[4] = _game->_board[cn -   bh - 2];
	farNeighborList[5] = _game->_board[cn -  2];
	farNeighborList[6] = _game->_board[cn +  2];
	farNeighborList[7] = _game->_board[cn +   bh - 1];
	farNeighborList[8] = _game->_board[cn +   bh + 2];
	farNeighborList[9] = _game->_board[cn + 2*bh    ];
   farNeighborList[10] = _game->_board[cn + 2*bh + 1];
   farNeighborList[11] = _game->_board[cn + 2*bh + 2];
}

//get all cells that are free and have borders with this cell
//if they are not free, remove them from the supplied list
void cell::getDoubles(cell* doubles[6]) const{
	getNeighbors(doubles); //fill the doubles list with all the neighbours
	for (int i = 0; i < 6; i++){ //all neighbor cells
		if (doubles[i] != nullptr && doubles[i]->getContent() == 1){ //content 1 means that the cell is drawable
			//ok
		}
		else{
			doubles[i] = nullptr; //remove this cell because it is not possible to overtake this one.
		}
	}
}

//return all cells that are free and have exactly one cell between the current cell
//if they are not free, remove them from the supplied list
void cell::getJumps(cell* jumps[12]) const{
	getFarNeighbors(jumps); //fill the doubles list with all the neighbours
	for (int i = 0; i < 12; i++){ //all jump cells
		if (jumps[i] != nullptr && jumps[i]->getContent() == 1){ //content 1 means that the cell is drawable
			//ok
		}
		else{
			jumps[i] = nullptr; //remove this cell because it is not possible to overtake this one.
		}
	}
}

//count how many cells can be taken over and remove the cells that can't be taken over from the supplied neighborlist
//this is splitted from takeOver() because this one can be used for an AI to determine the best move
unsigned char cell::checkTakeOver(cell* neighborList[6], unsigned char who) const{
	unsigned char numTakeOver = 0,c ;
	who += 3;
	for (int i = 0; i < 6; i++){
		c = neighborList[i]->getContent();
		if (neighborList[i] != nullptr && c > 1 && c != who){ //if possible to takeover
			numTakeOver++;
		}
		else{ //not able to take this one over, so remove it from the list
			neighborList[i] = nullptr;
		}
	}
	return numTakeOver;
}

//take neighbouring cells over from other user and claim this cell
//who is the number of the current player and src is the cell where the move came from
void cell::takeOver(cell* takeOverList[6], unsigned char who, cell* src){
	who += 3;
	//set this cell
	setContent(who);
	//take over neighbours
	for (int i = 0; i < 6; i++){
		if (takeOverList[i] != nullptr){
			takeOverList[i]->setContent(who);
		}
	}
	if (src != nullptr){ //in case of a jump, the source cell is cleared.
		src->setContent(1);
	}
}
///////////////////////////////////////////////////////
//
//  player class.
//
///////////////////////////////////////////////////////

//a player in the game. Max 8 players are possible
player::player(const unsigned char nr, const float color[3], game* game){

	_playerNumber = nr;
	_game = game;
	_points = 0;

	coord p[4];
	int lw = this->_game->_lineWidth;
	int ps = this->_game->_playerSize;
	double y = 2 * game->_hexSize*game->_boardHeight - nr*(ps/2 + 2*lw);
	
	p[0] = { lw, y };
	p[1] = { lw, y - ps/2 };
	p[2] = { ps+lw , y - ps/2 };
	p[3] = { ps +lw, y };
	coord t = { 15, y - ps / 4 -3 }; //text location
	_drawID = new drawPlayer(_game->_drawList, p, t, color, nr);
}

//remove player and his drawclass
player::~player(){
	delete _drawID;
}

//set the relative amount of points for this player
void player::setPoints(int points){
	if (points == 0){
		_points = 0;
	}
	else{
		_points += points;
	}
	assert(_points >= 0);
	_drawID->setTxt(to_string(_points)); //draw the number of points
	float clr[3];
	if (_points > 0){
		clr[0] = 0.7;
		clr[1] = 0.7;
		clr[2] = 0.7;
	}
	else{
		clr[0] = 0.2;
		clr[1] = 0.2;
		clr[2] = 0.2;
	}
	_drawID->setBackground(clr);
}

void player::setActive(const unsigned short active){
	float clr[3];
	if (active==1){ //current player
		clr[0] = 0.8;
		clr[1] = 0.8;
		clr[2] = 0.8;
	}
	else if(active==0){       //not active
		if (_points > 0){ //alive
			clr[0] = 0.7;
			clr[1] = 0.7;
			clr[2] = 0.7;
		}
		else{ //dead
			clr[0] = 0.2;
			clr[1] = 0.2;
			clr[2] = 0.2;
		}
	}
	else if (active == 2){ //player won the game
		clr[0] = 0.1;
		clr[1] = 0.7;
		clr[2] = 0.1;
	}
	_drawID->setBackground(clr); //put the color to the draw class, to actually draw it
}
//get the number of cells owned by this player
unsigned char player::getPoints() const{
	return _points;
}

//get this players color
void player::getColor(float clr[3]) const{
	_drawID->getColor(clr); //retrieve the color from the drawclass, It is not stored in this class to keep the memory usage low.
}

cell* player::checkMoves(cell* takeOverList[6], cell** jumpSrc) const{
	cell* neighborList[6] = {}; //temp for storing neighbors and far neighbor of cell under examination
	cell* farNeighborList[12] = {}; 
	cell* takeOverListHi[6] = {};//temp for storing takeoverlist of a possible move
	bool canMove = false; //if the player can actually move
	cell* thisCell = nullptr;
	int firstnr = 2 * _game->_boardHeight + 1, nrTakeOver = 0, nrTakeOverHi = 0, takeOverHiCell = -1; //first two columns inaccesable
	int lastnr = _game->_boardSize - 2 * _game->_boardHeight - 1;//last two columns inaccesable
	for (int cellNr = firstnr; cellNr <= lastnr; cellNr++){//iterate over all hexagons
		thisCell = _game->_board[cellNr];
		if (thisCell->getContent() == (_playerNumber+3)){ //if this cell is from this player, (all cells of this player)
			//get neighbors of this cell
			thisCell->getNeighbors(neighborList);
			//count points gain if a move would be made to any of these neighbors
			for (int i = 0; i<6; i++){ //neighbors
				if (neighborList[i] == nullptr || neighborList[i]->getContent() != 1){ //skip if a move can't be made to this cell
					continue; 
				}
				canMove = true;
				neighborList[i]->getNeighbors(takeOverList); //get the cells to possibly take over
				nrTakeOver = (neighborList[i]->checkTakeOver(takeOverList, _playerNumber) + 1); //maximum number of cells to take over. +1 because the cell doubles. 

				if (nrTakeOver >= nrTakeOverHi){ //store the highest takeOver
					nrTakeOverHi = nrTakeOver;
					*jumpSrc = nullptr;//this isn't a jump
					if (nrTakeOver == 6){ //this is the maximum possible
						return _game->_board[neighborList[i]->getCellNumber()];
					}
					else{
						takeOverHiCell = neighborList[i]->getCellNumber();
						for (int j = 0; j < 6; j++){
							takeOverListHi[j] = takeOverList[j];
						}
					}
				}
			}
			//if nrTakeOverHi is still zero no move can be made with all previous cells and no double is possible from this cell
			
			_game->_board[cellNr]->getFarNeighbors(farNeighborList);//check if a jump would be better than a double
			for (int i = 0; i<12; i++){ //farneighbors
				if (farNeighborList[i] == nullptr || farNeighborList[i]->getContent() != 1){ //skip if a move can't be made to this cell
					continue;
				}
				canMove = true;
				farNeighborList[i]->getNeighbors(takeOverList);//get the cells to possibly take over
				nrTakeOver = farNeighborList[i]->checkTakeOver(takeOverList, _playerNumber); //maximum number of cells to take over

				if (nrTakeOver > nrTakeOverHi || nrTakeOverHi == 0){ //never overwrite doubles with jumps (use >) except if nothing else is possibel (nrTakeOverHi = 0)
					nrTakeOverHi = nrTakeOver;
					*jumpSrc = _game->_board[cellNr]; //this is a jump, remember where it came from
					if (nrTakeOver == 6){ //this is the maximum possible
						return _game->_board[farNeighborList[i]->getCellNumber()];
					}
					else{
						takeOverHiCell = farNeighborList[i]->getCellNumber();
						for (int j = 0; j < 6; j++){
							takeOverListHi[j] = takeOverList[j];
						}
					}
				}
			}
		}
	}
	if (canMove){

		for (int j = 0; j < 6; j++){
			takeOverList[j] = takeOverListHi[j];
		}
		return _game->_board[takeOverHiCell];
	}
	else{ //can't takeover, just do something else
		return nullptr;
	}
}