#include "customGLSL.h"

//--------------------------------------------------------------

void customGLSL::setup() {

	ofBackground(ofColor::white);
	ofEnableDepthTest();

	//load in shader
	shader.load("customGLSL");

	//set light qualities
	myPointLight.setDiffuseColor(ofColor(138, 98, 46));
	myPointLight.setSpecularColor(ofColor(255, 255, 255));
	myPointLight.setPosition(vec4(200.0, 200.0, 200.0, 0.5));

	//set camera qualities
	camera.setPosition(vec3(0.0, 10.0, 17.0));
	camera.setTarget(vec3(0.0, 4.0, 0.0));
	camera.setFov(64.0);
	camera.setAutoDistance(false);

	//draw rectangles from the center
	ofSetRectMode(OF_RECTMODE_CENTER);

	//tex coords 0-1
	ofDisableArbTex();
	ofEnableNormalizedTexCoords();

	//supershape values
	//these are currently constant to make a cube
	//but can be animated (especially using the mOff)
	total = 200;
	t = 0;
	index = -1;
	r = 5.0;
	mOff = 0;
	m = 4;
	n1 = 100;
	n2 = 100;
	n3 = 100;

	//set the material properties
	matAmbient = ofFloatColor(0.0, 0.0, 0.0);
	matDiffuse = ofFloatColor(0.2, 0.5, 0.7);
	matSpecular = ofFloatColor(3.5, 4.0, 5.0);
	matShininess = 500.0;
	matReflectivity = 1.0;
	matTransparency = 1.0;
	matRefractiveIndex = 1.333;

	//set light properties to be fed into the shader
	lightPosition = vec4(100.0, 100.0, 100.0, 1.0);
	lightColor = ofFloatColor(0.0, 1.0, 0.8);

	//mesh display mode & initialise the box
	myMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	boxInit(myMesh, total);

}
//--------------------------------------------------------------
void customGLSL::update(ofTexture &curTexture) {
	
	//update the shader per frame 
	shader.setUniformTexture("environmentMap", curTexture, 0);
}

//--------------------------------------------------------------

void customGLSL::draw(ofTexture &curTexture) {

	//camera view starts
	//anything to be seen goes in here
	camera.begin();
	//lighting on
	ofEnableLighting();

	//shader starts
	//anything drawn in here will have this shader
	shader.begin();

	//mousePos for future animation
	shader.setUniform2f("mousePos", vec2(ofGetMouseX(), ofGetMouseY()));
	//window size to ratio the texture
	shader.setUniform2f("size", ofGetWindowSize());

	//feed in material properties (definted in setup)
	shader.setUniform4f("matAmbient", matAmbient);
	shader.setUniform4f("matDiffuse", matDiffuse);
	shader.setUniform4f("matSpecular", matSpecular);
	shader.setUniform4f("lightPosition", lightPosition);
	shader.setUniform4f("lightColor", lightColor);
	shader.setUniform1f("matReflectivity", matReflectivity);

	//set the texture to the fed in frames
	shader.setUniformTexture("environmentMap", curTexture, 1);

	//get the time and feed it in
	float timeIn = ofGetElapsedTimef();
	shader.setUniform1f("time", timeIn);

	//push/pop to save state
	ofPushMatrix();
	//push the box up by (so its base rests on y=0 plane)
	ofTranslate(0, r, 0);

	//draw box 
	myMesh.draw();
	ofPopMatrix();

	shader.end();

	//draw the reference grid (stepsize 2, 10 steps, unlabelled x plane)
	ofDrawGrid(2.0, 10, false, false, true, false);


	camera.end();
}

//--------------------------------------------------------------

void customGLSL::boxInit(ofMesh& myMesh, int total)
{
	//a box mesh generated based on daniel shiffmans code
	/* Shiffman, Daniel. “Coding Challenge #26: 3D Supershapes.”
	YouTube, 30 June 2016*/

	for (int i = 0; i < total + 1; ++i)
	{
		//latitude values mapped based on i (x)
		lat = ofMap(i, 0, total, -HALF_PI, HALF_PI);

		//supershape r2
		r2 = supershape(lat, m, n1, n2, n3);


		for (int j = 0; j < total + 1; ++j)
		{
			++t;
			//longitude values mapped based on j (y)

			lon = ofMap(j, 0, total, -PI, PI);
			r1 = supershape(lon, m, n1, n2, n3);

			//apply spherical geomtery ( cartesian co ords <- sphere)
			x = r * r1 * cos(lon) * r2 * cos(lat);
			y = r * r1 * sin(lon) * r2 * cos(lat);
			z = r * r2 * sin(lat);

			//add each vertex to the array of vertices 
			globe[i][j] = vec3(x, y, z);

		}
	}


	for (int i = 0; i < total + 1; ++i)
	{
		for (int j = 0; j < total ; ++j)
		{
			++index;
			//cycle through all of the array
			vec3 n = globe[i][j];
			//add each to the mesh
			myMesh.addVertex(n);
			//and tex coords
			myMesh.addTexCoord(vec2(i / total, j / total));
			n = normalize(cross(globe[i][j + 1] - globe[i][j], globe[i + 1][j] - globe[i][j]));
			//and normals
			myMesh.addNormal(n);
			//and indices
			myMesh.addIndex(index);

			//again for n+1 
			++index;
			vec3 n1 = globe[i + 1][j];
			myMesh.addVertex(n1);
			myMesh.addTexCoord(vec2((i + 1) / total, j / total));
			n1 = normalize(-cross(globe[i][j + 1] - globe[i + 1][j + 1], globe[i + 1][j] - globe[i + 1][j + 1]));
			myMesh.addNormal(n1);
			myMesh.addIndex(index);

		}
	}
}

//--------------------------------------------------------------

float customGLSL::supershape(float theta, float m, float n1, float n2, float n3) {
	//a supershape transformation applied to the mesh to make it into a cube 
	//this was probably super unnecessary but I want to play with animation etc later 

	//again from daniel shiffman but based on paul bourke
	/* Bourke, Paul. “Supershapes / SuperFormula.” 
		Paulbourke.Net, Mar. 2002, paulbourke.net/geometry/supershape/. */

	//this code basically applies bourke's formula piece by piece 
	a = 1;
	b = 1;

	float t1 = abs((1 / a) * cos(m * theta / 4));
	t1 = pow(t1, n2);

	float t2 = abs((1 / b * sin(m * theta / 4)));
	t2 = pow(t2, n3);

	float t3 = t1 + t2;
	t3 = pow(t3, -1 / n1);

	float value = t3;
	return value;
}

//--------------------------------------------------------------

void customGLSL::mouseMoved(int x, int y) {

}
//--------------------------------------------------------------

void customGLSL::mouseDragged(int x, int y, int button) {

}
//--------------------------------------------------------------

void customGLSL::mousePressed(int x, int y, int button) {

}
//--------------------------------------------------------------

void customGLSL::mouseReleased(int x, int y, int button) {

}