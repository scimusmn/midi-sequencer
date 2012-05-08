#pragma once

/*
*
* Trying out a new coinfiguration reader, for globalized constants
*
*/

#include "ofMain.h"
#include "ofExtended.h"
#include "ofxMidiIn.h"
#include "ofxMidiOut.h"
#include "../../dallasEng/dallasEng.h"

struct configuration {
	string midiOutName,midiInName;
	string instFile;
	string progTitle;
	vector<string> songFiles;
	double timeout;
	double numMeasures;
	double pixelsPerMeasure;
	bool scroll;
	int noteVelocity;
	bool verbose;
	void readGeneralConfig(string cfgFile);
	configuration();
	~configuration();
	void setup(){
		readGeneralConfig("config.txt");
	}
};

configuration & cfg();

struct synthInst {
  int number;
  string name;
  synthInst(int i, string n):number(i),name(n){}
};

vector<synthInst> & getSynthInstruments();

void loadSynthInstruments(string filename);
