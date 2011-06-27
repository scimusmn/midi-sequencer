/*
 *  instrument.h
 *  Miditron
 *
 *  Created by Exhibits on 3/29/2554.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#pragma once

#define F_YEAH_WOOD 0

#include "ofMain.h"
#include "ofExtended.h"
#include "ofxMidiIn.h"
#include "ofxMidiOut.h"

//typedef rhythmBlock;

class soundBlock : public ofInterObj{
protected:
public:
	string title;
	ofColor color;
	ofFont header;
	double xDisp, yDisp;
	soundBlock():ofInterObj(){}
	soundBlock(string objName);
	~soundBlock();
	soundBlock(const soundBlock & t);
	void operator=(const soundBlock & t);
	void setup(string objName);
	void update(double x_dis, double y_dis);
	void draw(int _x, int _y);
	void draw(int _y);
	void draw();
	bool clickDown(int _x, int _y);
	bool clickUp();
	void mouseMotion(int _x, int _y);
	friend class instrument;
};

class rhythmBlock : public soundBlock {
protected:
	ofBaseApp * baseApp;
	ofTimer rhythmTimer,playTimer;
	double playTime,rhythmTime;
	bool bPlaying,bRepeat;
	unsigned char note;
	unsigned char channel;
	vector<unsigned char> message;
public:
	rhythmBlock():soundBlock(){}
	rhythmBlock(const rhythmBlock & t);
	void setup(string title, unsigned char nt, unsigned char chan, bool repeat=false);
	void setMidi(unsigned char nt, unsigned char chan, bool repeat=false, double delay=1);
	void setDelay(double dly);
	void sendMidiMessage(vector<unsigned char> newMessage);
	void play(unsigned char vel);
	void play();
	void stop();
	void update();
	bool clickDown(int _x, int _y);
	bool clickUp();
	bool isPlaying(){ return bPlaying;}
	friend class instrument;
};

class dragBlock : public soundBlock {
protected:
	ofButton sizeAdj;
	bool bResizing;
	bool bActive;
	bool bJustActive;
	double aPos;
public:
  dragBlock(int x, int w, rhythmBlock & t);
	dragBlock(const rhythmBlock & t);
	void draw(int _y);
	void draw();
	void mouseMotion(int _x, int _y);
	bool clickDown(int _x, int _y);
	bool clickUp();
	bool active(double xPos);
	bool justChanged(double xPos);
	friend class instrument;
};

class instrument : public ofInterObj{
protected:
	bool bHolding;
	double mousePosX, yoff;
	double scrollX,scrollY;
	int lastBlock;
	bool bDefault;
	bool bPercussive;
public:
  vector<dragBlock> blocks;
	int point;
	string title;
	rhythmBlock base;
	dragBlock & operator[](int i);
	instrument & operator=(const instrument & t);
	int size(){ return blocks.size();}
	void clear(){ blocks.clear();}
	instrument():ofInterObj(){}
	instrument(string title, unsigned char chan, unsigned char nt, bool repeat=false);
	void resizeByFont(int fontSize);
	void update(int x_disp, int y_disp);
	void update(int disp,ofDirection dir);
	void update();
	void draw(int _x, int _y);
	void draw();
	bool isDefault(){ return bDefault; }
	void setDefault(bool holder){ bDefault=holder; } 
	void drawBackground();
	bool clickDown(int _x, int _y);
	bool clickUp(vector<instrument> & holders);
	bool clickUp();
	void mouseMotion(int _x, int _y);
	void setMidi(unsigned char chan, unsigned char nt, bool repeat=false);
	void setColor(unsigned long hex);
	void setDelay(double dly);
	void setup(string objName, unsigned char chan, unsigned char nt, bool repeat);
	void sendMidiMessage(vector<unsigned char> newMessage);
	void play(unsigned char vel);
	bool isPlaying(){ return base.isPlaying(); }
	bool isHeld(){ return bHolding; }
	void setPercussive(bool perc){bPercussive=perc;}
	void play();
	void stop();
	bool active(double pos);
	friend class bandBar;
	friend class remapBand;
	friend class remapInst;
};
	

