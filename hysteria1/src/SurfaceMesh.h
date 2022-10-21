#pragma once

#include "ofMain.h"

enum ColorScheme {
	Z,
	CIRCLE,
	WHITE
};

class SurfaceMesh {

public:
	SurfaceMesh(ofEasyCam& cam_)
		: cam(cam_) {}

	void setup();
	void update(float totals, float mids, float highs);
	void draw();

	ofFloatColor calculateColor(ofVec3f position);

	ofMesh mesh;
	ofParameter<bool> usePerlin;
	ofParameter<float> alpha;
	ofParameter<int> colorScheme;
	ofParameter<float> circleLower;
	ofParameter<float> circleUpper;
	ofParameter<bool> sinCircle;
	ofParameter<bool> rotateZ;
	ofParameter<bool> rotateX;
	ofParameter<bool> rotateY;
	ofParameter<bool> zoom;
	ofParameter<bool> rangeDependent;
	ofParameter<bool> wireframe;

	int size = 96;

	ofEasyCam& cam;
};