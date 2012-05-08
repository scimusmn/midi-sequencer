#include "midiConfig.h"

static configuration config;

configuration & cfg(){
	return config;
};

configuration::configuration(){
	ofEnableDataPath();
}

configuration::~configuration(){
	//midiIn.clear();
	//midiOut.clear();
}

void configuration::readGeneralConfig(string cfgFile){
	ifstream config(ofToDataPath(cfgFile).c_str());
	while (config.peek()!=EOF) {
		string nextLine;
		getline(config, nextLine);
		vector<string> token=ofSplitString(nextLine, "=\r\n");
		if(token.size()){
			if(token[0]=="VERBOSE"){
				verbose=ofToInt(token[1]);		//print out controlled interactions
			}
			else if(token[0]=="MIDI_IN"){
				midiInName=token[1];			//set the midiIn connection name
			}
			else if(token[0]=="MIDI_OUT"){
				midiOutName=token[1];			//set the midiOut connection name
			}
			else if(token[0]=="FULLSCREEN"){
				ofSetFullscreen(true);			//set fullscreen mode 
			}
			else if(token[0]=="INSTRUMENT_FILE"){
				instFile=token[1];				//tells the program where to find the xml file with the physical instruments
			}
			else if(token[0]=="SONG"){
				songFiles.push_back(token[1]);	//adds a song to the example songs
			}
			else if(token[0]=="TITLE"){
				progTitle=token[1];				// set the main title for the exhibits
			}
			else if(token[0]=="SYNTH_INST_FILE"){
				loadSynthInstruments(token[1]);		//tells the program where to find the file with the syth names
			}
			else if(token[0]=="NUM_MEASURES"){
				numMeasures=ofToFloat(token[1]);	// the number of measures on screen (sequencer)
			}
			else if(token[0]=="PIXELS_PER_MEASURE"){
				pixelsPerMeasure=ofToFloat(token[1]);	//the size of each measure, if we have horizontal scrolling enabled (sequencer)
			}
			else if(token[0]=="ENABLE_SIDE_SCROLLING"){
				scroll=ofToInt(token[1]);				//enable scrolling on the shortest tempo, on the sequencer
			}
			else if(token[0]=="TIMEOUT"){
				timeout=ofToInt(token[1]);				//the amount of time before the sequencer pauses playing, if no activity present
			}
			else if(token[0]=="NOTE_VELOCITY"){
				noteVelocity=ofToInt(token[1]);
			}
		}
	}
	config.close();
}

static vector<synthInst> instNames;

vector<synthInst> & getSynthInstruments()
{
	return instNames;
}

void loadSynthInstruments(string filename)
{
	instNames.clear();
  ifstream file(ofToDataPath(filename).c_str());
	while (file.peek()!=EOF) {
		string nextLine;
		getline(file, nextLine);
		vector<string> token=ofSplitString(nextLine, "=");
		if(token.size()>1){
			instNames.push_back(synthInst(ofToInt(token[0]),token[1]));
		}
		
	}
	file.close();
}