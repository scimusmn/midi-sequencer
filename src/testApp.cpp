#include "testApp.h"

#define MSG_START "MSGSTRT"
#define MSG_END   "MSGEND"

#include "ofNewShapes.h"

int roundH=300;
int roundR=50;

//--------------------------------------------------------------
void testApp::setup(){
	
	//Network Setup
	
	//we run at 60 fps!
	ofSetVerticalSync(true);
	
	xml.loadFile("instruments.xml");
	//MIDI setup
	report.loadFont("fonts/DinC.ttf");
	band.setup(xml);
	//keyboard.openPort("keyboard");
	rolandSynth.openPort("decoder");
	background.loadImage("images/background.jpg");
	kb.setup(800, 4);
	conductor.setup(10, 1, 50, ofGetWidth()-band.w); //10 seconds at 100 pixels per second
	conductor.registerPlayback(&band);
  conductor.setTimeSignature(4);
	test.setup(200, 6);
}

//--------------------------------------------------------------
void testApp::update(){
	//band.update(-conductor.getBarPosition(),OF_HOR);
//	if(conductor.isPlaying())
//		band.checkActives(conductor.cursor()+band.w);
	band.setHeight();
	band.update();
	conductor.update();
	
}

//--------------------------------------------------------------
void testApp::draw(){
  ofPushMatrix();
  //ofScale(ofGetWidth()/1920., ofGetHeight()/1200., 1);
	ofSetColor(255, 255, 255);
	
#if F_YEAH_WOOD
	background.draw(0, 75,ofGetWidth(),ofGetHeight());	
#else
	ofBackground(0x22, 0x22, 0x22);	
#endif
	
	/*ofSetColor(0x444400);
	for (int i=0; i*10<ofGetHeight(); i++) {
		ofRect(0, i*10, ofGetWidth(), 1);
	}
	for (int i=0; i*10<ofGetWidth(); i++) {
		ofRect(i*10, 0, 1, ofGetHeight());
	}*/
	
	//band.drawBackground();
	conductor.draw(band.x+band.w,75);
	band.draw(0,75);
	
	//shade to left of sidebar
	
	ofShade(band.x+band.w, band.y, 5, conductor.h, OF_RIGHT, .4);
	
	//kb.draw(100, 500);
#if F_YEAH_WOOD
	ofSetColor(0x90734B);
#else
	ofSetColor(0x555555);
#endif
  ofRaised(.2);
	ofRoundedRect(-30, 0, ofGetWidth()+60, 75, 15);
  ofFlat();
	ofSetColor(0xe5e00f);
	report.setMode(OF_FONT_CENTER);
	report.setSize(40);
	report.drawString("LAY A PHAT BEAT", ofGetWidth()/2, 75-20); //"Drag blocks and press play to make a rhythm"
  ofShade(0, 75, 10, ofGetWidth(), OF_DOWN, .5);
  
  ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key=='s') conductor.saveSong("testBand.xml");
	if(key==' '){
		if(conductor.isPlaying()) conductor.pause(),band.stopAll();
		else conductor.play();
	}
  if(key=='l'){
    conductor.loadSong("testBand.xml");
  }
	if(key=='p'){
		vector<unsigned char> msg;
		msg.push_back(MIDI_NOTE_ON+0);
		msg.push_back(0x3c);
		msg.push_back(0);
		rolandSynth.sendMessage(msg);
	}
		
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	conductor.mouseMotion(x,y);
  //conductor.drag(x, y);
	band.mouseMotion(x, y);
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	band.clickDown(x, y);
	conductor.clickDown(x,y);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	if(band.clickUp()){
		conductor.snapTo(band.lastBlock());
    if (band.lastBlock().isNew())
      band.lastBlock().makeOld();
	}
	conductor.clickUp();
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
  conductor.resize();
}

void testApp::midiToSend(vector< unsigned char > message){
	rolandSynth.sendMessage(message);
  cout << "Message Begin::";
  for (unsigned int i=0; i<message.size(); i++) {
    cout << int(message[i]) << "::";
  }
  cout << "Message end\n";
}

void testApp::midiReceived(double deltatime, std::vector< unsigned char > *message, int port){
	if(message->size()>=3){
		if (message->at(0)==MIDI_NOTE_ON&&port==keyboard.getPort()) {
			if(message->at(1)>MIDI_KEYBOARD_START&&message->at(1)<MIDI_KEYBOARD_END){
				kb[message->at(1)-MIDI_KEYBOARD_START].setPressed(message->at(2));
				vector<unsigned char> msg;
				msg=*message;
				msg[0]=MIDI_NOTE_ON+1;
				midiToSend(msg);
			}
		}
	}
}