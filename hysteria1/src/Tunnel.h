#pragma once

#include "ofMain.h"

class Tunnel {

public:
	void setup();
	void update();
	void draw();
	void drawFbo();
	void resize();

	ofParameter<int> spacing;
	ofFbo fbo;
	ofColor tunnelColor;
};