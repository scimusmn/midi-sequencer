/*
 *  instrument.cpp
 *  Miditron
 *
 *  Created by Exhibits on 3/29/2554.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#include "instrument.h"


soundBlock::soundBlock(string objName):ofInterObj()
{
	setup(objName);
}

soundBlock::~soundBlock()
{
}

soundBlock::soundBlock(const soundBlock & t):ofInterObj(t.x, t.y, t.w, t.h)
{
	*(this)=t;
}

void soundBlock::operator=(const soundBlock & t)
{
	title=t.title;
	cSetup(t.x, t.y, t.w, t.h);
	header.loadFont("Arial.ttf"); 
  header.setMode(OF_FONT_TOP);
  header.setMode(OF_FONT_CENTER);
	int pt=t.header.point;
	header.setSize(pt);
	bPressed=t.bPressed;
	color=t.color;
	relPos.x=0;
	relPos.y=0;
  bPercussive=t.bPercussive;
}

void soundBlock::setup(string objName)
{
	title=objName;
	header.loadFont("Arial.ttf");
  header.setMode(OF_FONT_TOP);
  header.setMode(OF_FONT_CENTER);
	header.setSize(24);
	color.set(0, 112, 187);
	w=max(float(150), header.stringWidth(title)+20);
	h=max(float(20), header.stringHeight("Kjg")+10);
	relPos.x=0;
	relPos.y=0;
  bPercussive=false;
}

void soundBlock::draw(int _x, int _y)
{
	x=_x;
	y=_y;
	draw();
}

void soundBlock::draw()
{
	ofSetColor((bPressed)?color+.2*255:color);
	ofRoundBox(x+relPos.x, y+relPos.y, w, h, h/8., .2);
	ofSetColor(0, 0, 0);
	ofEnableSmoothing();
	ofRoundShape(x+relPos.x, y+relPos.y, w, h, h/8., false);
	ofDisableSmoothing();
	ofSetColor(255, 255, 255);
	if(header.stringWidth(title)<=w-20){
		double scale=header.stringHeight(title);
		header.drawString(title, x+relPos.x+w/2, y+relPos.y+(h-header.stringHeight("Kjg"))/2);
	}
}

void soundBlock::update(double x_dis, double y_dis)
{
	relPos.x=x_dis;
	relPos.y=y_dis;
}

bool soundBlock::clickDown(int _x, int _y)
{
	if(over(_x-relPos.x, _y-relPos.y)){
		bPressed=true;
	}
	return bPressed;
}

bool soundBlock::clickUp()
{
	if(bPressed) bPressed=false;
	
}

void soundBlock::mouseMotion(int _x, int _y)
{
	if(bPressed) move(_x-relPos.x, _y-relPos.y);
}

//**************************************************//

rhythmBlock::rhythmBlock(const rhythmBlock & t):soundBlock(t)
{
	baseApp=t.baseApp;
	bPlaying=t.bPlaying;
	note=t.note;
	bRepeat=t.bRepeat;
	channel=t.channel;
}

void rhythmBlock::setup(string title, unsigned char chan, unsigned char nt,  bool repeat)
{
	soundBlock::setup(title);
	bPlaying=false;
	baseApp=ofGetAppPtr();
	setMidi(chan, nt, repeat, 1);
}

void rhythmBlock::setDelay(double dly)
{
	rhythmTime=dly;
}

void rhythmBlock::setMidi(unsigned char chan,unsigned char nt, bool repeat, double delay)
{
	note=nt;
	channel=chan;
	bRepeat=repeat;
	rhythmTime=delay;
	playTime=.1;
}

void rhythmBlock::update()
{
	/*if (rhythmTimer.justExpired()&&bRepeat&&bPlaying) {
		play();
	}
	if (playTimer.justExpired()&&bRepeat&&bPlaying) {
		stop();
		bPlaying=true;
	}*/
}

