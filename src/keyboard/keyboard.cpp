/*
 *  keyboard.cpp
 *  Miditron
 *
 *  Created by Exhibits on 3/28/2554.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#include "keyboard.h"

static vector<string> programNames;

int loadProgramNames(string filename){
	if(programNames.size()==0){
		programNames.clear();
		ifstream k(ofToDataPath(filename).c_str());
		while(k.peek()!=EOF){
			string temp;
			getline(k, temp);
			programNames.push_back(temp);
		}
	}
	return programNames.size();
}

vector<string> getProgramNames(){
	return programNames;
}

void parseMidiProgramNames(string filename){
	ifstream k(ofToDataPath(filename).c_str());
	while(k.peek()!=EOF){
		string temp;
		getline(k, temp);
		vector<string> tokend=ofSplitString(temp, "\t");
		if(tokend.size()>1)
			programNames.push_back(tokend[1]);
		else
			programNames.push_back(tokend[0]);
	}
	k.close();
	ofstream out(ofToDataPath(filename+"_parsed").c_str());
	for (unsigned int i=0; i<programNames.size(); i++) {
		out << programNames[i] << endl;
	}
	out.close();
}

pianoKey::pianoKey(double _w, double _h, char nt):ofInterObj(0,0,_w,_h)
{
	note=nt;
	o_w=_w;
	o_h=_h;
	bSharp=false;
	bSelected=false;
}

pianoKey::~pianoKey(){}

void pianoKey::setSharp(bool sharp)
{
	bSharp=sharp;
	if (bSharp) {
		w=o_w*2./3.;
		h=o_h*9./14.;
	}
}

void pianoKey::operator=(const pianoKey & t)
{
	note=t.note;
	x=t.x,y=t.y,w=t.w,h=t.h,o_w=t.o_w,o_h=t.o_h,bSharp=t.bSharp;
	buttons=t.buttons;
}

void pianoKey::draw(double _x, double _y)
{
	x=_x, y=_y;
	ofSetColor(0, 0, 0);
	ofRoundBox(x, y, w, h, w/8., .02);
	if(bPressed) ofSetColor(50, 150, 255);
	else if(buttons.getChoice()){
		if(notes.size()){
			if(bSharp) ofSetColor(notes[0].base.color-.2*255.);
			else ofSetColor(notes[0].base.color);
		}
	}
	else if(bSelected) ofSetColor(25, 75, 190);
	else {
		if(bSharp) ofSetColor(0, 0, 0);
		else ofSetColor(255, 255, 255);
	}
	ofRoundBox(x+1, y+1, w-2, h-2, w/8., .3);
	ofShade(x+1, y+1, 6, h-2, OF_RIGHT, .3, false);
}

void pianoKey::resetInstruments()
{
	for (unsigned int i=0; i<notes.size(); i++) {
		notes[i].setDefault(true);
	}
}

void pianoKey::press(bool sel)
{
	bPressed=sel;
}

bool pianoKey::clickDown(int _x, int _y)
{
	bool ret=false;
	if (over(_x, _y)) {
		ret=bSelected=bPressed=true;
	}
	//buttons.clickDown(_x, _y);
	if(bSelected&&clearNotes.clickDown(_x, _y)){
		resetInstruments();
		buttons.select(0); 
	}
	return ret;
}

bool pianoKey::clickUp()
{
  bool ret=bPressed;
	bPressed=false;
	clearNotes.clickUp();
	return ret;
}

pianoOctave::pianoOctave(double width,char octave_begin_note):ofInterGroup()
{
	w=width,h=width/1.2;
	double wwid=w/7.;
	int sharps[5]={1,3,6,8,10};
	for (int i=0; i<12; i++) {
		char nt=octave_begin_note+i;
		keys.push_back(pianoKey(wwid,h,nt));
	}
	for (unsigned int i=0; i<5; i++) {
		keys[sharps[i]].setSharp(true);
	}
}

void pianoOctave::setup(double width,char octave_begin_note)
{
	w=width,h=width/1.2;
	float wwid=w/7.;
	int sharps[5]={1,3,6,8,10};
	for (int i=0; i<12; i++) {
		char nt=octave_begin_note+i;
		keys.push_back(pianoKey(wwid,h,nt));
	}
	for (unsigned int i=0; i<5; i++) {
		keys[sharps[i]].setSharp(true);
	}
}

pianoOctave::~pianoOctave()
{
	keys.clear();
}

pianoKey & pianoOctave::operator[](int i)
{
	return keys[i];
}

void pianoOctave::draw(int _x, int _y)
{
	double xpos=_x;
	if(keys.size()) keys[0].draw(_x,_y),xpos+=keys[0].w;
	for (unsigned int i=1; i<keys.size(); i++) {
		if (!keys[i].bSharp) {
			keys[i].draw(xpos,_y);
			xpos+=keys[i].w;
		}
	}
	for (unsigned int i=1; i<keys.size(); i++) {
		if (keys[i].bSharp) {
			keys[i].draw(keys[i-1].x+keys[i-1].w-keys[i].w/2,_y);
		}
	}
}

bool pianoOctave::clickDown(int _x, int _y)
{
	bool ret=false;
  if(over(_x, _y)) bPressed=true;
	for (unsigned int i=0; i<keys.size(); i++) {
		if(keys[i].bSharp&&keys[i].clickDown(_x,_y)){
			ret=true;
			oSelected=i;
			for (unsigned int j=0; j<keys.size(); j++) {
				if(j!=i) keys[j].bSelected=false;
			}
			break;
		}
	}
	if(!ret) for (unsigned int i=0; i<keys.size(); i++) {
		if(!keys[i].bSharp&&keys[i].clickDown(_x,_y)){
			ret=true;
			oSelected=i;
			for (unsigned int j=0; j<keys.size(); j++) {
				if(j!=i) keys[j].bSelected=false;
			}
			break;
		}
	}
	return ret;
}

pianoKey & pianoOctave::getKey()
{
	return keys[oSelected];
}

bool pianoOctave::clickUp()
{
  bool ret=0;
  bPressed=false;
	for (unsigned int i=0; i<keys.size(); i++) {
		ret|=keys[i].clickUp();
	}
  return ret;
}

void pianoOctave::clear()
{
	for (unsigned int i=0; i<keys.size(); i++) {
		keys[i].bSelected=false;
	}
}

pianoKeyboard::pianoKeyboard(double wid, double nOctaves):ofInterGroup()
{
	w=wid;
	for (int i=0; i<nOctaves; i++) {
		octaves.push_back(pianoOctave(wid/nOctaves,i*12));
	}
}

pianoKeyboard::~pianoKeyboard()
{
	octaves.clear();
}

void pianoKeyboard::setup(double wid,double nOctaves)
{
	w=wid;
	for (int i=0; i<nOctaves; i++) {
		octaves.push_back(pianoOctave(wid/nOctaves,i*12));
	}
	xDis=wid/16.;
	w=w+xDis;
	h=octaves[0].h+xDis/2.;
}

pianoKey & pianoKeyboard::operator[](int i)
{
	return octaves[i/12][i%12];
}

void pianoKeyboard::draw(int _x, int _y)
{
	x=_x, y=_y;
	double xpos=x+xDis/2.;
	ofSetColor(175,112,75);
	ofRoundBox(_x, _y, w, h, xDis/4., .2);
	ofRoundShadow(_x+xDis/8., _y+xDis/8., w-xDis/4., h, xDis/2., .4);
	for (unsigned int i=0; i<octaves.size(); i++) {
		octaves[i].draw(xpos,_y+xDis/2.);
		xpos+=octaves[i].w;
	}
	ofSetColor(0, 0, 0);
	ofRect(_x+xDis/2., _y+xDis/2-xDis*.02, w-xDis, xDis/4.);
	ofShade(_x+xDis/2., _y+3*xDis/4.-xDis*.02, 10, w-xDis, OF_DOWN, .3);
}

bool pianoKeyboard::clickDown(int _x, int _y)
{
	bool ret=false;
  if(over(_x, _y)) bPressed=true;
	for (unsigned int i=0; i<octaves.size(); i++) {
		if (octaves[i].clickDown(_x,_y)) {
			oSelected=i;
			ret=1;
			for (unsigned int j=0; j<octaves.size(); j++) {
				if(i!=j) octaves[j].clear();
			}
		}
	}
	return ret;
}

int pianoKeyboard::size()
{
	return octaves.size()*12;
}

bool pianoKeyboard::clickUp()
{
  bool ret=0;
  bPressed=false;
	for (unsigned int i=0; i<size(); i++) {
		ret|=octaves[i/12][i%12].clickUp();
	}
  return ret;
}

void pianoKeyboard::update()
{
}

void pianoKeyboard::mouseMotion(int _x, int _y)
{
  if(bPressed){
    if(over(_x,_y)&&!getKey().over(_x, _y)){
      clickUp();
      clickDown(_x, _y);
    }
  }
}

pianoKey & pianoKeyboard::getKey()
{
	return octaves[oSelected].getKey();
}

int pianoKeyboard::activeKey(){
	return oSelected*12+octaves[oSelected].oSelected;
}

void pianoKeyboard::pressKey(int i){
	oSelected=i/12;
	octaves[oSelected].oSelected=i%12;
	(*this)[i].select();
}
