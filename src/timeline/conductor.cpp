/*
 *  conductor.cpp
 *  Miditron
 *
 *  Created by Exhibits on 4/1/2554.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#include "conductor.h"
#include "../instruments/synthInst.h"

void midiSequencer::setup(double nMeasures, double secondsPerMeasure, double pixelsPerSec, double vSize)
{
	mark.w=mark.h=20;
	h=30;
	divsPerMeasure=3;
	w=vSize;
	label.loadFont("Arial.ttf");
	label.setSize(12);
	midiConductor::setup(nMeasures*secondsPerMeasure,pixelsPerSec);
	pps=pixelsPerSec;
	numMeasures=nMeasures;
	measureLength=secondsPerMeasure*pps;
	bar.setup(20, vSize, OF_HOR);
	bar.registerArea(vSize, playTime*pps);
	playBut.setup(64, 64, "images/play.png","images/pause.png");
	rewindBut.setup( 48, 48, "images/rewind.png");
	loopBut.setup(48, 48, "images/repeat.png");
  waltz.setup(64,OF_VERT, "images/slow.png");
  blues.setup(64,OF_VERT, "images/slow.png");
	display.setup(300, 8);
  tempoSlide.setup(30, 30);
  
  loopBut.setPressed(true);
}

void midiSequencer::resize(){
  if(band) {
    bar.setup(20, ofGetWidth()-band->w, OF_HOR);
    bar.registerArea(ofGetWidth()-band->w, playTime*pps);
    if(numMeasures*measureLength<2*(ofGetWidth()-band->w)){
      double secondsPerMeasure=measureLength/pps;
      measureLength=2*(ofGetWidth()-band->w)/numMeasures;
      pps=measureLength/secondsPerMeasure;
      midiConductor::setup(numMeasures*secondsPerMeasure,pps);
    }
    setTempo(1);
  }
}

void midiSequencer::setTimeSignature(int beatsPerMeasure)
{
  divsPerMeasure=beatsPerMeasure;
}

void midiSequencer::setTempo(double secondsPerMeasure)
{
  //measureLength=(ofGetWidth()-band->w)/numMeasures;
  //pps=measureLength/secondsPerMeasure;
  double pos=metronome.getPercent();
  bool wasPlaying=isPlaying();
  measureLength=secondsPerMeasure*pps;
  midiConductor::setup(numMeasures*secondsPerMeasure,pps);
  setCursorPercent(pos);
  if(wasPlaying) play();
  divsPerMeasure=4*int(secondsPerMeasure);
  bar.registerArea(w, playTime*pps);
  band->scaleToTempo(secondsPerMeasure);
}

void midiSequencer::registerPlayback(bandBar * bnd)
{
	band=bnd;
	bar.x=band->w;
	setScrollPos(0);
}

void midiSequencer::loadSong(string filename)
{
  ofXML xml;
  xml.loadFile(filename);
  bandBar & bnd=*band;
  bnd.clear();
  xml.setCurrentTag(";band");
  double tempo=ofToFloat(xml.getCurrentTag().getAttribute("tempo"));
  double currentTempo=1+tempoSlide.getPercent()*3;
  ofTag & tag=xml.getCurrentTag();
  for (unsigned int i=0; i<tag.size(); i++) {
    if(tag[i].getLabel()=="instrument"){
      string which=tag[i].getAttribute("name");
      bool isSynth=(tag[i].getAttribute("type")=="synth");
      for (unsigned int j=0; j<bnd.size(); j++) {
        if(bnd[j]->title==which){
          if(isSynth){
            as_synth(bnd[j])->changeProgram(ofToInt(tag[i].getAttribute("prog")));
          }
          for (unsigned int k=0; k<tag[i].size(); k++) {
            if(tag[i][k].getLabel()=="strike"){
              int beat=ofToInt(tag[i][k].getAttribute("beat"));
              int length=ofToInt(tag[i][k].getAttribute("length"));
              double divLength=measureLength/divsPerMeasure;
              if(beat<numMeasures*divsPerMeasure){
                bnd[j]->blocks.push_back(dragBlock(beat*divLength*currentTempo/tempo+x,length*divLength*currentTempo/tempo,bnd[j]->base));
                dragBlock & db=bnd[j]->blocks[bnd[j]->blocks.size()-1];
                if(isSynth){
                  db.note=ofToInt(tag[i][k].getAttribute("note"));
                  cout << int(db.note) << endl;
                }
                db.clickUp();
              }
            }
          }
        }
      }
    }
  }
}



void midiSequencer::saveSong(string filename)
{
  ofXML xml;
  bandBar & bnd=*band;
  xml.newCurrentTag("band");
  xml.addAttribute("tempo", ofToString(1+tempoSlide.getPercent()*3,1));
  for (unsigned int i=0; i<bnd.size(); i++) {
    xml.setCurrentTag(";band");
    xml.newCurrentTag("instrument");
    xml.addAttribute("name", bnd[i]->title);
    if((*bnd[i]).getType()==INST_SYNTH){
      xml.addAttribute("type","synth");
      xml.addAttribute("prog", ofToString(as_synth(bnd[i])->getProgramNumber()));
    }
    else xml.addAttribute("type","default");
    for (unsigned int j=0; j<bnd[i]->size(); j++) {
      xml.newCurrentTag("strike");
      double divLength=measureLength/divsPerMeasure-1;
      int bt=((*bnd[i])[j].x-x)/divLength;
      int lng=(*bnd[i])[j].w/divLength;
      xml.addAttribute("beat", ofToString(bt));
      xml.addAttribute("length", ofToString(lng));
      if((*bnd[i]).getType()==INST_SYNTH){
        xml.addAttribute("note", ofToString((*bnd[i])[j].note));
      }

      xml.popTag();
    }
  }
  xml.writeFile(filename);
}

void midiSequencer::drawMark()
{
	ofSetColor(255, 255, 0);
	ofTriangle(mark.x-mark.w/2, mark.y, mark.x+mark.w/2, mark.y, mark.x, mark.y+mark.h);
	ofSetColor(255, 255, 0,128);
	ofLine(mark.x, y, mark.x, ofGetHeight());
}

void midiSequencer::drawDivs(bool full)
{
	for (int i=0; i<numMeasures; i++) {
		if(!full){
			ofSetColor(0xFFFFFF);
			label.drawString(ofToString(i), x+measureLength*i+1-bar.getScrollPosition(), y+label.stringHeight(ofToString(i)));
		}
		for (int j=0; j<divsPerMeasure; j++) {
			if(j%2) ofSetColor(0x77,0x77,0,64);
			else ofSetColor(0x66,0x66,0,128);
      if(j==0) ofSetColor(128,128,128,200);
			ofLine(x+measureLength*i+measureLength*j/divsPerMeasure-bar.getScrollPosition(), \
				   (full)?y:(abs(j-double(divsPerMeasure)/2)<=.5)?y+10:(j==0)?y+5:y+15,\
				   x+measureLength*i+measureLength*j/divsPerMeasure-bar.getScrollPosition(), (!full)?y+h:ofGetHeight());
		}
	}
	ofSetColor(0xE0C39B);
	label.drawString(ofToString(int(numMeasures)), x+measureLength*numMeasures+1-bar.getScrollPosition(), y+label.stringHeight(ofToString(numMeasures)));
	ofSetColor(0x60,0x43,0x1B,128);
	ofLine(x+measureLength*numMeasures-bar.getScrollPosition(),y+5, x+measureLength*numMeasures-bar.getScrollPosition(), (!full)?y+h:ofGetHeight());
}

void midiSequencer::draw(int _x, int _y)
{
	x=_x;
	w=ofGetWidth()-x;
	y=_y;
	mark.y=y+10;
	
	band->drawBackground();
	//Draw measure lines
	
	drawDivs(true);
	
	band->drawInstruments();
	
#if F_YEAH_WOOD
	ofSetColor(0xA0835B);
#else
	ofSetColor(0x777777);
#endif
	ofRect(x, y, w, h);
	ofShade(x, y+h, 15, w, OF_UP, .4);
	ofShade(x, y+h, 5, w, OF_DOWN, .3);
	
	drawDivs(false);
	
	drawMark();
	
	// Bottom scroll bar
  ofSetShadowDarkness(.3);
	ofShade(_x, band->getBottomPos(), 5, w, OF_UP);
	
	int botY=band->getBottomPos();
	if(bar.available()){
		bar.draw(_x, band->getBottomPos());
		botY=bar.y+bar.h+2;
	}
	
	int botH=ofGetHeight()-botY;
	//Task bar on bottom
	ofSetColor(0x777777);
  
  ofSetShadowDarkness(.3);
	ofRect(x,botY, w, botH);
  ofShade(x, botY, botH,w, OF_DOWN);
	//ofShade(x, bar.y+bar.h+2, 5, w, OF_UP, .3);
	ofShade(x,botY, 10, w, OF_DOWN, .3);
	
	double indent=4;
	label.setMode(OF_FONT_CENTER);
	//ofShadowRounded(x+w/2-playBut.w/2-indent/2, botY+(botH/2-playBut.h/2)-indent/2, playBut.w+indent, playBut.h+indent, playBut.w/2+indent/2, 1);
  ofSetShadowDarkness(.3);
  
  //_-_-_-_-_ play button drawing _-_-_-_-_
	ofShadowCircle(playBut.x+playBut.w/2, playBut.y+playBut.h/2, playBut.w/2, indent);
  playBut.draw(x+w/2-playBut.w/2, botY+(botH-playBut.h)/2);
  if(playBut.pressed())
    label.drawString("pause", playBut.x+playBut.w/2, playBut.y+playBut.h+15);
  else label.drawString("play", playBut.x+playBut.w/2, playBut.y+playBut.h+15);
  
  //_-_-_-_-_ rewind button _-_-_-_-_
  ofShadowCircle(rewindBut.x+rewindBut.w/2, rewindBut.y+rewindBut.h/2, rewindBut.w/2, indent);
	rewindBut.draw(x+w/2-playBut.w/2-50, botY+(botH-rewindBut.h)/2);
	label.drawString("reset", rewindBut.x+rewindBut.w/2, rewindBut.y+rewindBut.h+15);
	
  //_-_-_-_-_ loop button drawing _-_-_-_-_
  ofShadowCircle(loopBut.x+loopBut.w/2, loopBut.y+loopBut.h/2, loopBut.w/2, indent);
	loopBut.draw(x+w/2-playBut.w/2-100, botY+(botH-loopBut.h)/2);
	label.drawString("loop", loopBut.x+loopBut.w/2, loopBut.y+loopBut.h+15);
  
  //_-_-_-_-_ song1 draw
  ofShadowRounded(waltz.x, waltz.y, waltz.w, waltz.h, waltz.h/4, indent);
	waltz.draw(x+w-waltz.w-blues.w-100, botY+(botH-waltz.h*.5)/2);
  
  //_-_-_-_-_song2 draw _-_-_-_-_
  ofShadowRounded(blues.x, blues.y, blues.w, blues.h, blues.h/4, indent);
	blues.draw(x+w-blues.w-50, botY+(botH-blues.h*.5)/2);
	
  label.setSize(20);
  ofSetColor(255, 255, 255);
  label.drawString("load song", (waltz.x+(blues.x+blues.w))/2, loopBut.y-5);
  
  ofSetColor(220,220,220);
  
  //_-_-_-_-_ tempo slider draw
  ofShadowRounded(tempoSlide.x, tempoSlide.y, tempoSlide.w, tempoSlide.h, tempoSlide.h/4, indent);
  tempoSlide.draw(x+100, botY+(botH-tempoSlide.h)/2,300,10);
  ofSetColor(255, 255, 255);
  label.drawString("tempo", tempoSlide.x+tempoSlide.w/2, tempoSlide.y+tempoSlide.h+35);
	
  ofSetShadowDarkness(1);
  ofShadowRounded(display.x, display.y, display.w, display.h, display.h/4, indent);
  
	ofSetColor(0, 0, 0);
  ofRaised(.2);
	ofRoundedRect(display.x-10, display.y-10, display.w+20, display.h+20, 5);
  ofFlat();
	ofSetColor(0, 128, 200);
	label.setMode(OF_FONT_LEFT);
	label.setSize(12);
	
	int secs=metronome.getElapsed();
	display.draw(ssprintf("%02i:%02i.%02i",(secs/1000/60),(secs/1000)%60,(secs%1000/10)), x+w/2+playBut.w/2+40, botY+(botH-display.h)/2);
}

void midiSequencer::update()
{
  bar.update();
	band->update(-getBarPosition(),OF_HOR);
	if(isPlaying()){
		band->checkActives(cursor()+band->w);
    if(!loopBut.pressed()&&cursor()+band->w>=numMeasures*measureLength+x)
      pause(),reset();
  }
	if(loopBut.pressed()&&cursor()>band->farthestPoint()-x)
		reset(),setScrollPos(0);
	if(metronome.justExpired()){
		reset();
    band->stopAll();
  }
	midiConductor::update();
	double temp=metronome.getElapsedf()*pps-w/2;
	if (bPlaying&&cursorPos>=w/2+bar.getScrollPosition()&&bar.setScrollPosition(temp));
	mark.x=cursor()+x;
}

bool midiSequencer::clickDown(int _x, int _y)
{
	bool ret=0;
	if(mark.clickDown(_x, _y));
	else if(_y<mark.y+mark.h&&_x>x){
		mark.setPressed(true);
		setCursorPosition(_x-x+getBarPosition());
		mark.x=cursorPos+x;
	}
	else if(bar.clickDown(_x, _y));
	else if(playBut.toggle(_x, _y)){
		bPlaying=!playBut.pressed();
		if(isPlaying()) midiConductor::pause(),band->stopAll();
		else midiConductor::play();
	}
	else if(rewindBut.clickDown(_x, _y)) reset(), setScrollPos(0);
	else if(loopBut.toggle(_x, _y));
	else if(waltz.clickDown(_x, _y)){
    loopBut.setPressed(true);
    loadSong("waltz.xml");
  }
	else if(blues.clickDown(_x, _y)){
    loopBut.setPressed(true);
    loadSong("slow.xml");
  }
	else if(tempoSlide.clickDown(_x, _y)){
    setTempo(1+tempoSlide.getPercent()*3);
  }
	return ret;
}

void midiSequencer::play()
{
	playBut.setPressed(true);
	midiConductor::play();
}

void midiSequencer::pause()
{
	playBut.setPressed(false);
	midiConductor::pause();
}

bool midiSequencer::clickUp()
{
	bool ret=0;
	mark.clickUp();
	rewindBut.clickUp();
	bar.clickUp();
	waltz.clickUp();
	blues.clickUp();
	if(tempoSlide.clickUp()) setTempo(1+tempoSlide.getPercent()*3);
	return ret;
}

bool midiSequencer::mouseMotion(int _x, int _y)
{
	bool ret=0;
	if(mark.pressed()){
		int newPos=_x-x+getBarPosition();
		if(_x<x) newPos=0;
		else if(_x>ofGetWidth()) newPos=ofGetWidth();
		setCursorPosition(newPos);
		mark.x=cursorPos+x;
	}
  else if(tempoSlide.pressed()){
    tempoSlide.drag(_x, _y);
    setTempo(1+tempoSlide.getPercent()*3);
  }
	else if(bar.mouseMotion(_x, _y));
	return ret;
}

void midiSequencer::drag(int _x, int _y)
{
}

void midiSequencer::snapTo(double & num)
{
	for (int i=0; i<numMeasures; i++) {
		for (int j=0; j<divsPerMeasure; j++) {
			if (abs((x+measureLength*i+measureLength*j/divsPerMeasure)-num)<=measureLength/(divsPerMeasure*2)) {
				num=x+measureLength*i+measureLength*j/divsPerMeasure;
			}
		}
	}
}

void midiSequencer::snapTo(dragBlock & last)
{
	double & num = last.x;
	double & wid = last.w;
  //int k=0;
	for (int i=0; i<numMeasures; i++) {
		for (int j=0; j<divsPerMeasure; j++) {
      if(num<x) num=x;
			if(abs((x+measureLength*i+measureLength*j/divsPerMeasure)-num)<=measureLength/(divsPerMeasure*2)) {
				num=x+measureLength*i+measureLength*j/divsPerMeasure;
			}
			if(abs((x+measureLength*i+measureLength*j/divsPerMeasure)-(num+wid))<=measureLength/(divsPerMeasure*2)) {
				wid=x+measureLength*i+measureLength*j/divsPerMeasure-num;
        if(!wid) wid=measureLength/divsPerMeasure;
			}
			if(num>x+measureLength*numMeasures){
				num=x+measureLength*numMeasures-wid;
			}
			if(num+wid>x+measureLength*numMeasures){
				//num=x+measureLength*numMeasures-wid;
				wid=x+measureLength*numMeasures-num;
			}
		}
	}
}

double midiSequencer::getBarPosition()
{
	return bar.getScrollPosition();
}

double midiSequencer::cursor()
{
	return cursorPos-getBarPosition();
}

void midiSequencer::setScrollPos(double pos)
{
	bar.setScrollPosition(pos);
}