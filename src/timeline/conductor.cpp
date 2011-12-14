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
#include "../instruments/groupInstrument.h"

extern ofColor white;
extern ofColor black;
extern ofColor blue;
extern ofColor gray;
extern ofColor yellow;

extern double divLength;

void midiSequencer::setup(double nMeasures, double secondsPerMeasure, double pixelsPerSec, double vSize)
{
	mark.w=mark.h=20;
	h=30;
	divsPerMeasure=3;
	w=vSize;
	label.loadFont("DinC.ttf");
	label.setSize(12);
	midiConductor::setup(nMeasures*secondsPerMeasure,pixelsPerSec);
	pps=pixelsPerSec;
	numMeasures=nMeasures;
	measureLength=secondsPerMeasure*pps;
	bar.setup(30, vSize, OF_HOR);
	bar.registerArea(vSize, playTime*pps);
	playBut.setup(64, 64, "images/play2.png","images/pause2.png");
	rewindBut.setup( 48, 48, "images/rewind2.png");
	loopBut.setup(48, 48, "images/repeat.png");
  //waltz.setup(64,OF_VERT, "images/slow.png");
  //blues.setup(64,OF_VERT, "images/slow.png");
  waltz.setup("Song 1", 20);
  blues.setup("Song 2", 20);
	display.setup(300, 8);
  tempoSlide.setup(40,40);
  
  loopBut.setPressed(true);
  divLength=measureLength/divsPerMeasure;
}

void midiSequencer::resize(){
  if(band) {
    cout << ofGetHeight()-band->getBottomPos()<<endl;
    bar.setup(41, ofGetWidth()-band->w, OF_HOR);
    bar.registerArea(ofGetWidth()-band->w, playTime*pps);
    bar.changePadding();
    if(numMeasures*measureLength<2*(ofGetWidth()-band->w)){
      double secondsPerMeasure=measureLength/pps;
      measureLength=2*(ofGetWidth()-band->w)/numMeasures;
      pps=measureLength/secondsPerMeasure;
      midiConductor::setup(numMeasures*secondsPerMeasure,pps);
    }
    setTempo(1);
  }
  divLength=measureLength/divsPerMeasure;
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
  band->y=band->getControlBox().height-h;
	setScrollPos(0);
}

void midiSequencer::addStrike(inst & Inst, ofTag & tag, double tempo)
{
  double currentTempo=1+tempoSlide.getPercent()*3;
  for (unsigned int k=0; k<tag.size(); k++) {
    if(tag[k].getLabel()=="strike"){
      int beat=ofToInt(tag[k].getAttribute("beat"));
      int length=ofToInt(tag[k].getAttribute("length"));
      double divLength=measureLength/divsPerMeasure;
      if(beat<numMeasures*divsPerMeasure){
        Inst.blocks.push_back(dragBlock(beat*divLength*currentTempo/tempo+x,length*divLength*currentTempo/tempo,Inst.base));
        dragBlock & db=Inst.blocks[Inst.blocks.size()-1];
        /*if(isSynth){
          db.note=ofToInt(tag[i][k].getAttribute("note"));
          cout << int(db.note) << endl;
        }*/
        db.clickUp();
      }
    }
  }
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
      bool isGroup=(tag[i].getAttribute("type")=="group"||tag[i].getAttribute("type")=="synth");
      for (unsigned int j=0; j<bnd.size(); j++) {
        inst * cur=0;
        if(bnd[j]->title==which){
          if(!isGroup){
            cur=bnd[j];
            addStrike(*cur, tag[i],tempo);
          }
          else{
            groupInst &curGroup=*as_groupInst(bnd[j]);
            int prog = ofToInt(tag[i].getAttribute("prog"));
            if(tag[i].getAttribute("type")=="synth") curGroup.changeProgram(prog);
            for (unsigned int k=0; k<tag[i].size(); k++) {
              if(tag[i][k].getLabel()=="instrument"){
                string whichSub=tag[i][k].getAttribute("name");
                for (unsigned int l=0; l<curGroup.noteSize(); l++) {
                  if(curGroup(l).title==whichSub){
                    addStrike(curGroup(l), tag[i][k],tempo);
                  }
                }
                curGroup.loseFocus();
              }
            }
          }
        }
      }
    }
  }
}

void midiSequencer::addInstrumentTag(inst * Inst, ofXML & xml, string root)
{
  xml.newCurrentTag("instrument");
  xml.addAttribute("name", Inst->title);
  if(Inst->getType()==INST_GROUP||Inst->getType()==INST_SYNTH){
    if(Inst->getType()==INST_GROUP) xml.addAttribute("type","group");
    else if(Inst->getType()==INST_SYNTH) xml.addAttribute("type","synth");
    xml.addAttribute("prog", ofToString(as_groupInst(Inst)->getProgramNumber()));
    groupInst & curGroup=*as_groupInst(Inst);
    for (unsigned int i=0; i<curGroup.noteSize(); i++) {
      addInstrumentTag(&curGroup(i), xml,root);
    }
  }
  else{ 
    xml.addAttribute("type","default");
    for (unsigned int j=0; j<Inst->size(); j++) {
      xml.newCurrentTag("strike");
      double divLength=measureLength/divsPerMeasure-1;
      int bt=((*Inst)[j].x-x)/divLength;
      int lng=(*Inst)[j].w/divLength;
      xml.addAttribute("beat", ofToString(bt));
      xml.addAttribute("length", ofToString(lng));
      xml.popTag();
    }
  }
  xml.popTag();
}

