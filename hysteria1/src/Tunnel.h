#pragma once

#include "ofMain.h"

class Tunnel {

public:
	void setup();
	void update();
	void draw();
	void drawFbo();
	void resize();

	ofParameter<float> timeScale;
	ofParameter<float> clearAlpha;
	ofFbo fbo;
	ofColor tunnelColor;
};