#pragma once
#include "ofMain.h"
using namespace glm;

class openFunctions : public ofBaseApp {

public:
	void setup(ofVideoGrabber &webcam);
	void update(ofVideoGrabber &webcam);
	void draw();
	
	void drawPopArt(ofVideoGrabber &vid);
	void drawTriangles(ofVideoGrabber &vid);
	void drawRight(ofVideoGrabber &webcam, ofImage &photo);

	//dimensions and vid dimensions
	float width, height;

	float radius;

	ofImage image;

	ofVideoGrabber videoFeed;

	//a texture to load pixel data to later
	ofTexture third;

	//easycam
	ofEasyCam camera;
};

