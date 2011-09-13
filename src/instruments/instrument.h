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
#include "soundBlocks.h"

class inst : public ofInterObj{
protected:
  bool bHolding;
  bool bPercussive;
  int fullWidth;
  unsigned long startPlay;
  double tempo;
  double scrollX,scrollY;
  int lastBlock;
	bool bDefault;
public:
  string title;
	rhythmBlock base;
  vector<dragBlock> blocks;
  int size(){ return blocks.size();}
	void clear(){ blocks.clear();}
  bool isPlaying(){ return base.isPlaying(); }
	bool isHeld(){ return bHolding; }
  bool isDefault(){ return bDefault; }
	void setDefault(bool holder){ bDefault=holder; } 
  
  //---.cpp defined functions
  void setPercussive(bool perc);
  dragBlock & operator[](int i);
  void update(int disp,ofDirection dir);
  void setMidi(unsigned char chan, unsigned char nt);
  void setColor(unsigned long hex);
  ofColor getColor(){ return base.color; }
  void setup(string objName, unsigned char chan, unsigned char nt);
	void sendMidiMessage(vector<unsigned char> newMessage);
  
  //--- should be moved to a class for the keyboard insts
  
  
  //----- virtual functions
  virtual void draw(int _x, int _y){}
	virtual void draw(){}
	virtual void drawBackground(){}
  virtual bool clickDown(int _x, int _y){}
	virtual bool clickUp(){}
	virtual void mouseMotion(int _x, int _y){}
};

class instrument : public inst{
protected:
public:
	int point;
	instrument & operator=(const instrument & t);
	instrument():inst(){}
	instrument(string title, unsigned char chan, unsigned char nt);
	void resizeByFont(int fontSize);
  
  //---- inst class functions
	void draw(int _x, int _y);
	void draw();
	void drawBackground();
	bool clickDown(int _x, int _y);
	bool clickUp();
	void mouseMotion(int _x, int _y);
  
	void play(unsigned char vel);
	void play();
	void stop();
	bool active(double pos);
  void scaleToTempo(double time);
	friend class bandBar;
	friend class remapBand;
	friend class remapInst;
};
	
class synthInstrument : public inst{
protected:
public:
  synthInstrument():inst(){}
	synthInstrument(string title, unsigned char chan, unsigned char nt);
  void draw(int x, int y);
  void draw();
  void drawBackground();
  bool clickDown(int _x, int _y);
  bool clickUp();
};
