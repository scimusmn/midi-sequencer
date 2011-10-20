/*
 *  groupInstrument.cpp
 *  Midi_Sequencer
 *
 *  Created by Exhibits on 10/19/11.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#include "groupInstrument.h"

groupInst::groupInst(string title, unsigned char chan, unsigned char nt, bandBar * bnd){
  band=bnd;
  setup(title, chan, nt);
  type=INST_GROUP;
  for (int i=0; i<8; i++) {
    char nam[]="A";
    nam[0]+=(i+2)%7;
    string t=nam;
    cout << t << endl;
    notes.push_back(instrument(t,chan,nt+i));
    notes[i].setPercussive(true);
  }
  openBut.setAvailable(true);
  bOpen=false;
  label.loadFont("fonts/DinC.ttf");
  subhead.loadFont("fonts/Arial.ttf");
  lastInst=0;
}

void groupInst::update(int disp, ofDirection dir)
{
	bool vert=(dir==OF_VERT);
	base.update();
	if(!vert) scroll.x=disp;
	else scroll.y=disp;
	if(vert){
    base.soundBlock::update(0.,int(scroll.y));
  }
	else for (unsigned int i=0; i< blocks.size(); i++) {
    blocks[i].update(scroll.x,0);
  }
  for (unsigned int i=0; i<notes.size(); i++) {
    notes[i].update(disp,dir);
  }
}

void groupInst::update()
{
  
}

void groupInst::draw(int _x, int _y)
{
  x=_x;
  y=_y;
  draw();
}

void groupInst::draw()
{
  openBut.cSetup(x, y+scroll.y, ofGetWidth(), base.h);
  base.draw(x,y);
  if(bOpen){
    int ySpace=band->verticleBlockSpace();
    int yOff=base.h+2*ySpace;
    for (unsigned int i=0; i<notes.size(); i++) {
      notes[i].draw(x+20,y+yOff);
      yOff+=notes[i].h+2*ySpace;
    }
    h=yOff-2*ySpace;
  }
  else {
    h=base.h;
  }

}

void groupInst::drawBackground()
{
  for (unsigned int i=0; i<notes.size(); i++) {
    notes[i].drawBackground();
  }

}

void groupInst::drawForeground()
{
  
}

void groupInst::changeNoteHeight(instrument & b, double _h)
{
  b.h=_h;
  for (unsigned int j=0; j<b.size(); j++) {
    b[j].h=_h;
  }
}

bool groupInst::clickDown(int _x, int _y)
{
  bool ret=0;
  if(openBut.clickDown(_x, _y)){
    bOpen=!bOpen;
    if(bOpen){
      int ySpace=band->verticleBlockSpace();
      int yOff=base.h+2*ySpace;
      for (unsigned int i=0; i<notes.size(); i++) {
        changeNoteHeight(notes[i],notes[i].base.h);
        yOff+=notes[i].h+2*ySpace;
      }
      h=yOff-ySpace;
    }
    else {
      int yOff=0;
      for (unsigned int i=0; i<notes.size(); i++) {
        changeNoteHeight(notes[i],base.h/float(notes.size()));
        notes[i].base.y=y+yOff+scroll.y;
        yOff+=notes[i].h;
      }
      h=base.h;
    }

    band->setHeight();
  }
  if(bOpen){
    for (unsigned int i=0; i<notes.size(); i++) {
      if(notes[i].clickDown(_x,_y)){
        lastInst=i;
      }
    }
  }
  return ret;
}

bool groupInst::clickUp()
{
  bool ret=0;
  for (unsigned int i=0; i<notes.size(); i++) {
    if(notes[i].clickUp()){
      ret=1;
      lastInst=i;
    }
  }
  return ret;
}

void groupInst::mouseMotion(int _x, int _y)
{
  for (unsigned int i=0; i<notes.size(); i++) {
    notes[i].mouseMotion(_x,_y);
  }
}

bool groupInst::active(double pos)
{
  for (unsigned int i=0; i<notes.size(); i++) {
    if(notes[i].active(pos));
  }
}

bool groupInst::isPlaying()
{
  
}

dragBlock & groupInst::lastDrop()
{
  return notes[lastInst].lastDrop();
}

void groupInst::scaleToTempo(double time)
{
  for (unsigned int i=0; i<notes.size(); i++) {
    notes[i].scaleToTempo(time);
  }
}

void groupInst::setBandWidth(double wid)
{
  fullWidth=wid;
  for (unsigned int i=0; i<notes.size(); i++) {
    notes[i].setBandWidth(wid);
  }
}

int groupInst::farthestPoint()
{
  int ret=0;
  for (unsigned int i=0; i<notes.size(); i++) {
    ret=max(ret,notes[i].farthestPoint());
  }
  return ret;
}
