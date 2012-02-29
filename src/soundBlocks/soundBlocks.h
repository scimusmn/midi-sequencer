/*
 *  soundBlocks.h
 *  Miditron
 *
 *  Created by Exhibits on 9/13/11.
 *  Copyright 2011 Science Museum of Minnesota. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofExtended.h"

#include "../../../dallasEng/dallasEng.h"

#include "ofxMidiIn.h"
#include "ofxMidiOut.h"

/*****************************************************************
 * class soundBlock : public ofInterObj
 *
 *  Description:: class which carries and send midi info for both the remapper and sequencer
 *    Serve as both the base block and dragged blocks in derived class
 *
 *  Vars:
 *
 *   protected: bool bPercussive::_ tells how long to hold the note, either indefinitely or fraction of second
 *   public: string title:: _ _ _ _ name of the current block
 *   ofColor color::_ _ _ _ _ _ _ _ color of the block
 *   ofFont header::_ _ _ _ _ _ _ _ font used for writing onscreen
 */


class soundBlock : public ofInterObj{
protected:
  bool bPercussive;
public:
	string title;
	ofColor color;
	ofFont header;
	soundBlock():ofInterObj(){}
	soundBlock(string objName);
	~soundBlock();
	soundBlock(const soundBlock & t);
	void operator=(const soundBlock & t);
	void setup(string objName);
	void update(double x_dis, double y_dis);
	void draw(int _x, int _y);
	void draw(int _y);
	void draw();
	bool clickDown(int _x, int _y);
	bool clickUp();
	void mouseMotion(int _x, int _y);
  void changeTitle(string ttl);
	friend class inst;
};

/*****************************************************************
 * class rhythmBlock : public soundBlock 
 *
 *  Description:: block class which serves as the base block class
 *
 *  Vars:
 *
 *   bool bPlaying::_ _ _ _ _ _ _ _ indicates if the block is currently playing
 *   vector<unsigned char> message:: 
 *   public: unsigned char note:: _ 
 *   unsigned char channel::_ _ _ _  
 *   friend class instrument::_ _ _ 
 */


class rhythmBlock : public soundBlock {
protected:
	ofBaseApp * baseApp;
	bool bPlaying;
	vector<unsigned char> message;
public:
  unsigned char note;
	unsigned char channel;
	rhythmBlock():soundBlock(){}
	rhythmBlock(const rhythmBlock & t);
	void setup(string title, unsigned char nt, unsigned char chan);
	void setMidi(unsigned char nt, unsigned char chan, double delay=1);
	void setDelay(double dly);
	void sendMidiMessage(vector<unsigned char> newMessage);
	void play(unsigned char vel);
	void play();
	void stop();
	void update();
	bool clickDown(int _x, int _y);
	bool clickUp();
	bool isPlaying(){ return bPlaying;}
  
	friend class instrument;
};

/*****************************************************************
 * class dragBlock : public soundBlock 
 *
 *  Description:: blocks which the user drags onscreen
 *
 *
 *  Vars:
 *
 *   protected: ofButton leftAdj::_ 
 *   ofButton rightAdj::_ _ _ _ _ _ 
 *   bool bResizing:: _ _ _ _ _ _ _ 
 *   bool bLResize::_ _ _ _ _ _ _ _ 
 *   bool bActive:: _ _ _ _ _ _ _ _ 
 *   bool bJustActive:: _ _ _ _ _ _ 
 *   double aPos::_ _ _ _ _ _ _ _ _ 
 *   bool bNewBlock:: _ _ _ _ _ _ _ 
 *   public: unsigned char note:: _ 
 *   return bNewBlock:: _ _ _ _ _ _ 
 *   bNewBlock=false::_ _ _ _ _ _ _ 
 *   friend class inst::_ _ _ _ _ _ 
 */


class dragBlock : public soundBlock {
protected:
	ofButton leftAdj;
  ofButton rightAdj;
	bool bResizing;
  bool bLResize;
	bool bActive;
	bool bJustActive;
	double aPos;
  bool bNewBlock;
public:
  unsigned char note;
  dragBlock(int x, int w, rhythmBlock & t);
	dragBlock(rhythmBlock & t);
  void setup(rhythmBlock & t);
	void draw(int _y);
	void draw();
	void mouseMotion(int _x, int _y);
	bool clickDown(int _x, int _y);
	bool clickUp();
	bool active(double xPos);
	bool justChanged(double xPos);
  bool isNew(){ return bNewBlock; }
  void makeOld() { bNewBlock=false; }
	friend class inst;
};