void midiSequencer::saveSong(string filename)
{
  ofXML xml;
  bandBar & bnd=*band;
  xml.newCurrentTag("band");
  xml.addAttribute("tempo", ofToString(1+tempoSlide.getPercent()*3,1));
  for (unsigned int i=0; i<bnd.size(); i++) {
    addInstrumentTag(bnd[i], xml, ";band");
  }
  xml.writeFile(filename);
}

void midiSequencer::drawMark()
{
	ofSetColor(255, 255, 0);
	ofTriangle(mark.x-mark.w/2, mark.y, mark.x+mark.w/2, mark.y, mark.x, mark.y+mark.h);
	ofSetColor(255, 255, 0,128);
	ofLine(mark.x, topBar.y, mark.x, ofGetHeight());
}

void midiSequencer::drawDivs(bool full)
{
	for (int i=0; i<numMeasures; i++) {
		if(!full){
			ofSetColor(yellow);
      label.setMode(OF_FONT_RIGHT);
			label.drawString(ofToString(i), x+measureLength*i-2-bar.getScrollPosition(), topBar.y+label.stringHeight(ofToString(i)));
		}
		for (int j=0; j<divsPerMeasure; j++) {
			ofSetColor(yellow.opacity(.5-.25*(j%2)));
      if(j==0) ofSetColor(128,128,128,200);
			ofLine(x+measureLength*i+measureLength*j/divsPerMeasure-bar.getScrollPosition(), \
				   (full)?topBar.y:(abs(j-double(divsPerMeasure)/2)<=.5)?topBar.y+10:(j==0)?topBar.y+5:topBar.y+15,\
				   x+measureLength*i+measureLength*j/divsPerMeasure-bar.getScrollPosition(), (!full)?topBar.y+h:ofGetHeight());
		}
	}
	ofSetColor(yellow);
	label.drawString(ofToString(int(numMeasures)), x+measureLength*numMeasures+1-bar.getScrollPosition(), topBar.y+label.stringHeight(ofToString(numMeasures)));
	ofSetColor(yellow.opacity(.5));
	ofLine(x+measureLength*numMeasures-bar.getScrollPosition(),topBar.y+5, x+measureLength*numMeasures-bar.getScrollPosition(), (!full)?topBar.y+h:ofGetHeight());
}

void midiSequencer::draw(int _x, int _y)
{
	x=_x;
	w=ofGetWidth()-x;
	y=_y;
	mark.y=topBar.y+10;
  topBar.x=x;
  topBar.y=y;
  topBar.width=w;
  topBar.height=h;
  
  bool bot=band->controlsAtBottom;
  
  
  if(!bot){
    topBar.y=y+band->getControlBox().height-h;
  }
	
	band->drawBackground();
	//Draw measure lines
	
	drawDivs(true);
	
	band->drawInstruments();
	
	
  if(bot) drawControlBar(x, band->getControlBox().y+bar.h, w, band->getControlBox().height-bar.h);
  else drawControlBar(x, band->getControlBox().y, w, band->getControlBox().height-topBar.height);

	ofSetColor(gray);
  ofRect(topBar);
  drawBorder(topBar);
	
	drawDivs(false);
	
	drawMark();
  
  if(bar.available()){
    ofSetColor(white);
		if(!bot) bar.draw(_x, band->getBottomPos()+1);
    else bar.draw(_x, band->getControlBox().y);
	}
  
	
	//Task bar on bottom
}

