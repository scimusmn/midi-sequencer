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
#include "instruments/groupInstrument.h"

extern ofColor white;
extern ofColor black;
extern ofColor gray;
extern ofColor yellow;

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
  return bin.y+bin.height;
}

ofRectangle bandBar::getControlBox()
{
  return controls;
}

inst * bandBar::operator[](int i)
{
  return instruments[i];
}

int bandBar::size()
{
  return instruments.size();
}

void bandBar::adjustSize()
{
  bool bot=controlsAtBottom;
  h=ofGetHeight()-y;
  bar.w=rightBorder=20;
  controls.height=170;
  bin.y=y+((bot)?scrollUp.h:controls.height);
  blockMargin.x=10;
  blockMargin.y=10;
  cell.y=cell.x=0;
  int fullHeight=0;
  for (unsigned int i=0; i<instruments.size(); i++) {
    inst & t=*instruments[i];
    fullHeight+=t.h+blockMargin.y*2;
		cell.x=max(double(cell.x),t.w+blockMargin.x*2+((t.getType()==INST_GROUP)?t.base.h+10:0));
		cell.y=max(double(cell.y), t.base.h+blockMargin.y*2);
	}
  //cell.x+=(bar.w-blockMargin.x*2);
  
  bin.width=bar.w+cell.x;
  w=bin.width+rightBorder;
  controls.width=w;
  
  scrollUp.setup(w,OF_HOR, "images/mInstUp.jpg");
  scrollDown.setup(w,OF_HOR,"images/mInstDown.jpg");
  
  bin.height=h-(controls.height+((bot)?scrollUp.h:scrollDown.h));
  
  bar.setup(bar.w, bin.height, OF_VERT);
	bar.registerArea(bin.height,fullHeight);
  
  for (unsigned int i=0; i<instruments.size(); i++) {
    instruments[i]->setBandWidth(bin.width);
    if(!bar.available()) instruments[i]->update(-bar.getScrollPosition(),OF_VERT);
  }
  
}

void bandBar::setup(ofXML & xml)
{
  controlsAtBottom=0;
  activeInst=0;
  loadInstruments("midiPrograms.ini");
	clearBut.setup("clear screen","fonts/Arial.ttf",24);
	clearBut.setAvailable(true);
	sideBarBack.loadImage("images/sidebar.jpg");
  xml.setCurrentTag(";band");
	string font=xml.getCurrentTag().getAttribute("font");
	ofTag & tag=xml.getCurrentTag();
  ofTag * supportTag=0;
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
			double delay=0;
      instType type=INST_DEFAULT;
			map<string,int> list;
			list["note"]=0;
			list["channel"]=1;
      list["synth"]=2;
			list["delay"]=3;
			list["dropdown"]=4;
			list["percussive"]=5;
      list["group"]=6;
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
            type=INST_SYNTH;
            supportTag=&tag[i][j];
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
          case 6:
            type=INST_GROUP;
            supportTag=&tag[i][j];
            break;

					default:
						break;
				}
			}
			addInstrument(title,channel,note, type);
			instruments[curInst]->setPercussive(percussive);
			instruments[curInst]->setColor(color);
      if(type==INST_GROUP||type==INST_SYNTH) as_groupInst(instruments[curInst])->addInstruments(*supportTag);
      if(type==INST_SYNTH) as_groupInst(instruments[curInst])->loadInstruments(supportTag->getAttribute("file"));
    }
      //maxWid=max(maxWid,instruments[curInst]->w);
	}
	//setHeight();
  adjustSize();
}

