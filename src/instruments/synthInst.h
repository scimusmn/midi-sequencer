/*
 *  synthInst.h
 *  Miditron
 *
 *  Created by Exhibits on 9/13/11.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#pragma once

#include "band.h"
#include "instrument.h"
#include "keyboard.h"

void loadInstruments(string filename);

class synthInstrument : public inst{
protected:
  bandBar * band;
  bool bNoteSelect;
  pianoKeyboard kb;
  ofDropDown drop;
  ofTimer hold;
  bool bNewBlock;
public:
  synthInstrument():inst(){
    hold.set(1.0);
    hold.pause();
  }
	synthInstrument(string title, unsigned char chan, unsigned char nt, bandBar * bnd=0);
  
  void changeProgram(int progNum);
  void update();
  
  void draw(int x, int y);
  void draw();
  void drawBackground();
  bool clickDown(int _x, int _y);
  bool clickUp();
  void mouseMotion(int _x, int _y);
  bool active(double pos);
};