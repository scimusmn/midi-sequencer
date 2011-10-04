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

#include "../soundBlocks/soundBlocks.h"

enum instRet {
  NO_CLICK,BASE_CLICK,DRAG_CLICK,KB_CLICK
};

class inst : public ofInterObj{
protected:
  bool bHolding;
  bool bPercussive;
  int fullWidth;
  unsigned long startPlay;
  double tempo;
  ofPoint scroll;
  int lastBlock;
	bool bDefault;
  bool bNewBlock;
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
  void setBandWidth(double wid){ fullWidth=wid;}
  dragBlock & lastDrop(){ 
    if(lastBlock>=0&&lastBlock<blocks.size()) return blocks[lastBlock];
    return blocks[blocks.size()-1];
  }
  double vertScrollPos(){ return scroll.y;}
  
  //---.cpp defined functions
  void setPercussive(bool perc);
  dragBlock & operator[](int i);
  void update(int disp,ofDirection dir);
  void setMidi(unsigned char chan, unsigned char nt);
  void setColor(unsigned long hex);
  ofColor getColor(){ return base.color; }
  void setup(string objName, unsigned char chan, unsigned char nt);
	void sendMidiMessage(vector<unsigned char> newMessage);
	void play();
	void stop();
  void scaleToTempo(double time);
  bool releaseDraggedBlock();
  
  //--- should be moved to a class for the keyboard insts
  void play(unsigned char vel);
  
  //----- virtual functions
  virtual void draw(int _x, int _y){}
	virtual void draw(){}
	virtual void drawBackground(){}
  virtual void drawForeground(){}
  virtual bool clickDown(int _x, int _y){return false;}
	virtual bool clickUp(){return false;}
	virtual void mouseMotion(int _x, int _y){}
  virtual void update(){}
  virtual bool active(double pos){return false;}
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
  bool active(double pos);
  
	friend class bandBar;
	friend class remapBand;
	friend class remapInst;
};