void bandBar::addInstrument(string title, unsigned char channel, unsigned char nt, instType t)
{
	if(t==INST_DEFAULT) instruments.push_back( new instrument(title,channel,nt));
  else if(t==INST_GROUP||t==INST_SYNTH) instruments.push_back( new groupInst(title,channel,nt,this));
    
	//setHeight();
  adjustSize();
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

void drawGrid(float x,float y,float w,float h,float s)
{
  /*for (int i=0; i<w/s; i++) {
    ofLine(x+i*s, y, x+i*s, y+h);
  }
  for (int i=0; i<h/s; i++) {
    ofLine(x, y+i*s, x+w, y+i*s);
  }*/
  ofSetColor(black.opacity(.25));
  drawHatching(x,y,w,h,s,s);
}

void bandBar::draw(int _x, int _y)
{
  bool bot=controlsAtBottom;
  
  controls.x=bin.x=x=_x,y=_y;
  bin.y=y+scrollUp.h;
  controls.y=bin.y+bin.height;
  if(!bot){
    bin.y=y+controls.height;
    controls.y=y;
  }
  float binEdge=bin.x+bin.width;
  
  //_-_-_-_-_ sidebar background _-_-_-_-_
  ofSetColor(0x333333);
  ofRect(x, y, w, h);
  ofSetColor(229, 224, 15,64);
  drawGrid(x+w-rightBorder, bin.y, rightBorder, bin.height, rightBorder/2);
  
  ofSetColor(229, 224, 15,128);
  drawGrid(bin.x, bin.y, w-rightBorder, bin.height+10, rightBorder/4);
  
  //_-_-_-_-_ shade to the right of the sidebar
  ofSetShadowDarkness(.5);
  ofShade(x+w, bin.y, 15, bin.height, OF_RIGHT);
  
  //_-_-_-_-_ semi-transparent rectangle under the instrument base blocks
  ofSetColor(0x33, 0x33, 0x33,223);
	ofRect(bin);
	ofShade(x+w, bin.y, 10, bin.height, OF_LEFT, .3);
  
  //_-_-_-_-_ draw each of the instruments
  double yOff=0;
	for (unsigned int i=0; i<instruments.size(); i++) {
    inst & Inst=*instruments[i];
		Inst.draw(x+bar.w+blockMargin.x,bin.y+blockMargin.y +yOff);
    yOff+=((Inst.h<cell.y)?cell.y:Inst.h+blockMargin.y*2);
		double tmpY=Inst.y+((Inst.h<cell.y)?cell.y:Inst.h+blockMargin.y*2)+Inst.vertScrollPos()-blockMargin.y;
    
    ofSetShadowDarkness(.2);
		ofShade(x, tmpY, 3,bin.width, OF_UP);
		ofShade(x, tmpY, 3, bin.width, OF_DOWN,false);
	}
  
  //_-_-_-_-_ border shading
  ofSetShadowDarkness(.3);
  ofShade(binEdge, bin.y, 10, bin.height, OF_LEFT);
  ofShade(x+w, y+yoff, 10, bin.height, OF_LEFT);
  ofShade(binEdge, bin.y, 10, bin.height, OF_RIGHT);
  
  //_-_-_-_-_ shade over conductor controls
  ofSetShadowDarkness(.4);
  ofShade(controls.x+controls.width,controls.y, 10, controls.height, OF_RIGHT);
  ofShade(x+w,bin.y+bin.height, 10, ofGetHeight()-getBottomPos(), OF_RIGHT);
  
  //_-_-_-_-_ if there is an active instrument, draw it now
  //if(activeInst) activeInst->draw();
  
  //_-_-_-_-_ box at opposite end of scroll than controls
  ofRectangle t(x,((!bot)?bin.y+bin.height:y),w,scrollUp.h);
	ofSetColor(230, 230, 230);
	ofRect(t);
  ofSetShadowDarkness(.4);
  ofShade(t.x,t.y, t.height,t.width, OF_DOWN);
  
  //_-_-_-_-_ control box, to hold the clear button
  ofRect(controls);
  ofShade(controls.x, controls.y, controls.height, controls.width, OF_DOWN);
  ofShade(controls.x+controls.width, controls.y, 10, controls.height, OF_LEFT);
  ofShade(controls.x, controls.y+controls.height, 10, controls.width, OF_UP);
  if(!bot&&bar.available()) ofShade(bin.x, scrollUp.y, 10, w, OF_UP);
  //if(!bot) ofShade(controls.x, controls.y+controls.height, 10, controls.width, OF_DOWN);
  //else ofShade(controls.x, controls.y, 10, controls.width, OF_UP);
  ofShade(bin.x, bin.y, 10, w, OF_DOWN);
  ofShade(bin.x, bin.y+bin.height, 10, w, OF_UP);
  
	
	if(bar.available()){
    scrollUp.draw(x,bin.y-scrollUp.h);
    //if(bar.pressed()){
    ofSetColor(255, 255, 255,128);
    bar.draw(x,bin.y);
    //}
    scrollDown.draw(x,bin.y+bin.height);
  }
  
  int butDisp=((bot)?scrollDown.h:-scrollUp.h);
  ofSetShadowDarkness(.5); 
  ofShadowRounded(clearBut.x, clearBut.y, clearBut.w, clearBut.h, clearBut.h/2, 3);
  clearBut.draw((w-clearBut.w)/2, controls.y+butDisp+((controls.height-butDisp)-clearBut.h)/2);
  
  for (unsigned int i=0; i<instruments.size(); i++) {
    instruments[i]->drawForeground();
  }
}

int lastY=0;

bool bandBar::clickDown(int _x, int _y)
{
	bool ret=0;
  
  //_-_-_-_-_ control clickdowns
  if(clearBut.clickDown(_x, _y))
		ret=1,clear();
	//if(bar.available()&&!ret);
		//ret=bar.clickDown(_x, _y);
  if((scrollUp.getAvailable()||scrollDown.getAvailable())&&!ret){
    if(scrollUp.clickDown(_x, _y)){
      cout << bar.getScrollPosition()-cell.y << " should be the new pos\n";
      ret=1,bar.setScrollPosition(bar.getScrollPosition()-cell.y);
      cout << bar.getScrollPosition() << " is the new pos\n";
    }
    else if(scrollDown.clickDown(_x, _y))
      ret=1,bar.setScrollPosition(bar.getScrollPosition()+cell.y);
    
    if(ret) for (unsigned int i=0; i<instruments.size(); i++) {
      instruments[i]->update(-bar.getScrollPosition(),OF_VERT);
    }
  }
  
  //_-_-_-_-_instrument clickdowns
  if(!ret){
    for (unsigned int i=0; i<instruments.size(); i++) {
      if(!bHolding&&_y>bin.y&&_y<bin.y+bin.height&&instruments[i]->clickDown(_x,_y)){
        if(activeInst&&activeInst!=instruments[i]){
          instruments[i]->clickUp();
          activeInst->loseFocus();
        }
        else {
          ret=1;
          bHolding=true;
          lastInst=i;
        }

      }
    }
  }
  if(activeInst&&!ret) {
    if(!ret&&(activeInst->clickDown(_x, _y))){
      ret=1;
    }
  }
  
  if(!ret&&_y>bin.y&&_y<bin.y+bin.height&&bar.available()) lastY=_y,bar.setPressed(true);
  
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
  scrollUp.setAvailable((bar.getScrollPosition()>cell.y/4)&&bar.available());
  scrollDown.setAvailable((bar.getScrollPosition()<(bar.getFullSize()-bin.height)-cell.y/4)&&bar.available());
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
  
	//if(bar.mouseMotion(_x,_y)){
//		for (unsigned int i=0; i<instruments.size(); i++) {
//			instruments[i]->update(-bar.getScrollPosition(),OF_VERT);
//		}
//	}
  if(bar.available()&&bar.pressed()){
    bar.setScrollPosition(bar.getScrollPosition()+(lastY-_y));
    for (unsigned int i=0; i<instruments.size(); i++) {
			instruments[i]->update(-bar.getScrollPosition(),OF_VERT);
		}
    lastY=_y;
  }
  else bar.setPressed(false);
}

void bandBar::setActive(inst * currentInst)
{
  if(activeInst&&currentInst) activeInst->loseFocus();
  activeInst=currentInst;
}

int bandBar::farthestPoint()
{
	int ret=0;
	for (unsigned int i=0; i<instruments.size(); i++) {
    ret=max(ret,instruments[i]->farthestPoint());
	}
	return ret;
}

void bandBar::clear()
{
  stopAll();
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
  vector<unsigned char> msg;
  for (unsigned int i=0; i<4; i++) {
    msg.clear();
    msg.push_back( MIDI_CONTROL_CHANGE + i );
    msg.push_back( MIDI_CTL_ALL_NOTES_OFF );
    msg.push_back( 0x0 );
    ofGetAppPtr()->midiToSend(msg);
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
