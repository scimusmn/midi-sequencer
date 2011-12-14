/*
 *  keyboard.cpp
 *  Miditron
 *
 *  Created by Exhibits on 3/28/2554.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#include "keyboard.h"

extern ofColor white;
extern ofColor black;
extern ofColor gray;
extern ofColor yellow;
extern ofColor orange;
extern ofColor blue;

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
  ofFlat();
	ofRoundedRect(x, y, w, h, w/8.);
	if(buttons.getChoice()){
		if(notes.size()){
			if(bSharp) ofSetColor(notes[0].base.color-.2*255.);
			else ofSetColor(notes[0].base.color);
		}
	}
	else if(bSelected){
    if(!bSharp) ofSetColor(white);
    else ofSetColor(black);
  }
	else {
		if(bSharp) ofSetColor(0, 0, 0);
		else ofSetColor(255, 255, 255);
	}
  if(bPressed) ofSetColor(ofGetStyle().color*.8);
  ofFlat();
	ofRoundedRect(x+1, y+1, w-2, h-2, w/8.);
  //ofSetShadowDarkness(.2);
	//ofShade(x+1, y+1, 6, h-2, OF_RIGHT, false);
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
  x=_x,y=_y;
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
  framePad.y=wid/32.;
	framePad.x=wid/32.;
	w=w+framePad.x;
	h=octaves[0].h+framePad.y;
}

pianoKey & pianoKeyboard::operator[](int i)
{
	return octaves[i/12][i%12];
}

void pianoKeyboard::draw(int _x, int _y)
{
	x=_x, y=_y;
	double keyboardStartX=x+framePad.x;
	ofSetColor(black);
  ofFlat();
	ofRect(_x, _y, w, h*63/64);
  //ofSetShadowDarkness(.4);
  //ofShadowRounded(octaves[0].x, octaves[0].y, octaves[0].w*octaves.size(), octaves[0].h-20, 0, 20);
	for (unsigned int i=0; i<octaves.size(); i++) {
		octaves[i].draw(keyboardStartX,_y+framePad.y);
		keyboardStartX+=octaves[i].w;
	}
	ofSetColor(0, 0, 0);
	ofRect(octaves[0].x, _y+framePad.y*.9, w-framePad.x*2, framePad.y/2.);
	//ofShade(octaves[0].x, _y+framePad.y*.9, 10, w-framePad.x*2, OF_UP);
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

void pianoKeyboard::clear()
{
	for (unsigned int i=0; i<octaves.size(); i++) {
		octaves[i].clear();
	}
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
