/*
 *  band.cpp
 *  Miditron
 *
 *  Created by Exhibits on 3/30/2554.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#include "band.h"

bandBar::~bandBar()
{
	stopAll();
}

void bandBar::setup()
{
	
}

double bandBar::getBottomPos()
{
	return y+h-yBot;
}

inst * bandBar::operator[](int i)
{
  return instruments[i];
}

int bandBar::size()
{
  return instruments.size();
}

void bandBar::setHeight(double hgt,double top, double bot)
{
	h=(hgt)?hgt:ofGetHeight();
	yoff=top;
	yBot=bot;
	viewSize=h-(yoff+yBot);
	xoff=30;
	xGap=bar.w+2;
	yGap=20;
	xBlockSpace=10;
	yBlockSpace=10;
	numBins=1;
	for (unsigned int i=0; i<instruments.size(); i++) {
		binWidth=max(binWidth,instruments[i]->w+xBlockSpace*2);
		binHeight=max(binHeight, instruments[i]->h+yBlockSpace*2);
	}
	w=(xGap+binWidth)*numBins+xGap;
	double fullSize=binHeight*instruments.size();
	bar.setup(20, viewSize, OF_VERT);
	bar.registerArea(viewSize,fullSize);
  for (unsigned int i=0; i<instruments.size(); i++) {
    instruments[i]->setBandWidth(w);
  }
  
}

void bandBar::setup(ofXML & xml)
{
	clearBut.setup("clear blocks","fonts/Arial.ttf",24);
	clearBut.setAvailable(true);
	sideBarBack.loadImage("images/sidebar.jpg");
  xml.setCurrentTag(";band");
	string font=xml.getCurrentTag().getAttribute("font");
	ofTag & tag=xml.getCurrentTag();
  bHolding=false;
  double maxWid=0;
	for (unsigned int i=0; i<tag.size(); i++) {
		if (tag[i].getLabel()=="instrument") {
			string col=tag[i].getAttribute("color");
			string title=tag[i].getAttribute("name");
			long color=strtol(col.c_str(),NULL,0);
			int curInst=instruments.size();
			unsigned char note, channel;
			bool percussive=false;
      bool synth=false;
			double delay=0;
			map<string,int> list;
			list["note"]=0;
			list["channel"]=1;
      list["synth"]=2;
			list["delay"]=3;
			list["dropdown"]=4;
			list["percussive"]=5;
			for(int j=0;j<tag[i].size();j++){
				string node[2]={tag[i][j].getLabel(),tag[i][j].getValue()};;
				switch (list.find(node[0])->second) {
					case 0:
						note=ofToInt(node[1]);
						break;
					case 1:
						channel=ofToInt(node[1]);
						break;
          case 2:
            synth=true;
            break;

					case 3:
						delay=ofToFloat(node[1]);
						break;
					case 4:
						//ddGroup.push_back(ofDropDown(&xml->child[i]));
						break;
					case 5:
						percussive=true;
						break;
					default:
						break;
				}
			}
			addInstrument(title,channel,note);
			instruments[curInst]->setPercussive(percussive);
			instruments[curInst]->setColor(color);
      //maxWid=max(maxWid,instruments[curInst]->w);
		}
	}
	setHeight();
}

void bandBar::addInstrument(string title, unsigned char channel, unsigned char nt)
{
	instruments.push_back( new instrument(title,channel,nt));
	setHeight();
}

void bandBar::drawBackground()
{
	for (unsigned int i=0; i<instruments.size(); i++) {
#if F_YEAH_WOOD
		if(i%2) ofSetColor(0x80,0x63,0x3B,128);
		else ofSetColor(0xA0,0x83,0x5B,128);
#else
		if(i%2) ofSetColor(0x66,0x66,0x66,128);
		else ofSetColor(0x44,0x44,0x44,128);
#endif
    //ofSetColor(instruments[i]->getColor()-.3*255);
    //ofColor k=instruments[i]->getColor();
		ofRect(instruments[i]->x, instruments[i]->y-3, ofGetWidth(), instruments[i]->h+6);
    //ofSetColor(k.r, k.g, k.b,64);
    //ofRect(instruments[i]->x, instruments[i]->y+instruments[i]->yoff+instruments[i]->h/2-5, ofGetWidth(), 10);
	}
}

void bandBar::drawInstruments()
{
	for (unsigned int i=0; i<instruments.size(); i++) {
		instruments[i]->drawBackground();
	}
}

void bandBar::draw(int _x, int _y)
{
	x=_x,y=_y;
	
	//sidebar background
	ofSetColor(255, 255, 255);
	sideBarBack.draw(x+w-(sideBarBack.height/h)*sideBarBack.width, y,(sideBarBack.height/h)*sideBarBack.width,h);
	
	//
	ofSetColor(0x80633B);
	ofRect(x+xGap, y+yoff, w-xoff-10, viewSize);
	ofShade(x+w, y+yoff, 10, viewSize, OF_LEFT, .3);
	
	for (unsigned int i=0; i<instruments.size(); i++) {
		instruments[i]->draw(x+xGap+xBlockSpace,y+yoff+yBlockSpace +binHeight*(i));
		double tmpY=instruments[i]->y+binHeight+instruments[i]->vertScrollPos()-yBlockSpace;
		ofShade(x+xGap, tmpY, 3,binWidth, OF_UP, .3);
		ofShade(x+xGap, tmpY, 3, binWidth, OF_DOWN, .3,false);
    //ofSetColor(0, 0, 0,128);
    //ofRect(x+xGap, tmpY-5, w-xGap, 10);
	}
	
	ofShade(x+w-20, y+yoff, 10, viewSize, OF_LEFT, .3);
	
	//Box at top of sidebar
	ofSetColor(230, 230, 230);
	ofShadeBox(x, y, w, yoff, OF_DOWN, .4);
	ofShade(x, y+yoff, 10, w, OF_DOWN, .3);
	
	//box at bottom of sidebar
	ofRoundBox(x-10, getBottomPos(), w+10, yBot, 10, .4);
	ofShade(x, getBottomPos(), 10, w, OF_UP, .3);
	ofShade(x, getBottomPos(), 15, w, OF_DOWN, .2);
	
	if(bar.available())
		bar.draw(x,y+yoff);
	
	//Shades over ends of the scroll bar
	ofShade(x, y+yoff, 5, bar.w+4, OF_DOWN, .3);
	ofShade(x, y+yoff+viewSize, 5, bar.w+4, OF_UP, .3);
	
	//shade to right of scrollbar
	ofShade(x+xGap, y+yoff, 5, viewSize, OF_RIGHT, .3);
	
	clearBut.draw((w-clearBut.w)/2, getBottomPos()+((yBot)-clearBut.h)/2);
}

bool bandBar::clickDown(int _x, int _y)
{
	for (unsigned int i=0; i<instruments.size(); i++) {
		if(!bHolding&&_y>yoff&&_y<yoff+viewSize&&instruments[i]->clickDown(_x,_y)){
			bHolding=true;
			lastInst=i;
		}
	}
	if(clearBut.clickDown(_x, _y))
		clear();
	if(bar.available())
		bar.clickDown(_x, _y);
}

bool bandBar::clickUp()
{
	bool ret=false;
	for (unsigned int i=0; i<instruments.size(); i++) {
		if(instruments[i]->clickUp()){
			ret=true;
			lastInst=i;
		}
	}
	bHolding=false;
	clearBut.clickUp();
	bar.clickUp();
	return ret;
}

void bandBar::update()
{
	bar.update();
}

void bandBar::update(int disp, ofDirection dir)
{
	for (unsigned int i=0; i<instruments.size(); i++) {
		instruments[i]->update(disp,dir);
	}
	update();
}

void bandBar::mouseMotion(int _x, int _y)
{
	for (unsigned int i=0; i<instruments.size(); i++) {
		instruments[i]->mouseMotion(_x,_y);
	}
	if(bar.mouseMotion(_x,_y)){
		for (unsigned int i=0; i<instruments.size(); i++) {
			instruments[i]->update(-bar.getScrollPosition(),OF_VERT);
		}
	}
}

double bandBar::farthestPoint()
{
	double ret=0;
	for (unsigned int i=0; i<instruments.size(); i++) {
		for (unsigned int j=0; j<instruments[i]->size(); j++) {
			ret=max(ret,instruments[i][j].x+instruments[i][j].w);
		}
	}
	return ret;
}

void bandBar::clear()
{
	for (unsigned int i=0; i<instruments.size(); i++) {
		instruments[i]->clear();
	}
}

void bandBar::drag(int _x, int _y)
{
	
}

void bandBar::stopAll()
{
	for (unsigned int i=0; i<instruments.size(); i++) {
		instruments[i]->stop();
	}
}

void bandBar::checkActives(double xPos)
{
	for (unsigned int i=0; i<instruments.size(); i++) {
		instruments[i]->active(xPos);
	}
}

dragBlock & bandBar::lastBlock()
{
	return instruments[lastInst]->lastDrop();
}

void bandBar::scaleToTempo(double time)
{
  for (unsigned int i=0; i<instruments.size(); i++) {
    instruments[i]->scaleToTempo(time);
  }
}
