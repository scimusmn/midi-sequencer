/*
 *  band.h
 *  Miditron
 *
 *  Created by Exhibits on 3/30/2554.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#pragma once

#include "instrument.h"

class bandBar : public ofInterGroup{
protected:
	vector<instrument> instruments;
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
	double xGap,yGap;
	int numBlocksPerBin;
	int numBins;
public:
	bandBar():ofInterGroup(){
		lastInst=0;
	}
	~bandBar();
  instrument & operator[](int i);
  int size();
	void setup();
	void setup(xmlParse * config);
  void setup(ofXML & xml);
	void addInstrument(string title, unsigned char channel, unsigned char nt);
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
	void setHeight(double height=0, double top=30, double bot=95);
	double getBottomPos();
	double farthestPoint();
	void stopAll();
	dragBlock & lastBlock();
  void scaleToTempo(double time);
	friend class conductor;
};