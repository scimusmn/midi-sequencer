/*
 *  synthInst.h
 *  Miditron
 *
 *  Created by Exhibits on 9/13/11.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#pragma once

#include "../band.h"
#include "instrument.h"
#include "../keyboard/keyboard.h"

#define as_synth(x) static_cast<synthInstrument *>(x)

class synthInstrument : public inst{
protected:
  bandBar * band;
  bool bNoteSelect;
  pianoKeyboard kb;
  ofDropDown drop;
  ofTimer hold;
  double xOrig;
  
  ofFont label;
  ofFont subhead;
public:
  synthInstrument():inst(){
    hold.set(1.0);
    hold.pause();
  }
	synthInstrument(string title, unsigned char chan, unsigned char nt, bandBar * bnd=0);
  
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
  void loseFocus(){ band->setActive(0); }
};