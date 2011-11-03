/*
 *  soundBlocks.cpp
 *  Miditron
 *
 *  Created by Exhibits on 9/13/11.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#include "soundBlocks.h"

extern ofColor white;
extern ofColor black;
extern ofColor blue;
extern ofColor yellow;
extern ofColor orange;


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
	header.loadFont("fonts/Arial.ttf"); 
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
	header.loadFont("fonts/Arial.ttf");
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
  ofRaised(.2);
	ofRoundedRect(x+relPos.x, y+relPos.y, w, h, h/8.);
	ofSetColor(0, 0, 0);
	ofEnableSmoothing();
  ofNoFill();
	ofRoundedRect(x+relPos.x, y+relPos.y, w, h, h/8.);
  ofFill();
	ofDisableSmoothing();
	ofSetColor(255, 255, 255);
	if(header.stringWidth(title)<=w-20&&h>20){
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
	bool ret=bPressed;
	if(bPressed) bPressed=false;
	return ret;
}

void soundBlock::changeTitle(string ttl)
{
  title=ttl;
  w=max(float(150), header.stringWidth(title)+20);
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
	channel=t.channel;
}

void rhythmBlock::setup(string title, unsigned char chan, unsigned char nt)
{
	soundBlock::setup(title);
	bPlaying=false;
	baseApp=ofGetAppPtr();
	setMidi(chan, nt, 1);
}

void rhythmBlock::setDelay(double dly)
{
	rhythmTime=dly;
}

void rhythmBlock::setMidi(unsigned char chan,unsigned char nt, double delay)
{
	note=nt;
	channel=chan;
	rhythmTime=delay;
	playTime=.1;
}

void rhythmBlock::update()
{
	
}

void rhythmBlock::sendMidiMessage(vector<unsigned char> newMessage)
{
	baseApp->midiToSend(newMessage);
}

void rhythmBlock::play(unsigned char vel)
{
	message.clear();
	message.push_back(MIDI_NOTE_ON+channel);
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
	bool ret=bPressed;
	if(bPressed) bPressed=false, stop();
	return ret;
}
//**************************************************//

dragBlock::dragBlock(rhythmBlock & t):soundBlock(t)
{
  note=t.note;
  setup(t);
}

dragBlock::dragBlock(int _x, int _w, rhythmBlock & t):soundBlock(t)
{
  note=t.note;
  setup(t);
  x=_x;
  w=_w;
}

void dragBlock::setup(rhythmBlock & t)
{
  rightAdj.setup(h, OF_VERT, "images/rightArrow.png");
  leftAdj.setup(h, OF_VERT, "images/leftArrow.png");
  if(bPercussive) w=80;
  bPressed=true;
  relMouse.x=0;
  bNewBlock=true;
	bResizing=bLResize=bActive=false;
}

void dragBlock::draw(int _y)
{
	y=_y;
	//if(bActive) color=color+.2*255;
  header.setSize(10);
	soundBlock::draw();
	if(bActive){
    ofSetShadowDarkness(.5);
		ofShade(x+relPos.x+aPos, y, aPos, h, OF_LEFT, false);
		ofShade(x+relPos.x+aPos, y, w-aPos, h, OF_RIGHT, false);
	}
  if(!bPercussive&&h>20){
    rightAdj.draw(x+relPos.x+w-rightAdj.w, y+relPos.y+(h-rightAdj.h)/2);
    if(w>30) leftAdj.draw(x+relPos.x, y+relPos.y+(h-leftAdj.h)/2);
  }
}

bool dragBlock::clickDown(int _x, int _y)
{
	int ret=0;
	if (!bPercussive&&rightAdj.clickDown(_x, _y)) {
    relMouse.x=x+w-_x;
		bResizing=true;
	}
  else if (!bPercussive&&leftAdj.clickDown(_x, _y)) {
    relMouse.x=_x-x;
		bLResize=true;
	}
	else if(soundBlock::clickDown(_x,_y)){
    ret=1;
    relMouse.x=_x-relPos.x-x;
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
		int wid=_x+relMouse.x-x;
		w=(wid>30)?wid:30;
	}
  else if (bLResize){
		int wid=(x+w)-(_x-relMouse.x);
		w=(wid>30)?wid:30;
    x=_x-relMouse.x;
	}
	else if(bPressed) move(_x-relMouse.x-relPos.x, _y-relPos.y);
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