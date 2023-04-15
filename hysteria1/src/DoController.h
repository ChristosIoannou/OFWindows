#pragma once

#include "ofMain.h"

class DoController {

public:
	void setup();

	void audioSphereListener(bool& b_audioSphere);
	void particleRiverListener(bool& b_particleRiver);
	void flashingTextListener(bool& b_flashingText);
	void kinectContourListener(bool& b_kinectContour);
	void kinectPointCloudListener(bool& b_kinectPointCloud);
	void tunnelListener(bool& b_tunnel);
	void surfaceMeshListener(bool& b_surfaceMesh);
	void videoPlayerListener(bool& b_videoPlayer);
	void parabolicCurveListener(bool& b_parabolicCurve);

	ofParameter<bool> b_audioSphere;
	ofParameter<bool> b_particleRiver;
	ofParameter<bool> b_flashingText;
	ofParameter<bool> b_kinectContour;
	ofParameter<bool> b_kinect;
	ofParameter<bool> b_kinectPointCloud;
	ofParameter<bool> b_tunnel;
	ofParameter<bool> b_surfaceMesh;
	ofParameter<bool> b_videoPlayer;
	ofParameter<bool> b_parabolicCurve;

};