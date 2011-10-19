/*
 *  groupInstrument.h
 *  Midi_Sequencer
 *
 *  Created by Exhibits on 10/19/11.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#pragma once

#include "../band.h"
#include "instrument.h"
#include "synthInst.h"

#define as_groupInst(x) static_cast<synthInstrument *>(x)

class groupInst : public inst{
protected:
  bandBar * band;
  vector<instrument> notes;
  bool bNoteSelect;
  ofDropDown drop;
  int lastInst;
  
  bool bOpen;
  ofButton openBut;
  
  ofFont label;
  ofFont subhead;
public:
  groupInst():inst(){
  }
	groupInst(string title, unsigned char chan, unsigned char nt, bandBar * bnd=0);
  
  string getProgramName(){ return drop.getString(); }
  int getProgramNumber(){ return drop.getChoiceNumber(); }
  void changeProgram(int progNum);

  void update();
  
  void draw(int x, int y);
  void draw();
  void drawBackground();
  void drawForeground();
  bool clickDown(int _x, int _y);
  bool clickUp();
  void mouseMotion(int _x, int _y);
  bool active(double pos);
  
  void changeNoteHeight(instrument & b, double h);
  
  bool isPlaying();
  dragBlock & lastDrop();
  void scaleToTempo(double time);
  void setBandWidth(double wid);
  int farthestPoint();
};