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
  hold.set(2);
  hold.pause();
  setup(title, chan, nt);
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
    kb.pressKey(lastDrop().note);
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
  if(bNoteSelect){
    kb.draw((ofGetWidth()-kb.w)/2, (ofGetHeight()-kb.h)/2);
  }
  if(hold.running()){
    double k=hold.getPercent();
    ofSetColor(255*k, 255-255*k*k, 255-255*sqrt(k));
    dragBlock &t=lastDrop();
    ofRingSegment(t.x+t.w/2, t.y+t.h/2, 10, 20, 90, 90+360*k);
  }
}

void synthInstrument::drawBackground()
{
  for (unsigned int i=0; i<blocks.size(); i++) {
		blocks[i].draw(base.y+base.relPos.y);
	}
}

bool synthInstrument::clickDown(int _x, int _y)
{
  bool ret=0;
  if(!bNoteSelect&&!drop.open){
    if(!bHolding&&base.over(_x, _y)){
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
        hold.reset(),hold.run();
      }
    }
  }
  else if(drop.open){
    drop.clickDown(_x, _y-scroll.y);
    if(drop.justSelected()) base.changeTitle(drop.getString()),changeProgram(drop.getChoiceNumber());
    if(band&&!drop.open) band->setActive(0);
  }
  else if(bNoteSelect){
    if(kb.clickDown(_x, _y))
      lastDrop().note=kb.getKey().getNote(),bNewBlock=false;
    else if(!bNewBlock){
      bNoteSelect=false;
      if(band) band->setActive(0);;
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
        if(abs((_x-blocks[i].relMouse.x)-blocks[i].x)>5) hold.reset(),hold.pause();
      }  
        blocks[i].mouseMotion(_x,_y);
      
    }
  }
  else kb.mouseMotion(_x, _y);
}

bool synthInstrument::active(double pos)
{
  bool ret=false;
  for (unsigned int i=0; i<blocks.size(); i++) {
    if(blocks[i].justChanged(pos)){
      if (blocks[i].active(pos)) base.note=blocks[i].note+24,play();
      else stop();
    }
    else if(bPercussive&&base.isPlaying()&&ofGetElapsedTimeMillis()>=150+startPlay) stop();
  }
	return ret;
}
