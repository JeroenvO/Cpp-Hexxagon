////////////////////////////////////////////////////////////
//
// File: drawtools.cpp
// Author: Sven Goossens
// Purpose:
//      Extending the dlist and item classes to provide a set of 'drawtools'
//      Contains an implementation of the pixel class, and skeleton for the
//      Line class
//
//	Hexagon, Jeroen van Oorschot 2014
//
///////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string>

#include "glut.h"
#include "hex.h"
#include "drawtools.h"

using namespace std;


// class definitions:

     ///////////////////////////////////////////////////////
    //
   //  pixel class.
  //
 ///////////////////////////////////////////////////////
/*
      Please note that the pixel class is an incredibly inefficient way to draw
      1(!!) pixel to the screen. This class is mearly ment as an example of
      how to use the DrawList. 
*/
pixel::pixel(dlist* list, const float p[2], const float color[3])
	: item(list)    // call the ctor of item and chain into list
{
	assert(p);   // if you crash here then you called this with no coordinates
	_p[0] = p[0];// Store location of the pixel in the member variable _p
	_p[1] = p[1];

	if(!color) {      // When a null pointer is passed to this function
		_color[0] = 0;// black is used
		_color[1] = 0;
		_color[2] = 0;
	}
	else {
		_color[0] = color[0];
		_color[1] = color[1];
		_color[2] = color[2];
	}
}
//---------------------------------------------------------------------------
//
// draw myself on the screen
//
void pixel::draw(void) const
{
	glBegin(GL_POINTS);            // Start drawing mode

	// If a color was passed to the function, set it
	glColor3f(_color[0],_color[1], _color[2]);
	glVertex2fv(this->_p);        // Draw the pixel at (_p[0], _p[1])

	glEnd();
}
//---------------------------------------------------------------------------
//
// Print myself in ELIF format
//
void pixel::print(void) const
{
	printf(".pixel %.0f %.0f %.5f %.5f %.5f\n", _p[0], _p[1], _color[0],
															_color[1], _color[2]);
}
//---------------------------------------------------------------------------
//
// destructor of a pixel
//
pixel::~pixel(void)
{

}
//---------------------------------------------------------------------------
     ///////////////////////////////////////////////////////
    //
   //  line class.
  //
 ///////////////////////////////////////////////////////
line::line(dlist* list, const float p1[2], const float p2[2],
                        const float color[3], const int lineWidth)
	: item(list)    // call the ctor of item and chain into list
{
	{
		assert(p1);   // if you crash here then you called this with no coordinates
		assert(p2);

		_p1[0] = p1[0];// Store location of the pixel in the member variable _p
		_p1[1] = p1[1];
		_p2[0] = p2[0];// Store location of the pixel in the member variable _p
		_p2[1] = p2[1];

		if (!color) {      // When a null pointer is passed to this function
			_color[0] = 0;// black is used
			_color[1] = 0;
			_color[2] = 0;
		}
		else {
			_color[0] = color[0];
			_color[1] = color[1];
			_color[2] = color[2];
		}
		if (lineWidth > 0){
			_lw = lineWidth;
		}
		else{
			_lw = 1;
		}
	}
}
//---------------------------------------------------------------------------
//
// destructor of a line
//
line::~line(void)
{

}
//---------------------------------------------------------------------------
//
// Print myself in EDIF++ format
//
void line::print(void) const
{
	printf(".line %.0f %.0f %.0f %.0f %.5f %.5f %.5f %.0f\n", _p1[0], _p1[1], _p2[0], _p2[1], _color[0],
		_color[1], _color[2], _lw);
}
//---------------------------------------------------------------------------
//
// draw myself on the screen
//
void line::draw(void) const
{
	// TODO: line::draw(void), to be implemented by you
	// Aim for about 10 lines of code
	glLineWidth(_lw);
	glBegin(GL_LINES);            // Start drawing mode
		
		// If a color was passed to the function, set it
		glColor3f(_color[0], _color[1], _color[2]);
		glVertex2fv(_p1);        // Draw the pixel at (_p[0], _p[1])
		glVertex2fv(_p2);

	glEnd();
}

///////////////////////////////////////////////////////
//
//  text class.
//
///////////////////////////////////////////////////////

text::text(dlist* list, const float p[2], const float color[3], const string str)
: item(list)    // call the ctor of item and chain into list
{
	assert(p);   // if you crash here then you called this with no coordinates
//	assert(str);
	if (str == "")
		return;
	_p[0] = p[0];// Store location of the pixel in the member variable _p
	_p[1] = p[1];

	if (!color) {      // When a null pointer is passed to this function
		_color[0] = 0;// black is used
		_color[1] = 0;
		_color[2] = 0;
	}
	else {
		_color[0] = color[0];
		_color[1] = color[1];
		_color[2] = color[2];
	}
	//HELP
	//malloc(strlen(str));
	//strcpy(_str, str);
	_str = str;
}
//---------------------------------------------------------------------------
//
// destructor of a line
//
text::~text(void)
{

}
void text::draw(void) const{
	glColor3fv(this->_color);

	//start drawing
	glRasterPos2fv(_p);
	int len = _str.length();
	for (int i = 0; i < len; i++){
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, _str[i]);
	}
}

