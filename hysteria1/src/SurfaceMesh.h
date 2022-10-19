#pragma once

#include "ofMain.h"

class SurfaceMesh {

public:
	SurfaceMesh(ofEasyCam& cam_)
		: cam(cam_) {}

	void setup();
	void update(float totals, float mids, float highs);
	void draw();

	ofMesh mesh;
	ofParameter<bool> usePerlin;
	ofParameter<float> amount;
	int size = 96;

	ofEasyCam& cam;
};