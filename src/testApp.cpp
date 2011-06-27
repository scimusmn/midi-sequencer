#include "testApp.h"

#define MSG_START "MSGSTRT"
#define MSG_END   "MSGEND"


//--------------------------------------------------------------
void testApp::setup(){
	
	//Network Setup
	
	//we run at 60 fps!
	ofSetVerticalSync(true);
	//setup the server to listen on 11999
	TCP.setup(11999);
	xml.loadFile("instruments.xml");
	//MIDI setup
	report.loadFont("fonts/DinC.ttf");
	band.setup(xml);
	keyboard.openPort("keyboard");
	rolandSynth.openPort("decoder");
	background.loadImage("images/background.jpg");
	kb.setup(800, 4);
	conductor.setup(10, 1, 100, ofGetWidth()-band.w); //26 seconds at 100 pixels per second
	conductor.registerPlayback(&band);
  conductor.setTimeSignature(4);
	test.setup(200, 6);
}

//--------------------------------------------------------------
void testApp::update(){
	//band.update(-conductor.getBarPosition(),OF_HOR);
//	if(conductor.isPlaying())
//		band.checkActives(conductor.cursor()+band.w);
	band.setHeight(ofGetHeight()-75, 30,120);
	band.update();
	conductor.update();
	
	
	//Network
	for(int i = 0; i < TCP.getNumClients(); i++){
		TCP.send(i, "hello client - you are connected on port - "+ofToString(TCP.getClientPort(i)) );
	}
	
	//for each connected client lets get the data being sent and lets print it to the screen
	for(int i = 0; i < TCP.getNumClients(); i++){
		
		//get the ip and port of the client
		string port = ofToString( TCP.getClientPort(i) );
		string ip   = TCP.getClientIP(i);
		string info = "client "+ofToString(i)+" -connected from "+ip+" on port: "+port;
		
		
		//if we don't have a string allocated yet
		//lets create one
		if(i >= storeText.size() ){
			storeText.push_back( string() );
		}
		
		string str = TCP.receive(i);
		
		if(str.length()){
			//storeText[i] = str;
			cout << str << " new string"<< endl;
			vector<unsigned char> msg;
			vector<string> token=ofSplitString(str, ".");
			for (unsigned int i=0; i<token.size(); i++) {
				if(!token[i].compare(MSG_START)){
					msg.clear();
					while(token[++i].compare(MSG_END)){
						unsigned char tmp=ofToInt(token[i]);
						cout << int(tmp)<< ".";
						msg.push_back(tmp);
					}
					cout << " Message end size->"<< msg.size() << endl;
					if(msg.size()) midiToSend(msg);
				}
			}
		}
		
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	ofSetColor(255, 255, 255);
	
#if F_YEAH_WOOD
	background.draw(0, 75,ofGetWidth(),ofGetHeight());	
#else
	ofBackground(0x33, 0x33, 0x33);	
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
	ofShade(band.x+band.w, band.y+conductor.h, 10, band.getBottomPos()-conductor.h, OF_RIGHT, .4);
	ofShade(band.x+band.w, band.getBottomPos(), 5, ofGetHeight()-band.getBottomPos(), OF_RIGHT, .4);
	ofShade(band.x+band.w, band.y, 5, conductor.h, OF_RIGHT, .4);
	
	//kb.draw(100, 500);
#if F_YEAH_WOOD
	ofSetColor(0x90734B);
#else
	ofSetColor(0x555555);
#endif
	ofRoundBox(-30, 0, ofGetWidth()+60, 75, 15, .2);
	ofSetColor(0xDDDDDD);
	report.setMode(OF_FONT_CENTER);
	report.setSize(40);
	report.drawString("Drag blocks and press play to make a rhythm!", ofGetWidth()/2, 75-20);
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
	}
	conductor.clickUp();
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

void testApp::midiToSend(vector< unsigned char > message){
  if(message.size()>=3) cout << ((message[2])?"on":"off") << endl;
	rolandSynth.sendMessage(message);
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