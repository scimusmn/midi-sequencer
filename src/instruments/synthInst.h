/*
 *  synthInst.h
 *  Miditron
 *
 *  Created by Exhibits on 9/13/11.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#pragma once

#include "instrument.h"
#include "keyboard.h"

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