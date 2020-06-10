#pragma once

#include "ofMain.h"
#include "../customGLSL.h"
#include "../openFunctions.h"

using namespace glm;

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
//load  in an image later 
	ofImage image;

// webcam used
	ofVideoPlayer video;
	ofVideoGrabber webcam;

//easy cam to view scene
	ofEasyCam camera;

//creating an object for each class
	customGLSL glsl;
	openFunctions functions;
	
//switch
	bool glslOn;

//floats for timer 
	float timer, startTime, maxTime, halfTime, currBar, barWidth, barHeight;



};
