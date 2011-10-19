/*
 *  keyboard.h
 *  Miditron
 *
 *  Created by Exhibits on 3/28/2554.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */
#pragma once

#include "ofMain.h"
#include "ofExtended.h"
#include "../instruments/instrument.h"

#define MIDI_KEYBOARD_START 36
#define MIDI_KEYBOARD_END 84

int loadProgramNames(string filename);

vector<string> getProgramNames();

void parseMidiProgramNames(string filename);

class pianoKey:public ofInterObj{
protected:
	double o_w,o_h;
	unsigned char note;
	bool bSharp,bSelected;
public:
	vector<instrument> notes;
	ofButton clearNotes;
	ofRadioButtons buttons;
	pianoKey():ofInterObj(){}
	pianoKey(double _x, double _y, char note);
	~pianoKey();
	void operator=(const pianoKey & t);
	void setSharp(bool sharp);
	void draw(double _x, double _y);
	void update(bool sel);
	bool clickDown(int _x, int _y);
	unsigned char getNote(){return note;}
	bool clickUp();
	void select(bool s=true){bSelected=s;}
	void press(bool sel);
	bool isSharp(){return bSharp;}
	bool isSelected(){ return bSelected;}
	void resetInstruments();
	friend class pianoOctave;
};

class pianoOctave:public ofInterGroup{
protected:
	vector<pianoKey> keys;
	int oSelected;
public:
	pianoOctave(){}
	pianoOctave(double _w,char octave_begin_note);
	~pianoOctave();
	void setup(double _w,char octave_begin_note);
	pianoKey & operator[](int i);
	pianoKey & getKey();
	void draw(int _x, int _y);
	bool clickDown(int _x, int _y);
	bool clickUp();
	void clear();
	int size(){ return keys.size(); }
	friend class pianoKeyboard;
};

class pianoKeyboard:public ofInterGroup{
protected:
	vector<pianoOctave> octaves;
	int oSelected;
	double xDis,yDis;
	unsigned char channel;
public:
	pianoKeyboard(){};
	pianoKeyboard(double wid,double nOctaves);
	~pianoKeyboard();
	pianoKey & operator[](int i);
	void setup(double wid, double nOctaves);
	void setChannel(unsigned char chan){ channel=chan; }
	unsigned char getChannel(){ return channel; }
  void mouseMotion(int _x, int _y);
	void draw(int _x, int _y);
	bool clickDown(int _x, int _y);
	bool clickUp();
  void clear();
	void update();
	int size();
	int activeKey();
	void pressKey(int i);
	pianoKey & getKey();
};