/*
 *  band.h
 *  Miditron
 *
 *  Created by Exhibits on 3/30/2554.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#pragma once

#include "instruments/instrument.h"

class bandBar : public ofInterGroup{
protected:
	vector<inst *> instruments;
	bool bHolding;
	ofScrollBar bar;
	int viewSize;
	double xoff,yoff,yBot;
	ofImage sideBarBack;
	int lastInst;
	ofButton clearBut;
	double binWidth;
	double binHeight;
	double yBlockSpace;
	double xBlockSpace;
	double barW,yGap;
	int numBlocksPerBin;
	int numBins;
  inst * activeInst;
  
  ofButton scrollUp;
  ofButton scrollDown;
public:
	bandBar():ofInterGroup(){
    activeInst=0;
		lastInst=0;
	}
	~bandBar();
  inst * operator[](int i);
  int size();
	void setup();
	void setup(xmlParse * config);
  void setup(ofXML & xml);
	void addInstrument(string title, unsigned char channel, unsigned char nt, instType t);
	void drawInstruments();
	void drawBackground();
	void clear();
	void draw(int _x=0, int _y=0);
	bool clickDown(int _x, int _y);
	bool clickUp();
	void update(int disp, ofDirection dir);
	void update();
	void mouseMotion(int _x, int _y);
	void drag(int _x, int _y);
	void checkActives(double xPos);
	void setHeight();
  double verticleBlockSpace(){ return yBlockSpace;}
  
  void setActive(inst * currentInst);
	double getBottomPos();
	int farthestPoint();
	void stopAll();
	dragBlock & lastBlock();
  void scaleToTempo(double time);
	friend class conductor;
};