void text::print(void) const{
	printf(".text %.0f %.0f %.5f %.5f %.5f \"%s\"\n", _p[0], _p[1], _color[0],
		_color[1], _color[2], _str.c_str());
}

///////////////////////////////////////////////////////
//
//  drawCell class.
//
///////////////////////////////////////////////////////

drawCell::drawCell(dlist* list, const coord center, const coord p[6], const unsigned char lw, int cellNr)
: item(list)    // call the ctor of item and chain into list
{
	assert(p);   // if you crash here then you called this with no coordinates
	//	assert(str);
	
	_p[0] = p[0];
	_p[1] = p[1];
	_p[2] = p[2];
	_p[3] = p[3];
	_p[4] = p[4];
	_p[5] = p[5];
	
	_center = center;
	 
	_linewidth = lw;

	setNr(cellNr);
}
//---------------------------------------------------------------------------
//
// destructor of a hex
//
drawCell::~drawCell(void)
{

}
void drawCell::draw(void) const{
	//fill
	glColor3fv(_fill);
	glBegin(GL_POLYGON);
	for (int i = 0; i < 6; i++){
		glVertex2f(_p[i].x, _p[i].y);
	}
	glEnd();
	//border
	glLineStipple(3, 0xAAAA);
	if (_stipple){
		glEnable(GL_LINE_STIPPLE);
	}
	else{
		glDisable(GL_LINE_STIPPLE);
	}
	glColor3fv(_highlight);
	glLineWidth(_linewidth);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 6; i++){
		glVertex2f(_p[i].x, _p[i].y);
	}
	glEnd();

	glRasterPos2f(_center.x-12, _center.y-3);
	//draw number on cell
	int len = _cellNr.length();
	for (int i = 0; i < len; i++){
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, _cellNr[i]);
	}
	
}
coord drawCell::getCenter(void) const{
	return _center;
}
void drawCell::getPoints(coord points[6]) const{
	for (int i = 0; i < 6; i++){
		points[i] = _p[i];
	}
}
void drawCell::setHighlight(const float clr[3], bool stipple){
	for (int i = 0; i < 3; i++){
		_highlight[i] = clr[i];
	}
	_stipple = stipple;
}
void drawCell::setFill(const float clr[3]){
	for (int i = 0; i < 3; i++){
		_fill[i] = clr[i];
	}
}

void drawCell::setNr(int cellNr){
	if (cellNr < 0){
		_cellNr = "";
	}
	else{
		_cellNr = to_string(cellNr);
	}
}

///////////////////////////////////////////////////////
//
//  drawPlayer class.
//
///////////////////////////////////////////////////////
drawPlayer::drawPlayer(dlist* list, const coord p[4], const coord t, const float clr[3], const unsigned char nr)
: item(list)    // call the ctor of item and chain into list
{
	assert(p);   // if you crash here then you called this with no coordinates
	//	assert(str);
	_clr[0] = clr[0];
	_clr[1] = clr[1];
	_clr[2] = clr[2];

	_p[0] = p[0];
	_p[1] = p[1];
	_p[2] = p[2];
	_p[3] = p[3];
	_t = t;
	_playerNr = nr + 49;
}
drawPlayer::~drawPlayer(void){}

void drawPlayer::setTxt(string txt){
	if (txt.length() < 4){
		_txt = txt;
	}
}
void drawPlayer::draw() const{
	//background
	glColor3fv(_bgclr);
	glBegin(GL_QUADS);
	for (int i = 0; i < 4; i++){
		glVertex2f(_p[i].x, _p[i].y);
	}
	glEnd();
	//border
	glColor3fv(_clr);
	glLineWidth(3);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 4; i++){
		glVertex2f(_p[i].x, _p[i].y);
	}
	glEnd();
	
	
	//player number
	glRasterPos2f(_t.x-2, _t.y+6);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, _playerNr);
	//player text, number of points (=number of cells)
	glRasterPos2f(_t.x+30, _t.y);
	int len = _txt.length();
	for (int i = 0; i < len; i++){
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, _txt[i]);
	}

	
}
void drawPlayer::getColor(float clr[3]) const{
	clr[0] = _clr[0];
	clr[1] = _clr[1];
	clr[2] = _clr[2];
}
void drawPlayer::setBackground(float clr[3]){
	_bgclr[0] = clr[0];
	_bgclr[1] = clr[1];
	_bgclr[2] = clr[2];
}