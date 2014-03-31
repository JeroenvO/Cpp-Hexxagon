////////////////////////////////////////////////////////////
//
// File: drawtools.cpp
// Author: Sven Goossens ( largely based on schematic.h by Patrick Groeneveld )
// Purpose:
//      Extending the dlist and item classes to provide a set of 'drawtools'
//      Contains an implementation of the pixel class, and skeleton for the
//      Line class
//
///////////////////////////////////////////////////////////

#ifndef DRAWTOOLS_H // this avoids multiple inclusion of this file
#define DRAWTOOLS_H

#include "dlist.h"  // to include the dlist class
#include "coord.h"
#include <string>

using namespace std;
class line;          // forward: the classes in this file
class pixel;
class text;
class drawCell;


// class declarations
     ///////////////////////////////////////////////////////
    //
   //  pixel class.
  //
 ///////////////////////////////////////////////////////

class pixel :
             public item    // inherits from item because its part of a DrawList
{
public:
	// constructors. Specify the list of the item, postion and color
	pixel(dlist* list, const float p[2], const float color[3]);
	~pixel(void);

	void print() const;   // print myself in EDIF++ format
	void draw(void) const;// draw myself to the screen

	// you can add more public members here, if needed

private:
	float _p[2];    // Coordinates
	float _color[3];
	// you can add more private members here, if needed

};

     ///////////////////////////////////////////////////////
    //
   //  line class.
  //
 ///////////////////////////////////////////////////////

class line :
             public item    // inherits from item because its part of a DrawList
{
public:
	// constructors. Specify the list of the line, postion, color and width
	line(dlist* list, const float p1[2], const float p2[2],
									const float color[3], const int lineWidth);
	~line(void);

	void print() const;   // print myself in EDIF++ format
	void draw(void) const;// draw myself to the screen

	// you can add more public members here, if needed

private:
	float _p1[2], _p2[2];
	float _color[3];
	float _lw;
};

     ///////////////////////////////////////////////////////
    //
   //  text class.
  //
 ///////////////////////////////////////////////////////

class text :
			public item    // inherits from item because its part of a DrawList
{
public:
	text(dlist* list, const float p[2], const float color[3], const string str);
	~text(void);

	void print() const;   // print myself in EDIF++ format
	void draw(void) const;// draw myself to the screen

	// you can add more public members here, if needed

private:
	string _str;
	float _p[2];    // Coordinates
	float _color[3];
	float _lw; //linewidth
	// you can add more private members here, if needed

};

///////////////////////////////////////////////////////
//
//  drawCell class.
//
///////////////////////////////////////////////////////

class drawCell :
	public item    // inherits from item because its part of a DrawList
{
public:
	drawCell(dlist* list, const coord center, const coord p[6], const unsigned char lw, int cellNr);
	~drawCell(void);

	//void print() const;   // print myself in EDIF++ format
	void draw(void) const;// draw myself to the screen
	void getPoints(coord points[6]) const;
	coord getCenter(void) const;

	void setHighlight(const float clr[3], bool stipple);
	void setFill(const float clr[3]);
	void setNr(int cellNr); //set the text to display on the 
	
private:
	string _cellNr;
	coord _p[6];    // 6 
	coord _center;
	float _fill[3];
	float _highlight[3];
	unsigned char _linewidth;
	bool _stipple;
};
///////////////////////////////////////////////////////
//
//  drawPlayer class.
//
///////////////////////////////////////////////////////
class drawPlayer :
	public item
{
public:
	drawPlayer(dlist* list, const coord p[4], const coord t, const float clr[3], const unsigned char nr);
	~drawPlayer(void);
	void draw(void) const;
	void getColor(float clr[3]) const;
	void setBackground(float clr[3]);
	//int getPoints(void) const;
	void setTxt(string txt);
private:
	unsigned char _playerNr;
	string _txt;
	float _clr[3];
	float _bgclr[3];
	coord _p[4];
	coord _t; //text location
};
#endif