void rhythmBlock::sendMidiMessage(vector<unsigned char> newMessage)
{
	baseApp->midiToSend(newMessage);
}

void rhythmBlock::play(unsigned char vel)
{
	message.clear();
	message.push_back(MIDI_NOTE_ON+0);
	message.push_back(note);
	message.push_back(vel);
	sendMidiMessage(message);
}

void rhythmBlock::play()
{
	//if(!bPlaying)
	play(45);
	bPlaying=true;
}

void rhythmBlock::stop()
{
	play(0);
	bPlaying=false;
}

bool rhythmBlock::clickDown(int _x, int _y)
{
	bool ret=0;
	if(ret=over(_x-relPos.x, _y-relPos.y)){
		bPressed=true;
		play();
	}
	return ret;
}

bool rhythmBlock::clickUp()
{
	if(bPressed) bPressed=false, stop();
	
}
//**************************************************//

dragBlock::dragBlock(const rhythmBlock & t):soundBlock(t)
{
	rightAdj.setup(16, 32, "images/rightArrow.png");
  leftAdj.setup(16, 32, "images/leftArrow.png");
  if(bPercussive) w=40;
  bPressed=true;
  relMouse.x=0;
	bResizing=bLResize=bActive=false;
}

dragBlock::dragBlock(int _x, int _w, rhythmBlock & t):soundBlock(t)
{
  rightAdj.setup(16, 32, "images/rightArrow.png");
  leftAdj.setup(16, 32, "images/leftArrow.png");
  if(bPercussive) w=40;
  bPressed=true;
  relMouse.x=0;
	bResizing=bLResize=bActive=false;
  x=_x;
  w=_w;
}

void dragBlock::draw(int _y)
{
	y=_y;
	//if(bActive) color=color+.2*255;
  header.setSize(10);
	soundBlock::draw();
	if(bActive){
		ofShade(x+relPos.x+aPos, y, aPos, h, OF_LEFT, .5, false);
		ofShade(x+relPos.x+aPos, y, w-aPos, h, OF_RIGHT, .5, false);
	}
  if(!bPercussive){
    rightAdj.draw(x+relPos.x+w-rightAdj.w-5, y+relPos.y+(h-rightAdj.h)/2);
    if(w>30) leftAdj.draw(x+relPos.x+5, y+relPos.y+(h-leftAdj.h)/2);
  }
}

bool dragBlock::clickDown(int _x, int _y)
{
	int ret=0;
	if (rightAdj.clickDown(_x, _y)) {
    relMouse.x=x+w-_x;
		bResizing=true;
	}
  else if (leftAdj.clickDown(_x, _y)) {
    relMouse.x=_x-x;
		bLResize=true;
	}
	else if(soundBlock::clickDown(_x,_y)){
    ret=1;
    relMouse.x=_x-x;
  }
	return ret;
}

bool dragBlock::clickUp()
{
	bool ret=bPressed;
	ret|=rightAdj.clickUp();
  ret|=leftAdj.clickUp();
	bResizing=bLResize=false;
	bPressed=false;
	return ret;
}

void dragBlock::mouseMotion(int _x, int _y)
{
	if (bResizing){
		int wid=_x+relMouse.x-(x+relPos.x);
		w=(wid>30)?wid:30;
	}
  else if (bLResize){
		int wid=(x+relPos.x+w)-(_x-relMouse.x);
		w=(wid>30)?wid:30;
    x=_x-relMouse.x;
	}
	else if(bPressed) move(_x-relPos.x-relMouse.x, _y-relPos.y);
}

bool dragBlock::active(double xPos){
	aPos=xPos-(x+relPos.x);
	return bActive=xPos>=x+relPos.x&&xPos<=x+relPos.x+w;
}

