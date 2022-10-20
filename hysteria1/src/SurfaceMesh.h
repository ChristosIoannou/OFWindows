#pragma once

#include "ofMain.h"

enum ColorScheme {
	Z,
	RAD
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
	ofParameter<int> colorScheme;

	int size = 96;

	ofEasyCam& cam;
};