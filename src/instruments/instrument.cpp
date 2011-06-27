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
	title=t.title;
	header.loadFont("Arial.ttf");
	int pt=t.header.point;
	header.setSize(pt);
	bPressed=t.bPressed;
	color=t.color;
	xDisp=t.xDisp;
	yDisp=t.yDisp;
}

void soundBlock::operator=(const soundBlock & t)
{
	title=t.title;
	cSetup(t.x, t.y, t.w, t.h);
	header.loadFont("Arial.ttf");
	int pt=t.header.point;
	header.setSize(pt);
	bPressed=t.bPressed;
	color=t.color;
	xDisp=0;
	yDisp=0;
}

void soundBlock::setup(string objName)
{
	title=objName;
	header.loadFont("Arial.ttf");
	header.setSize(24);
	color.set(0, 112, 187);
	w=max(float(150), header.stringWidth(title)+20);
	h=max(float(20), header.stringHeight(title)+10);
	xDisp=0;
	yDisp=0;
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
	ofRoundBox(x+xDisp, y+yDisp, w, h, h/8., .2);
	ofSetColor(0, 0, 0);
	ofEnableSmoothing();
	ofRoundShape(x+xDisp, y+yDisp, w, h, h/8., false);
	ofDisableSmoothing();
	ofSetColor(255, 255, 255);
	if(header.stringWidth(title)<w){
		double scale=header.stringHeight(title);
		header.drawString(title, x+xDisp+5, y+yDisp+header.stringHeight("K")+(h-header.stringHeight(title))/2);
	}
}

void soundBlock::update(double x_dis, double y_dis)
{
	xDisp=x_dis;
	yDisp=y_dis;
}

bool soundBlock::clickDown(int _x, int _y)
{
	if(over(_x-xDisp, _y-yDisp)){
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
	if(bPressed) move(_x-xDisp, _y-yDisp);
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
	if (rhythmTimer.justExpired()&&bRepeat&&bPlaying) {
		play();
	}
	if (playTimer.justExpired()&&bRepeat&&bPlaying) {
		stop();
		bPlaying=true;
	}
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
	if (bRepeat) {
		playTimer.set(playTime);
		rhythmTimer.set(rhythmTime);
	}
}

void rhythmBlock::stop()
{
	play(0);
	bPlaying=false;
}

bool rhythmBlock::clickDown(int _x, int _y)
{
	bool ret=0;
	if(ret=over(_x-xDisp, _y-yDisp)){
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
	sizeAdj.setup(28, 42, "images/resize.png");
	bResizing=bActive=false;
}

dragBlock::dragBlock(int _x, int _w, rhythmBlock & t):soundBlock(t)
{
  sizeAdj.setup(28, 42, "images/resize.png");
	bResizing=bActive=false;
  x=_x;
  w=_w;
}

void dragBlock::draw(int _y)
{
	y=_y;
	//if(bActive) color=color+.2*255;
	soundBlock::draw();
	if(bActive){
		ofShade(x+xDisp+aPos, y, aPos, h, OF_LEFT, .5, false);
		ofShade(x+xDisp+aPos, y, w-aPos, h, OF_RIGHT, .5, false);
	}
	sizeAdj.draw(x+xDisp+w-sizeAdj.w/2, y+yDisp+(h-sizeAdj.h)/2);
	//if(bActive) color=color-.2*255;
}

bool dragBlock::clickDown(int _x, int _y)
{
	int ret=0;
	if (sizeAdj.clickDown(_x, _y)) {
		bResizing=true;
	}
	else if(soundBlock::clickDown(_x,_y)) ret=1;
	return ret;
}

bool dragBlock::clickUp()
{
	bool ret=bPressed;
	ret|=sizeAdj.clickUp();
	bResizing=false;
	bPressed=false;
	return ret;
}

void dragBlock::mouseMotion(int _x, int _y)
{
	if (bResizing){
		int wid=_x-(x+xDisp);
		w=(wid>30)?wid:30;
	}
	else if(bPressed) move(_x-xDisp, _y-yDisp);
}

bool dragBlock::active(double xPos){
	aPos=xPos-(x+xDisp);
	return bActive=xPos>=x+xDisp&&xPos<=x+xDisp+w;
}

bool dragBlock::justChanged(double xPos){
	bool ret=false;
  bool temp;
	aPos=xPos-(x+xDisp);
	temp=xPos>=x+xDisp&&xPos<=x+xDisp+w;
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
	mousePosX=t.mousePosX, yoff=t.yoff;
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
	base.h=max(float(20), base.header.stringHeight(title)+10);
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
	if(!base.isPlaying()) ofSetColor(255, 0, 0);
	else ofSetColor(0, 255, 0);
	//ofCircle(base.x, base.y+base.yDisp, 10);
}

void instrument::drawBackground()
{
	for (unsigned int i=0; i<blocks.size(); i++) {
		blocks[i].draw(base.y+base.yDisp);
	}
}

bool instrument::clickDown(int _x, int _y)
{
	bool ret=0;
	if(!bHolding&&base.over(_x, _y)&&!bDefault){
    cout << "Why are we here?" << endl;
		blocks.push_back(dragBlock(base));
		ret=1;
	}
	for (unsigned int i=0; i<blocks.size(); i++) {
		if(!bHolding&&blocks[i].clickDown(_x,_y)){
			play();
			bHolding=true;
			mousePosX=_x-blocks[i].x-scrollX;
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
			if (blocks[i].x<x+w) {
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
        if(blocks[j].w+blocks[j].x-30>blocks[i].x+blocks[i].w) blocks[j].w=blocks[j].w-((blocks[i].x+blocks[i].w)-blocks[j].x);
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
		//if(blocks[i].pressed()){
			blocks[i].mouseMotion(_x-mousePosX,_y);
		//}
	}
}

bool instrument::active(double xPos)
{
	bool ret=false;
	/*if(!bPercussive)
		for (unsigned int i=0; i<blocks.size(); i++) {
			ret|=blocks[i].active(xPos);
		}
	else {
		for (unsigned int i=0; i<blocks.size(); i++) {
			ret|=blocks[i].justChanged(xPos);
		}
	}*/
  for (unsigned int i=0; i<blocks.size(); i++) {
    if(blocks[i].justChanged(xPos)){
      if (blocks[i].active(xPos)) play();
      else stop();
    }
  }
	return ret;
}

void instrument::sendMidiMessage(vector<unsigned char> newMessage)
{
	base.sendMidiMessage(newMessage);
}

void instrument::play(unsigned char vel)
{
	base.play(vel);
}

void instrument::play()
{
	base.play();
}

void instrument::stop()
{
	base.stop();
	for (unsigned int i=0; i<blocks.size(); i++) {
		blocks[i].bActive=false;
	}
}