void midiSequencer::drawControlBar(int _x, int _y, int _w, int _h)
{
  ofSetColor(0x777777);
  double boxPad=(ofGetWidth()-x-(waltz.w+blues.w+50+playBut.w+rewindBut.w+display.w+35+tempoSlide.w))/3;
  ofRectangle loadSeq(x,_y,waltz.w+blues.w+50+boxPad,_h);
  ofRectangle buttons(loadSeq.x+loadSeq.width,_y,playBut.w+rewindBut.w+display.w+35+boxPad,_h);
  ofRectangle tempBox(buttons.x+buttons.width,_y,tempoSlide.w+boxPad,_h);
  
  ofSetColor(gray);
	ofRect(x,_y, w, _h);
  
  ofSetColor(black);
  drawHatching(x, _y, w, _h, 10, 1);
  
  drawBorder(loadSeq);
  drawBorder(buttons);
  drawBorder(tempBox);
	
	
  //_-_-_-_-_//_-_-_-_-_//_-_-_-_-_//_-_-_-_-_//_-_-_-_-_//_-_-_-_-_//_-_-_-_-_//_-_-_-_-_
  //_-_-_-_-_//_-_-_-_-_//_-_-_-_-_  buttons draw  //_-_-_-_-_//_-_-_-_-_//_-_-_-_-_
  double indent=4;
	label.setMode(OF_FONT_CENTER);
  
  //_-_-_-_-_ rewind button _-_-_-_-_
	rewindBut.draw(buttons.x+boxPad/2, buttons.y+(buttons.height-rewindBut.h)/2);
  ofSetColor(yellow);
	label.drawString("rewind", rewindBut.x+rewindBut.w/2, rewindBut.y+rewindBut.h+15);
  
  //_-_-_-_-_ play button drawing _-_-_-_-_
  playBut.draw(rewindBut.x+rewindBut.w+5, _y+(_h-playBut.h)/2);
  ofSetColor(yellow);
  if(playBut.pressed())
    label.drawString("pause", playBut.x+playBut.w/2, playBut.y+playBut.h+15);
  else label.drawString("play", playBut.x+playBut.w/2, playBut.y+playBut.h+15);
	
  /*//_-_-_-_-_ loop button drawing _-_-_-_-_
	loopBut.draw(x+w/2-playBut.w/2-100, _y+(_h-loopBut.h)/2);
	label.drawString("loop", loopBut.x+loopBut.w/2, loopBut.y+loopBut.h+15);*/
  
  ofSetColor(black);
  ofRoundedRect(display.x-10, display.y-10, display.w+20, display.h+20, (display.h+20)/8);
	ofSetColor(blue);
	
	int secs=metronome.getElapsed();
	display.draw(ssprintf("%02i:%02i.%02i",(secs/1000/60),(secs/1000)%60,(secs%1000/10)), playBut.x+playBut.w+30, _y+(_h-display.h)/2);
  ofSetColor(yellow);
  label.drawString("Time in seconds", display.x+display.w/2, display.y+display.h+25);
  
  label.setMode(OF_FONT_LEFT);
  
  
  //_-_-_-_-_//_-_-_-_-_//_-_-_-_-_//_-_-_-_-_//_-_-_-_-_//_-_-_-_-_//_-_-_-_-_//_-_-_-_-_
  //_-_-_-_-_//_-_-_-_-_//_-_-_-_-_  load sequence draw  //_-_-_-_-_//_-_-_-_-_//_-_-_-_-_
  
  
  //_-_-_-_-_ song1 draw
	waltz.draw(loadSeq.x+boxPad/2, blues.y);
  
  //_-_-_-_-_song2 draw _-_-_-_-_
	blues.draw(waltz.x+waltz.w+50, loadSeq.y+(loadSeq.height-blues.h*.5)/2);
	
  label.setSize(20);
  ofSetColor(yellow);
  label.setMode(OF_FONT_CENTER);
  label.drawString("Load example sequence", (waltz.x+(blues.x+blues.w))/2, rewindBut.y-5);
  
  ofSetColor(220,220,220);
  
  //_-_-_-_-_//_-_-_-_-_//_-_-_-_-_//_-_-_-_-_//_-_-_-_-_//_-_-_-_-_//_-_-_-_-_//_-_-_-_-_
  //_-_-_-_-_//_-_-_-_-_//_-_-_-_-_   Tempo slider draw  //_-_-_-_-_//_-_-_-_-_//_-_-_-_-_
  
  tempoSlide.draw(tempBox.x+boxPad/2, tempBox.y+(tempBox.height-tempoSlide.h)/2+20,300,10);
  ofSetColor(yellow);
  label.setMode(OF_FONT_BOT);
  label.drawString("Slide to change tempo", tempoSlide.x+tempoSlide.w/2, tempoSlide.y-40);
  label.setSize(12);
  
}

void midiSequencer::update()
{
  //bar.update();
	band->update(-getBarPosition(),OF_HOR);
	if(isPlaying()){
		band->checkActives(cursor()+band->w);
    if(!loopBut.pressed()&&cursor()+band->w>=numMeasures*measureLength+x)
      pause(),reset();
  }
  else {
    band->checkActives(-200);
  }

	if(loopBut.pressed()&&cursor()>band->farthestPoint()-x&&!band->empty()&&bPlaying)
		reset(),setScrollPos(0);
	if(metronome.justExpired()){
		reset();
    band->stopAll();
  }
	midiConductor::update();
	double temp=metronome.getElapsedf()*pps-w/2;
	if (bPlaying&&cursorPos>=w/2+bar.getScrollPosition()){
    bar.setScrollPosition(temp);
  }
	mark.x=cursor()+x;
}

bool midiSequencer::clickDown(int _x, int _y)
{
	bool ret=0;
	if(mark.clickDown(_x, _y));
	else if(_y>topBar.y&&_y<mark.y+mark.h&&_x>x){
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
	else if(rewindBut.clickDown(_x, _y)) reset(), band->stopAll(), setScrollPos(0);
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