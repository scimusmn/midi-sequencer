/*
 *  groupInstrument.cpp
 *  Midi_Sequencer
 *
 *  Created by Exhibits on 10/19/11.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#include "groupInstrument.h"
#include "../midiConfig.h"

extern ofColor white;
extern ofColor black;
extern ofColor gray;
extern ofColor yellow;

groupInst::groupInst(string title, unsigned char chan, unsigned char nt, bandBar * bnd){
  band=bnd;
  setup(title, chan, nt);
  type=INST_GROUP;
  openBut.setAvailable(true);
  bOpen=bSynth=false;
  label.loadFont("fonts/DinC.ttf");
  label.setSize(24);
  label.setMode(OF_FONT_TOP);
  subhead.loadFont("fonts/Arial.ttf");
  lastInst=0;
  openBut.setup(base.h, OF_VERT, "images/dropdown2.png");
}

void groupInst::addInstruments(ofTag & tag)
{
  for (unsigned int i=0; i<tag.size(); i++) {
		if (tag[i].getLabel()=="instrument") {
			string col=tag[i].getAttribute("color");
			string title=tag[i].getAttribute("name");
			long color=strtol(col.c_str(),NULL,0);
			int curInst=notes.size();
			unsigned char note;
      instType type=INST_DEFAULT;
			map<string,int> list;
			list["note"]=0;
			for(int j=0;j<tag[i].size();j++){
				string node[2]={tag[i][j].getLabel(),tag[i][j].getValue()};;
				switch (list.find(node[0])->second) {
					case 0:
						note=ofToInt(node[1]);
						break;
					default:
						break;
				}
			}
			notes.push_back(instrument(title,base.channel,note));
			notes[curInst].setPercussive(bPercussive);
			notes[curInst].setColor(color);
		}
	}
}

instrument & groupInst::operator()(int i)
{
  return notes[i];
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
  double yOff=0;
  for (unsigned int i=0; i<notes.size(); i++) {
    notes[i].update(disp,dir);
    
    if(!bOpen){
      notes[i].base.y=y+yOff;
      yOff+=notes[i].h;
    }
  }
}

void groupInst::clear()
{
  for (unsigned int i=0; i<notes.size(); i++) {
    notes[i].clear();
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
  //openBut.cSetup(x, y+scroll.y, ofGetWidth(), base.h);
  if(bOpen){
    int ySpace=band->verticleBlockSpace();
    int yOff=base.h+2*ySpace;
    ofSetColor((base.color*.5).opacity(.3));
    ofRect(band->getBorderSize(),y+scroll.y-ySpace, fullWidth-band->getBorderSize(), base.h+ySpace*2);
    for (unsigned int i=0; i<notes.size(); i++) {
      if(i%2) ofSetColor((base.color*.5).opacity(.1));
      else ofSetColor((base.color*.7).opacity(.1));
      ofRect(band->getBorderSize(),y+yOff-ySpace+scroll.y, fullWidth-band->getBorderSize(), notes[i].h+ySpace*2);
      ofSetShadowDarkness(.1);
      ofShade(band->getBorderSize(), y+yOff-ySpace+scroll.y,3, fullWidth-band->getBorderSize(), OF_UP);
      ofShade(band->getBorderSize(), y+yOff-ySpace+scroll.y,3, fullWidth-band->getBorderSize(), OF_DOWN,false);
      notes[i].draw(x+50,y+yOff);
      yOff+=notes[i].h+2*ySpace;
    }
    h=yOff-2*ySpace;
    openBut.draw(x+w +10, y+scroll.y+openBut.h, openBut.w,-openBut.h);
  }
  else {
    openBut.draw(x+w +10, y+scroll.y);
  }
  
  if(!bSynth||(bSynth&&!bOpen)) base.draw(x,y);
  else synthDD.draw(x, y+(base.h-synthDD.h)/2+scroll.y);
  
}

void groupInst::drawBackground()
{
  for (unsigned int i=0; i<notes.size(); i++) {
    if(bOpen){
      if(i%2) ofSetColor((base.color*.5).opacity(.1));
      else ofSetColor((base.color*.7).opacity(.1));
      ofRect(fullWidth,notes[i].y-band->verticleBlockSpace()+scroll.y, ofGetWidth()-fullWidth, notes[i].h+band->verticleBlockSpace()*2);
      ofSetColor(0, 0, 0);
      ofRect(fullWidth, notes[i].y-band->verticleBlockSpace()+scroll.y, ofGetWidth(), 1);
    }
    notes[i].drawBackground();
  }

  if(bOpen){
    string lab="Drag the below blocks to the right to program a sequence";
    if(bSynth) lab="Select a voice from the dropdown to the right, and drag blocks into a sequence";
    ofRectangle rect=label.getBoundingBox(lab, x+fullWidth, y);
    
    ofSetColor(black.opacity(.25));
    ofRect(rect.x-10, rect.y+(base.h-rect.height)/2-10+scroll.y, rect.width+20, rect.height+20);
    ofSetColor(yellow);
    label.drawString(lab, x+fullWidth, y+(base.h-rect.height)/2+scroll.y);
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
  bool clicked=0;
  if(!bOpen&&(openBut.clickDown(_x, _y)||base.clickDown(_x, _y)||(_x>x+fullWidth&&_y>y+scroll.y&&_y<y+h+scroll.y))){
    int ySpace=band->verticleBlockSpace();
    int yOff=base.h+2*ySpace;
    for (unsigned int i=0; i<notes.size(); i++) {
      changeNoteHeight(notes[i],notes[i].base.h);
      yOff+=notes[i].h+2*ySpace;
    }
    h=yOff-ySpace;
    band->adjustSize();
    band->setActive(this);
    bOpen=true;
    ret=1;
  }
  if(bOpen&&!ret){
    if(!openBut.pressed()&&(openBut.clickDown(_x, _y)||(0&&_x>x+fullWidth&&_y>y+scroll.y&&_y<y+base.h+scroll.y)))
      ret=1,loseFocus();
    else if(bSynth&&!synthDD.open&&synthDD.clickDown(_x, _y)){
      ret=1;
    }
    else if(bSynth&&synthDD.open){
      if(!synthDD.clickDown(_x, _y)){
        if(synthDD.justSelected()) changeProgram(synthDD.getChoiceNumber()),ret=1;
        if(synthDD.open) ret=1;
      }
    }
    
    for (unsigned int i=0; i<notes.size()&&!ret; i++) {
      if(notes[i].clickDown(_x,_y)){
        ret=1;
        lastInst=i;
      }
    }
  }
  return ret;
}

void groupInst::loseFocus()
{
  h=base.h;
  if(bOpen||1){
    int yOff=0;
    for (unsigned int i=0; i<notes.size(); i++) {
      changeNoteHeight(notes[i],base.h/float(notes.size()));
      notes[i].base.y=y+yOff+scroll.y;
      yOff+=notes[i].h;
    }
  }
  bOpen=false;
  band->setActive(0);
  band->adjustSize();
}

bool groupInst::clickUp()
{
  bool ret=0;
  openBut.clickUp();
  base.clickUp();
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
	bool ret=false;
  for (unsigned int i=0; i<notes.size(); i++) {
    if(notes[i].active(pos)) ret=true;
  }
  return ret;
}

bool groupInst::isPlaying()
{
  bool ret=false;
  for (unsigned int i=0; i<notes.size(); i++) {
    if(notes[i].isPlaying()) ret=true;
  }
  return ret; 
}

dragBlock & groupInst::lastDrop()
{
  return notes[lastInst].lastDrop();
}

void groupInst::scaleToTempo(double time,double xScroll)
{
  for (unsigned int i=0; i<notes.size(); i++) {
    notes[i].scaleToTempo(time,xScroll);
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

void groupInst::loadInstruments(string filename)
{
  bSynth=true;
  //synthDD.setTextSize(20);
  synthDD.dallasStyle();
  for (unsigned int i=0; i<getSynthInstruments().size(); i++) {
    synthDD.setValue(getSynthInstruments()[i].name);
  }
  synthDD.setMode(false);
  synthDD.dallasStyle();
}

void groupInst::changeProgram(int progNum)
{
  base.changeTitle(getSynthInstruments()[progNum].name);
  vector<unsigned char> message;
  message.push_back(MIDI_PROGRAM_CHANGE+base.channel);
  message.push_back(getSynthInstruments()[progNum].number);
  base.sendMidiMessage(message);
}
