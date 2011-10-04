/*
 *  synthInst.cpp
 *  Miditron
 *
 *  Created by Exhibits on 9/13/11.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#include "synthInst.h"

vector<string> instNames;

void loadInstruments(string filename)
{
  ifstream file(ofToDataPath(filename).c_str());
	while (file.peek()!=EOF) {
		string nextLine;
		getline(file, nextLine);
		vector<string> token=ofSplitString(nextLine, "\t");
		if(token.size()>1){
			//if(!token[0].compare("ROOT_DIR")){
			//	ret = token[1];
			//}
      instNames.push_back(token[1]);
		}
		
	}
	file.close();
}

synthInstrument::synthInstrument(string title, unsigned char chan, unsigned char nt, bandBar * bnd)
{
  hold.set(1);
  hold.pause();
  setup(title, chan, nt);
  label.loadFont("fonts/DinC.ttf");
  label.setSize(30);
  label.setMode(OF_FONT_CENTER);
  subhead.loadFont("fonts/Arial.ttf");
  subhead.setSize(15);
  subhead.setMode(OF_FONT_CENTER);
  subhead.setMode(OF_FONT_TOP);
  kb.setup(800, 4);
  drop.setUnopenImage("images/dropdown.png");
  for (unsigned int i=0; i<instNames.size(); i++) {
    drop.setValue(instNames[i]);
  }
  bNewBlock=false;
  band=bnd;
  bNoteSelect=false;
}

void synthInstrument::changeProgram(int progNum)
{
  vector<unsigned char> message;
  message.push_back(MIDI_PROGRAM_CHANGE+base.channel);
  message.push_back(progNum);
  base.sendMidiMessage(message);
}

void synthInstrument::update()
{
  if(hold.justExpired()){
    bNoteSelect=true;
    kb.getKey().select(false);
    kb.pressKey(lastDrop().note-MIDI_KEYBOARD_START);
    if(band) band->setActive(this);
  }
}

void synthInstrument::draw(int _x, int _y)
{
  x=_x, y=_y;
  draw();
}

void synthInstrument::draw()
{
  base.draw(x,y);
  drop.draw(x+base.w+10,y+scroll.y);
  if(hold.running()){
    double k=hold.getPercent();
    ofSetColor(255*k, 255-255*k*k, 255-255*sqrt(k),((k>.5)?255:255*k*k*4));
    dragBlock &t=lastDrop();
    ofRingSegment(t.x+t.w/2, t.y+t.h/2, 40, 60, 90, 90+360*k);
  }
}

void synthInstrument::drawForeground()
{
  if(bNoteSelect){
    ofSetColor(0,0,0,128);
    ofRect(0,0,ofGetWidth(),ofGetHeight());
    ofSetColor(0x333333);
    ofRoundBox((ofGetWidth()-(kb.w+100))/2, (ofGetHeight()-(kb.h+200))/2, kb.w+100, kb.h+200, 4, .5);
    ofSetColor(229, 224, 15);
    label.drawString("SELECT A NOTE FOR THIS BLOCK", ofGetWidth()/2, (ofGetHeight()-kb.h)/2-50);
    
    kb.draw((ofGetWidth()-kb.w)/2, (ofGetHeight()-kb.h)/2);
    
    ofSetColor(255, 255, 255);
    subhead.drawString("To change note later, press and hold block for one second.", ofGetWidth()/2, (ofGetHeight()+kb.h)/2+50);
  }
}

void synthInstrument::drawBackground()
{
  if(bNoteSelect&&(ofGetElapsedTimeMillis()/250)%2){
    dragBlock &t=lastDrop();
    ofSetColor(251, 176, 23);
    ofRoundShape(t.x-5, t.y-5, t.w+10, t.h+10, (t.h+10)/4, true);
  }
  
  for (unsigned int i=0; i<blocks.size(); i++) {
		blocks[i].draw(base.y+base.relPos.y);
	}
}

bool synthInstrument::clickDown(int _x, int _y)
{
  bool ret=0;
  if(!bNoteSelect&&!drop.open){
    if(!bHolding&&base.over(_x, _y-scroll.y)){
      blocks.push_back(dragBlock(base));
      blocks[blocks.size()-1].w=80;
      blocks[blocks.size()-1].x=_x-blocks[blocks.size()-1].w/2;
      bNewBlock=true;
      ret=1;
    }
    else if(drop.clickDown(_x, _y)){
      if(band) band->setActive(this);
    }
    for (unsigned int i=0; i<blocks.size(); i++) {
      if(!bHolding&&blocks[i].clickDown(_x,_y)){
        play();
        lastBlock=i;
        bHolding=true;
        if(!bNewBlock) hold.reset(),hold.run(),xOrig=blocks[i].x;
      }
    }
  }
  else if(drop.open){
    drop.clickDown(_x, _y);
    if(drop.justSelected()) base.changeTitle(drop.getString()),changeProgram(drop.getChoiceNumber());
    if(band&&!drop.open) band->setActive(0);
  }
  else if(bNoteSelect){
    bool sel=false;
    if(kb.clickDown(_x, _y)){
      sel=true;
      lastDrop().note=kb.getKey().getNote()+MIDI_KEYBOARD_START,bNewBlock=false;
      base.note=lastDrop().note,play();
    }
    if(!sel){
      for (unsigned int i=0; i<blocks.size(); i++) {
        if(!bHolding&&blocks[i].over(_x,_y)){
          sel=true;
          lastBlock=i;
          kb.getKey().select(false);
          kb.pressKey(blocks[i].note-MIDI_KEYBOARD_START);
        }
      }
    }
    if(!bNewBlock&&!sel){
      bNoteSelect=false;
      if(band) band->setActive(0);
    }
  }

	return ret;
}

bool synthInstrument::clickUp()
{
  base.clickUp();
	bool ret=false;
  ret=releaseDraggedBlock();
  if(ret) hold.pause();
  if (ret&&bHolding&&bNewBlock) {
    bNoteSelect=true;
    if(band) band->setActive(this);
  }
  if(drop.clickUp()){
    
  }
  if(kb.clickUp()){
    bNoteSelect=false;
    lastDrop().note=kb.getKey().getNote()+MIDI_KEYBOARD_START;
    base.note=lastDrop().note,stop();
    if(band) band->setActive(0);
  }
	bHolding=false;
	return ret;
}

void synthInstrument::mouseMotion(int _x, int _y)
{
  if(!bNoteSelect){
    for (unsigned int i=0; i<blocks.size(); i++) {
      if(blocks[i].pressed()){
        if(abs((_x-blocks[i].relMouse.x)-xOrig)>5) hold.reset(),hold.pause();
      }  
        blocks[i].mouseMotion(_x,_y);
      
    }
  }
  else {
    kb.mouseMotion(_x, _y);
    lastDrop().note=kb.getKey().getNote()+MIDI_KEYBOARD_START;
    if(base.note!=lastDrop().note){
      cout << "changed key" << endl;
      stop();
      base.note=lastDrop().note,play();
    }
  }
}

bool synthInstrument::active(double pos)
{
  bool ret=false;
  for (unsigned int i=0; i<blocks.size(); i++) {
    if(blocks[i].justChanged(pos)){
      if (blocks[i].active(pos)) base.note=blocks[i].note,play();
      else stop();
    }
    else if(bPercussive&&base.isPlaying()&&ofGetElapsedTimeMillis()>=150+startPlay) stop();
  }
	return ret;
}
