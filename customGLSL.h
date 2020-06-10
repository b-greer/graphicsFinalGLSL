#pragma once

#include "ofMain.h"

using namespace glm;

class customGLSL
{
public:
	void setup();
	void update(ofTexture &curTexture);
	void draw(ofTexture &curTexture);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void boxInit(ofMesh& myMesh, int total);
	float supershape(float theta, float m, float n1, float n2, float n3);

	//init all the objects i want 
	ofShader shader;
	ofImage image;
	ofImage environmentMap;
	ofTexture texture;
	ofMesh myMesh;
	ofLight myPointLight;
	ofEasyCam camera;

	//colors which will be set and fed into both the 
	//relevant objects and the shader
	ofFloatColor lightColor;
	ofFloatColor matAmbient;
	ofFloatColor matDiffuse;
	ofFloatColor matSpecular;

	vec4 lightPosition;

	//values for both the mat and its shader
	float matShininess;
	float matReflectivity;
	float matTransparency;
	float matRefractiveIndex;

	//these floats are all used in the supershape/mesh functions
	float Time, a, b, r, r1, r2, m, n1, n2, n3, lon, lat, x, y, z, boxHeight, mChange, mOff;
	int total, t, index;

	//a 2d array of vertices
	vec3 globe[401][401];
	//an array of indexes
	int indies[320800];

};