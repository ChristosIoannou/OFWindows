#pragma once

#include "ofMain.h"
#include "ofxCv.h"

class VideoPlayer {

enum class Video 
{
	NONE,
	BIKE,
	PAPA_SPEAKS_JAPANESE
};

public:
	VideoPlayer() {}

	void setup();
	void update();
	void draw();

	ofVideoPlayer bikeVideo;
	ofVideoPlayer papaVideo;
	ofImage gray, edge, sobel;

	ofParameter<bool> papaSpeaksJapanese;
	ofParameter<bool> bikeVid;
	ofParameter<int> vidChoice;
	Video video;
};