bool dragBlock::justChanged(double xPos){
	bool ret=false;
  bool temp;
	aPos=xPos-(x+relPos.x);
	temp=xPos>=x+relPos.x&&xPos<=x+relPos.x+w;
	if(bActive!=temp) bActive=temp, ret=true;
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
  yoff=t.yoff;
	scrollX=t.scrollX,scrollY=t.scrollY;
	lastBlock=t.lastBlock;
	bDefault=t.bDefault;
	point=t.point;
}

instrument::instrument(string objName, unsigned char chan, unsigned char nt, bool repeat)
{
	setup(objName, chan, nt, repeat);
	
}

void instrument::setup(string objName, unsigned char chan, unsigned char nt, bool repeat)
{
	bHolding=false;
	title=objName;
	setMidi(chan,nt,repeat);
	lastBlock=0;
	bDefault=false;
  bPercussive=false;
}

void instrument::setColor(unsigned long hex)
{
	base.color.set(hex);
}

dragBlock & instrument::operator[](int i)
{
	return blocks[i];
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

void instrument::setMidi(unsigned char chan, unsigned char nt,bool repeat)
{
	base.setup(title, chan, nt, repeat);
	w=base.w;
	h=base.h;
}

void instrument::setDelay(double dly)
{
	base.setDelay(dly);
}

void instrument::update(int x_disp, int y_disp)
{
	scrollX=x_disp, scrollY=y_disp;
	base.soundBlock::update(0.,int(scrollY));
	base.update();
	for (unsigned int i=0; i< blocks.size(); i++) {
		blocks[i].update(x_disp,0);
	}
}

void instrument::update()
{
	base.update();
}

void instrument::update(int disp, ofDirection dir)
{
	bool vert=(dir==OF_VERT);
	base.update();
	if(!vert) scrollX=disp;
	else scrollY=disp;
	if(vert) base.soundBlock::update(0.,int(scrollY));
	else for (unsigned int i=0; i< blocks.size(); i++) {
		blocks[i].update(scrollX,0);
	}
}

void instrument::draw(int _x,int _y)
{
	x=_x, y=_y;
	draw();
}

void instrument::draw()
{
	base.draw(x,y);
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
	if(!bHolding&&base.over(_x, _y-scrollY)&&!bDefault){
    cout << "Why are we here?" << endl;
		blocks.push_back(dragBlock(base));
    blocks[blocks.size()-1].x=_x-blocks[blocks.size()-1].w/2;
		ret=1;
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
	for (unsigned int i=0; i<blocks.size(); i++) {
		if(blocks[i].clickUp()){
			ret=true;
			lastBlock=i;
			if (blocks[i].x<fullWidth) {
				ret=false;
				blocks.erase(blocks.begin()+i);
				lastBlock=-1;
			}
			stop();
		}
	}
  for (unsigned int i=0; i<blocks.size(); i++) {
    for (unsigned int j=0; j<blocks.size(); j++) {
      if(blocks[j].x>blocks[i].x&&blocks[j].x<blocks[i].x+blocks[i].w){
        if(!bPercussive&&blocks[j].w+blocks[j].x-30>blocks[i].x+blocks[i].w) blocks[j].w=blocks[j].w-((blocks[i].x+blocks[i].w)-blocks[j].x);
        blocks[j].x=blocks[i].x+blocks[i].w;
      }
    }
  }
	bHolding=false;
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
    else if(bPercussive&&base.isPlaying()&&ofGetElapsedTimeMillis()>=150+startPlay) stop();
  }
	return ret;
}

void instrument::sendMidiMessage(vector<unsigned char> newMessage)
{
	base.sendMidiMessage(newMessage);
}

void instrument::play(unsigned char vel)
{
  startPlay=ofGetElapsedTimeMillis();
	base.play(vel);
}

void instrument::play()
{
  startPlay=ofGetElapsedTimeMillis();
	base.play();
}

void instrument::stop()
{
	base.stop();
}

void instrument::setPercussive(bool perc){
  bPercussive=perc;
  base.bPercussive=perc;
}