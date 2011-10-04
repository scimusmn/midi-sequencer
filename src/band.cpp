/*
 *  band.cpp
 *  Miditron
 *
 *  Created by Exhibits on 3/30/2554.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#include "band.h"
#include "instruments/synthInst.h"

bandBar::~bandBar()
{
	stopAll();
}

void bandBar::setup()
{
	
}

double bandBar::getBottomPos()
{
	//return y+h-yBot;
  return y+yoff+viewSize;
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
  int barWid=30;
	yoff=top;
	yBot=bot;
  xoff=barWid+10;
	xGap=barWid+2;
	yGap=20;
	xBlockSpace=10;
	yBlockSpace=10;
	numBins=1;
  binWidth=0;
	for (unsigned int i=0; i<instruments.size(); i++) {
		binWidth=max(binWidth,instruments[i]->w+xBlockSpace*2);
		binHeight=max(binHeight, instruments[i]->h+yBlockSpace*2);
	}
	w=(xGap+binWidth)*numBins+xGap;
  binWidth+=(barWid-xBlockSpace*2);
	double fullSize=binHeight*instruments.size();
  
  scrollUp.setup(w,OF_HOR, "images/mInstUp.jpg");
  scrollDown.setup(w,OF_HOR,"images/mInstDown.jpg");
  
  yoff=top+scrollUp.h;
  
  viewSize=h-(yoff+yBot)-scrollDown.h;
  
	bar.setup(barWid, viewSize, OF_VERT);
	bar.registerArea(viewSize,fullSize);
  for (unsigned int i=0; i<instruments.size(); i++) {
    instruments[i]->setBandWidth(w);
  }
  
  if(!bar.available()) {
    scrollUp.setup(0,OF_HOR, "images/mInstUp.jpg");
    scrollDown.setup(0,OF_HOR,"images/mInstDown.jpg");
  }
  
}

void bandBar::setup(ofXML & xml)
{
  loadInstruments("midiPrograms.ini");
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
			addInstrument(title,channel,note, synth);
			instruments[curInst]->setPercussive(percussive);
			instruments[curInst]->setColor(color);
      //maxWid=max(maxWid,instruments[curInst]->w);
		}
	}
	setHeight();
}

void bandBar::addInstrument(string title, unsigned char channel, unsigned char nt, bool synth)
{
	if(!synth) instruments.push_back( new instrument(title,channel,nt));
  else instruments.push_back( new synthInstrument(title,channel,nt,this));
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
		ofRect(instruments[i]->x, instruments[i]->y-bar.getScrollPosition()-3, ofGetWidth(), instruments[i]->h+6);
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
	//sideBarBack.draw(x+w-(sideBarBack.height/h)*sideBarBack.width, y,(sideBarBack.height/h)*sideBarBack.width,h);
  ofSetColor(0x333333);
  ofRect(x, y, w, h);
  ofSetColor(229, 224, 15,128);
  double s=10;
  for (int i=0; i<w/s; i++) {
    ofLine(x+w-i*s, y, x+w-i*s, y+h);
  }
  for (int i=0; i<h/s; i++) {
    ofLine(x, y+i*s, x+w, y+i*s);
  }
  ofShade(x+w, y+yoff, 10, viewSize+scrollDown.h, OF_RIGHT, .3);
	
	//ofSetColor(0x80633B);
  ofSetColor(0x33, 0x33, 0x33,223);
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
  ofShade(x+w,getBottomPos(), 10, ofGetHeight()-y, OF_RIGHT, .4);
  //shade to right of scrollbar
	ofShade(x+xGap, y+yoff, 5, viewSize, OF_RIGHT, .3);
  
  if(activeInst) activeInst->draw();
	
	//Box at top of sidebar
	ofSetColor(230, 230, 230);
	ofShadeBox(x, y, w, yoff, OF_DOWN, .4);
	ofShade(x, y+yoff, 10, w, OF_DOWN, .3);
  
  //Box at bottom of sidebar
	ofSetColor(230, 230, 230);
	ofShadeBox(x, y+yoff+viewSize, w, yoff, OF_DOWN, .4);
	
	//box at bottom of sidebar
	ofRoundBox(x-10, getBottomPos()+scrollDown.h, w+10, yBot, 2, .4);
	ofShade(x, getBottomPos(), 10, w, OF_UP, .3);
	ofShade(x, getBottomPos()+scrollDown.h, 15, w, OF_DOWN, .2);
	
	if(bar.available()){
    scrollUp.draw(x,y+yoff-scrollUp.h);
		bar.draw(x,y+yoff);
    scrollDown.draw(x,y+yoff+viewSize);
  }
	
	//Shades over ends of the scroll bar
	ofShade(x, y+yoff, 5, bar.w+4, OF_DOWN, .3);
	ofShade(x, y+yoff+viewSize, 5, bar.w+4, OF_UP, .3);
	
	
	clearBut.draw((w-clearBut.w)/2, getBottomPos()+scrollDown.h+((yBot)-clearBut.h)/2);
  
  for (unsigned int i=0; i<instruments.size(); i++) {
    instruments[i]->drawForeground();
  }
}

bool bandBar::clickDown(int _x, int _y)
{
	bool ret=0;
  if(!activeInst){
    for (unsigned int i=0; i<instruments.size(); i++) {
      if(!bHolding&&_y>y+yoff&&_y<y+yoff+viewSize&&instruments[i]->clickDown(_x,_y)){
        bHolding=true;
        lastInst=i;
      }
    }
  }
  else {
    activeInst->clickDown(_x, _y);
  }

	if(clearBut.clickDown(_x, _y))
		clear();
	if(bar.available())
		bar.clickDown(_x, _y);
  if(scrollUp.getAvailable()){
    if(scrollUp.clickDown(_x, _y)){
      bar.setScrollPosition(bar.getScrollPosition()-binHeight);
      for (unsigned int i=0; i<instruments.size(); i++) {
        instruments[i]->update(-bar.getScrollPosition(),OF_VERT);
      }
  }
  }
  if(scrollDown.getAvailable()){
    if(scrollDown.clickDown(_x, _y)){
      bar.setScrollPosition(bar.getScrollPosition()+binHeight);
      for (unsigned int i=0; i<instruments.size(); i++) {
        instruments[i]->update(-bar.getScrollPosition(),OF_VERT);
      }
    }
  }
	return ret;
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
  scrollUp.clickUp();
  scrollDown.clickUp();
	bHolding=false;
	clearBut.clickUp();
	bar.clickUp();
	return ret;
}

void bandBar::update()
{
	bar.update();
  scrollUp.setAvailable((bar.getScrollPosition()>binHeight));
  scrollDown.setAvailable((bar.getScrollPosition()<(bar.getFullSize()-viewSize-binHeight)));
  for (unsigned int i=0; i<instruments.size(); i++) {
		instruments[i]->update();
	}
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

void bandBar::setActive(inst * currentInst)
{
  activeInst=currentInst;
}

double bandBar::farthestPoint()
{
	double ret=0;
	for (unsigned int i=0; i<instruments.size(); i++) {
		for (unsigned int j=0; j<instruments[i]->size(); j++) {
			ret=max(ret,(*instruments[i])[j].x+(*instruments[i])[j].w+(*instruments[i])[j].relPos.x);
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
