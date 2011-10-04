/*
 *  instrument.cpp
 *  Miditron
 *
 *  Created by Exhibits on 3/29/2554.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#include "instrument.h"

dragBlock & inst::operator[](int i)
{
	return blocks[i];
}

void inst::setPercussive(bool perc){
  bPercussive=perc;
  base.bPercussive=perc;
}

void inst::update(int disp, ofDirection dir)
{
	bool vert=(dir==OF_VERT);
	base.update();
	if(!vert) scroll.x=disp;
	else scroll.y=disp;
	if(vert) base.soundBlock::update(0.,int(scroll.y));
	else for (unsigned int i=0; i< blocks.size(); i++) {
		blocks[i].update(scroll.x,0);
	}
}

void inst::setMidi(unsigned char chan, unsigned char nt)
{
	base.setup(title, chan, nt);
	w=base.w;
	h=base.h;
}

void inst::setColor(unsigned long hex)
{
	base.color.set(hex);
}

void inst::setup(string objName, unsigned char chan, unsigned char nt)
{
	bHolding=false;
	title=objName;
	setMidi(chan,nt);
	lastBlock=0;
  scroll.y=scroll.x=0;
  bPercussive=false;
  tempo=1;
}
void inst::sendMidiMessage(vector<unsigned char> newMessage)
{
	base.sendMidiMessage(newMessage);
}

void inst::play(unsigned char vel)
{
  startPlay=ofGetElapsedTimeMillis();
	base.play(vel);
}

void inst::play()
{
  startPlay=ofGetElapsedTimeMillis();
	base.play();
}

void inst::scaleToTempo(double time)
{
  for (unsigned int i=0; i<blocks.size(); i++) {
    blocks[i].w*=time/tempo;
    blocks[i].x=(blocks[i].x-fullWidth)*time/tempo+fullWidth;
  }
  tempo=time;
}

void inst::stop()
{
	base.stop();
}

bool inst::releaseDraggedBlock(){
  bool ret=false;
  for (unsigned int i=0; i<blocks.size(); i++) {
    for (unsigned int j=0; j<blocks.size(); j++) {
      if(j!=i&&blocks[j].x>=blocks[i].x&&blocks[j].x<blocks[i].x+blocks[i].w){
        if(!bPercussive&&blocks[j].w+blocks[j].x-30>blocks[i].x+blocks[i].w) {
          blocks[j].w=blocks[j].w-((blocks[i].x+blocks[i].w)-blocks[j].x);
          blocks[j].x=blocks[i].x+blocks[i].w;
        }
        else if(bPercussive&&abs(blocks[j].x-blocks[i].x)<blocks[i].w/2&&blocks[j].pressed()){
          blocks.erase(blocks.begin()+j);
          lastBlock=-1;
        }
      }
    }
  }
  for (unsigned int i=0; i<blocks.size(); i++) {
		if(blocks[i].clickUp()){
			ret=true;
			lastBlock=i;
			if ((blocks[i].x+blocks[i].w/2<fullWidth&&!bNewBlock)||(blocks[i].x+blocks[i].w<fullWidth&&bNewBlock)) {
				ret=false;
				blocks.erase(blocks.begin()+i);
				lastBlock=-1;
			}
			stop();
		}
	}
  return ret;
}

//**************************************************//

instrument & instrument::operator=(const instrument & t)
{
	w=t.w;
	base=t.base;
	bHolding=t.bHolding;
	blocks=t.blocks;
	title=t.title;
	scroll.x=t.scroll.x,scroll.y=t.scroll.y;
	lastBlock=t.lastBlock;
	point=t.point;
	return (*this);
}

instrument::instrument(string objName, unsigned char chan, unsigned char nt):inst()
{
	setup(objName, chan, nt);
}

void instrument::resizeByFont(int fontSize)
{
	point=fontSize;
	base.header.setSize(point);
	base.w=max(float(150), base.header.stringWidth(title)+20);
	base.h=max(float(20), base.header.stringHeight("Kjg")+10);
	w=base.w;
	h=base.h;
}

void instrument::draw(int _x,int _y)
{
	x=_x, y=_y;
  draw();
}

void instrument::draw()
{
	base.draw(x,y);
  //ofRect(x, y, w, h);
	//if(!base.isPlaying()) ofSetColor(255, 0, 0);
//	else ofSetColor(0, 255, 0);
//	ofCircle(base.x, base.y+base.relPos.y, 10);
}

void instrument::drawBackground()
{
	for (unsigned int i=0; i<blocks.size(); i++) {
		blocks[i].draw(base.y+base.relPos.y);
	}
}

bool instrument::clickDown(int _x, int _y)
{
	bool ret=0;
	if(!bHolding&&base.over(_x, _y-scroll.y)){
		blocks.push_back(dragBlock(base));
    blocks[blocks.size()-1].x=_x-blocks[blocks.size()-1].w/2;
    bNewBlock=ret=true;
	}
	for (unsigned int i=0; i<blocks.size(); i++) {
		if(!bHolding&&blocks[i].clickDown(_x,_y)){
			play();
			bHolding=true;
		}
	}
	return ret;
}

bool instrument::clickUp()
{
	base.clickUp();
	bool ret=false;
  ret=releaseDraggedBlock();
	bNewBlock=bHolding=false;
	return ret;
}

void instrument::mouseMotion(int _x, int _y)
{
	for (unsigned int i=0; i<blocks.size(); i++) {
		blocks[i].mouseMotion(_x,_y);
	}
}

bool instrument::active(double xPos)
{
	bool ret=false;
  for (unsigned int i=0; i<blocks.size(); i++) {
    if(blocks[i].justChanged(xPos)){
      if (blocks[i].active(xPos)) play();
      else stop();
    }
    else if(bPercussive&&base.isPlaying()&&ofGetElapsedTimeMillis()>=20+startPlay) stop();
  }
	return ret;
}

