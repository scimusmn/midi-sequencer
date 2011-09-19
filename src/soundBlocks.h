/*
 *  soundBlocks.h
 *  Miditron
 *
 *  Created by Exhibits on 9/13/11.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofExtended.h"
#include "ofxMidiIn.h"
#include "ofxMidiOut.h"

class soundBlock : public ofInterObj{
protected:
  bool bPercussive;
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
  void changeTitle(string ttl);
	friend class inst;
};

class rhythmBlock : public soundBlock {
protected:
	ofBaseApp * baseApp;
	ofTimer rhythmTimer,playTimer;
	double playTime,rhythmTime;
	bool bPlaying;
	vector<unsigned char> message;
public:
  unsigned char note;
	unsigned char channel;
	rhythmBlock():soundBlock(){}
	rhythmBlock(const rhythmBlock & t);
	void setup(string title, unsigned char nt, unsigned char chan);
	void setMidi(unsigned char nt, unsigned char chan, double delay=1);
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
	ofButton leftAdj;
  ofButton rightAdj;
	bool bResizing;
  bool bLResize;
	bool bActive;
	bool bJustActive;
	double aPos;
public:
  unsigned char note;
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