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

enum instType {
  INST_DEFAULT,INST_SYNTH,INST_GROUP
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
  instType type;
public:
  string title;
	rhythmBlock base;
  vector<dragBlock> blocks;
  int size(){ return blocks.size();}
  /*inst & operator=(inst & t)
   {
   bHolding=t.bHolding;
   bPercussive=t.bPercussive;
   fullWidth=t.fullWidth;
   base=t.base;
   title=t.title;
   startPlay=t.startPlay;
   tempo=t.tempo;
   bDefault=t.bDefault;
   type=t.type;
   }*/
  
	bool isHeld(){ return bHolding; }
  bool isDefault(){ return bDefault; }
	void setDefault(bool holder);
  
  double vertScrollPos(){ return scroll.y;}
  instType getType(){ return type; }
  ofColor getColor(){ return base.color; }
  
  //---.cpp defined functions
  void setPercussive(bool perc);
  dragBlock & operator[](int i);
  void setMidi(unsigned char chan, unsigned char nt);
  void setColor(unsigned long hex);
  void setHeld(bool hld);
  void setup(string objName, unsigned char chan, unsigned char nt);
	void sendMidiMessage(vector<unsigned char> newMessage);
	void play();
	void stop();
  
  bool releaseDraggedBlock();
  
  //--- should be moved to a class for the keyboard insts
  void play(unsigned char vel);
  
  //----- virtual functions
  virtual void draw(int _x, int _y);
	virtual void draw();
	virtual void drawBackground(){}
  virtual void drawForeground(){}
  virtual bool clickDown(int _x, int _y){return false;}
	virtual bool clickUp(){return false;}
	virtual void mouseMotion(int _x, int _y){}
  virtual void update(){}
  virtual bool active(double pos){return false;}
  virtual void loseFocus(){}
  
  
  //_-_-_-_-_ virtual functions which are also defined
  virtual bool isPlaying(){ return base.isPlaying(); }
  virtual void setBandWidth(double wid){ fullWidth=wid;}
  virtual dragBlock & lastDrop(){ 
    if(lastBlock>=0&&lastBlock<blocks.size()) return blocks[lastBlock];
    return blocks[blocks.size()-1];
  }
  virtual void scaleToTempo(double time,double xScroll);
  virtual int farthestPoint();
  virtual void update(int disp,ofDirection dir);
  virtual void clear(){ blocks.clear();}
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
