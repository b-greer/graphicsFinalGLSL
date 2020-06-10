#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	//functions.setup(webcam);

	//setup both
	glsl.setup();
	functions.setup(webcam);

	//load an image
	image.load("riot.jpg");

	//tex coords 0-1 not arbs
	ofDisableArbTex();
	ofEnableNormalizedTexCoords();

	//switch 
	glslOn = false;

	//setup webcam size/ratios
	webcam.setup(640, 480);

	//setup timer values
	timer = ofGetElapsedTimef();
	maxTime = 30.0;
	halfTime = maxTime / 2;

	//max timer bars 
	barHeight = 20.0;
	barWidth = 100.0;

}

//--------------------------------------------------------------
void ofApp::update() {

	//update the webcam feed
	webcam.update();
	
if (webcam.isFrameNew())
{ 
	if (glslOn)
	{
		//feed new feed into the glsl
		glsl.update(webcam.getTexture());
	}
	else
	{
		//call the update into the functions object
		functions.update(webcam);
	}
}
}
//--------------------------------------------------------------
void ofApp::draw() {

	//timer value ++ 
	timer = ofGetElapsedTimef();
	//map timer bar to value
	currBar = ofMap(timer, 0.0, maxTime, 0, barWidth, true);

	if (timer < halfTime)
	{
		//switches the of functions effects on when timer < half of the max time
		ofClear(ofColor::white);
		glslOn = false;
	}
	else if ( halfTime < timer < maxTime)
	{
		//switches on glsl shader when timer > half of max time
		ofClear(ofColor::white);
		glslOn = true;
	}
	
	if ( timer > maxTime)
	{
		//reset the scene when max time is topped
		glslOn = false;
		timer = 0;
		ofResetElapsedTimeCounter();

		ofClear(ofColor::white);
	}


	//set drawing color to black
	ofSetColor(ofColor::black);

	//labels
	if (glslOn)
	{
		ofDrawBitmapString(" glsl shader ", ofGetWidth() * 0.8, 15);
	}
	if (!glslOn)
	{
		ofDrawBitmapString(" ofFunctions effects ", ofGetWidth() * 0.8, 15);
	}

	//draw timer value
	ofDrawBitmapString("timer: " + ofToString(timer), ofGetWidth() * 0.8, 30);

	//mapped values at beginning make a growing timer bar
	ofSetRectMode(OF_RECTMODE_CORNER);
	ofDrawRectangle(ofGetWidth() * 0.8, 50, currBar, barHeight);

	ofSetRectMode(OF_RECTMODE_CENTER);
	//return to white
	ofSetColor(ofColor::white);


	//call draw functions of whichever class is switched on
	if(glslOn)
	{ 
	glsl.draw(webcam.getTexture());
	}
	else
	{
		functions.draw();
	}


}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if (key == ' ' && glslOn)
	{
		ofClear(ofColor::white);
		glslOn = false;
	}

	else if (key == ' ')
	{
		ofClear(ofColor::white);
		glslOn = true;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
