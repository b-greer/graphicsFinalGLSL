#include "openFunctions.h"
#include "iostream"

void openFunctions::setup(ofVideoGrabber &webcam) {
	//rectmode center cos it's easier for me to figure out
	ofSetRectMode(OF_RECTMODE_CENTER);
	videoFeed = webcam;
	
	//i saved these to save approx 2seconds of typing later  (lazy)
	width = ofGetWidth();
	height = ofGetHeight();

	//allocate pixel space to this texture (enough for RGB (don't really care about A in this context))
	third.allocate(videoFeed.getWidth(), videoFeed.getHeight(), OF_IMAGE_COLOR);

}

void openFunctions::draw() {
	
ofPushMatrix();
	//shift the canvas to be centered and draw the original video feed
		ofTranslate(width / 2, height / 2);
		videoFeed.draw( 0, 0 , - videoFeed.getWidth(), videoFeed.getHeight());

ofPopMatrix();

ofPushMatrix(); 
	//scale down to make the smaller tiles, translate and draw each of the effects 
		ofScale(0.45);

		ofTranslate(width * 0.525,0);
	//theyre functions which are fed the video feed to mess around with 
		drawPopArt(videoFeed);

		ofTranslate(0,  height * 0.7);
		drawTriangles(videoFeed);

		ofTranslate(width * -0.25, height * 1.1 );
		drawRight(videoFeed, image);
	//and pop back 
ofPopMatrix();

}

void openFunctions::drawPopArt(ofVideoGrabber &webcam) {
	ofSetColor(ofColor::white);

	//this loop basically cycles for every 16px square 
	for (int i = 0; i < webcam.getWidth(); i += 16) {
		for (int j = 0; j < webcam.getHeight(); j += 16) {
			//takes the first pixel of the square and gets the colour
			ofColor color = webcam.getPixels().getColor(i, j);
			//set that as our drawing color
			ofSetColor(color);
			
			//isolate the green of the rgb and map that to pixels
			//so greener pixels expand
			float green = color.g;
			float radius = ofMap(green, 0, 255, 6, 16);
			//draw a box in each 16px block with a mapped radius/size
			ofDrawRectangle(-i, j, radius, radius);
		}
	}

//for some reason if i don't set the colour back to white everything else gets thrown off 
	ofSetColor(ofColor::white);
}

void openFunctions::drawTriangles(ofVideoGrabber &webcam) {
	ofSetColor(ofColor::white);

//same concept except 15 px blocks
	for (int i = 0; i < webcam.getWidth(); i += 15) {
		for (int j = 0; j < webcam.getHeight(); j += 15) {

			//receive px color
			ofColor color = webcam.getPixels().getColor(i, j);
			//invert it
			color = color.getInverted();
			//set drwaing colour to the inverted
			ofSetColor(color);

			//map radius to brightness 
			float brightness = color.getBrightness();
			float radius = ofMap(brightness * -1, -255, 0, 20, 25);
			//allow x and y values because it's easier for me to read for triangles 
			float x = -i;
			float y = j;

			if (i % 2 == 0) {
				//if we are on an even i (aka x) block, draw a triangle one way 
				ofDrawTriangle(x + radius , y - radius,
							   x + radius, y + radius, 
							   x - radius, y + radius);
			}
			else {
				//else draw a triangle a slightly different way
				ofDrawTriangle(x - radius, y - radius, 
							   x - radius, y + radius, 
							   x + radius, y - radius);
			}
		}
	}

//again we have to return the colour to white 
	ofSetColor(ofColor::white);
}

//i didn't have a good name for this 
void openFunctions::drawRight(ofVideoGrabber &webcam, ofImage &photo) {

	ofSetColor(ofColor::white);

	//create a pixels storage unit basically for the videofeed
	ofPixels pixels = videoFeed.getPixels();

	//10px blocks
	for (int i = 0; i < webcam.getWidth(); i += 10) {
		for (int j = 0; j < webcam.getHeight(); j += 10) {

			//get the colour of the first pix
			ofColor color = videoFeed.getPixels().getColor(i, j);
			ofSetColor(color);

			//map redness to the mouse? 
			float red = color.r * ofMap(ofGetMouseX(), 0, width, 0, 225, false);
			color.r = red;
			color.setBrightness(color.getBrightness() * 2);
			//pick out the first pixel 
			int index = pixels.getPixelIndex(i, j);

			//set that and the next 20 px to the colour we picked 
			//makes these cool scan lines that respond to mouseX input
			for (int n = 0; n < 20; ++n)
			{
				pixels.setColor(index + n, color);
			}
		}
	}

	//load the pixel data into the texture
	third.loadData(pixels);
	//and draw it (flipped)
	third.draw(0, 0, - third.getWidth(), third.getHeight());

	ofSetColor(ofColor::white);
}

void openFunctions::update(ofVideoGrabber &webcam) {
	videoFeed = webcam;
	ofSetColor(ofColor::white);
}