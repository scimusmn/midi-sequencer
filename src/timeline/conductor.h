/*
 *  conductor.h
 *  Miditron
 *
 *  Created by Exhibits on 4/1/2554.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#pragma once

#include "timeline.h"
#include "../band.h"

class midiSequencer : public midiConductor, public ofInterObj{
	int divsPerMeasure;
	dallasScroll bar;
	ofButton	mark;
	ofFont		label;
	bandBar * band;
	double viewSize;
	double numMeasures;
	double measureLength;
	ofButton playBut;
	ofButton rewindBut;
	ofButton loopBut;
  
  dallasButton waltz;
  dallasButton blues;
  
  
	digitDisplay display;
  dallasSlider tempoSlide;
  
  ofRectangle topBar;
public:
	midiSequencer():midiConductor(),ofInterObj(){
	}
	void registerPlayback(bandBar * bnd);
	void setup(double nMeasures, double secondsPerMeasure, double pixPerSec, double vSize);
  void setTimeSignature(int beatsPerMeasure);
  void setTempo(double secondsPerMeasure);
  
  void addStrike(inst & Inst, ofTag & tag, double tempo);
  void loadSong(string filename);
  void addInstrumentTag(inst * Inst, ofXML & xml, string root);
  void saveSong(string filename);
  
	void drawMark();
	void draw(int _x, int _y);
  void drawControlBar(int _x, int _y, int _w, int _h);
	void drawDivs(bool full); 
	void update();
	bool clickDown(int _x, int _y);
	bool clickUp();
	bool mouseMotion(int _x, int _y);
  void drag(int _x, int _y);
	void snapTo(double & num);
	void snapTo(dragBlock & last);
	double getBarPosition();
	double cursor();
	void setScrollPos(double pos);
	void play();
	void pause();
  void resize();
};