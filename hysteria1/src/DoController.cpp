#include "DoController.h"

void DoController::setup() {
	b_audioSphere.addListener(this, &DoController::audioSphereListener);
	b_particleRiver.addListener(this, &DoController::particleRiverListener);
	b_flashingText.addListener(this, &DoController::flashingTextListener);
	b_kinectContour.addListener(this, &DoController::kinectContourListener);
	b_kinectPointCloud.addListener(this, &DoController::kinectPointCloudListener);
	b_tunnel.addListener(this, &DoController::tunnelListener);
	b_surfaceMesh.addListener(this, &DoController::surfaceMeshListener);
	b_videoPlayer.addListener(this, &DoController::videoPlayerListener);
	b_parabolicCurve.addListener(this, &DoController::parabolicCurveListener);
}

void DoController::audioSphereListener(bool& b_audioSphere) {
	ofRotateX(0.0);
	ofRotateY(0.0);
	ofRotateZ(0.0);
	if (b_audioSphere) {
		b_particleRiver = false;
		b_flashingText = false;
		b_kinectContour = false;
		b_kinect = false;
		b_kinectPointCloud = false;
		b_tunnel = false;
		b_surfaceMesh = false;
		b_videoPlayer = false;
		b_parabolicCurve = false;
	}
}

void DoController::particleRiverListener(bool& b_particleRiver) {
	if (b_particleRiver) {
		b_audioSphere = false;
		b_flashingText = false;
		b_kinectContour = false;
		b_kinect = false;
		b_kinectPointCloud = false;
		b_tunnel = false;
		b_surfaceMesh = false;
		b_videoPlayer = false;
	}
}

void DoController::flashingTextListener(bool& b_flashingText) {
	if (b_flashingText) {
		b_audioSphere = false;
		b_particleRiver = false;
		b_kinectContour = false;
		b_kinect = false;
		b_kinectPointCloud = false;
		b_tunnel = false;
		b_surfaceMesh = false;
		b_videoPlayer = false;
		b_parabolicCurve = false;
	}
}


void DoController::kinectContourListener(bool& b_kinectContour) {
	if (b_kinectContour) {
		b_audioSphere = false;
		b_particleRiver = false;
		b_flashingText = false;
		b_kinect = true;
		b_kinectPointCloud = false;
		b_tunnel = false;
		b_surfaceMesh = false;
		b_videoPlayer = false;
		b_parabolicCurve = false;
	}
}


void DoController::kinectPointCloudListener(bool& b_kinectPointCloud) {
	if (b_kinectPointCloud) {
		b_audioSphere = false;
		b_particleRiver = false;
		b_flashingText = false;
		b_kinectContour = false;
		b_kinect = true;
		b_tunnel = false;
		b_surfaceMesh = false;
		b_videoPlayer = false;
		b_parabolicCurve = false;
	}
}

void DoController::tunnelListener(bool& b_tunnel) {
	if (b_tunnel) {
		b_audioSphere = false;
		b_particleRiver = false;
		b_flashingText = false;
		b_kinectContour = false;
		b_kinect = false;
		b_kinectPointCloud = false;
		b_surfaceMesh = false;
		b_videoPlayer = false;
		b_parabolicCurve = false;
	}
}

void DoController::surfaceMeshListener(bool& b_surfaceMesh) {
	if (b_surfaceMesh) {
		b_audioSphere = false;
		b_particleRiver = false;
		b_flashingText = false;
		b_kinectContour = false;
		b_kinect = false;
		b_kinectPointCloud = false;
		b_tunnel = false;
		b_videoPlayer = false;
		b_parabolicCurve = false;
	}
}

void DoController::videoPlayerListener(bool& b_videoPlayer) {
	if (b_videoPlayer) {
		b_audioSphere = false;
		b_particleRiver = false;
		b_flashingText = false;
		b_kinectContour = false;
		b_kinect = false;
		b_kinectPointCloud = false;
		b_tunnel = false;
		b_surfaceMesh = false;
		b_parabolicCurve = false;
	}
}

void DoController::parabolicCurveListener(bool& b_parabolicCurve) {
	if (b_parabolicCurve) {
		b_audioSphere = false;
		b_particleRiver = false;
		b_flashingText = false;
		b_kinectContour = false;
		b_kinect = false;
		b_kinectPointCloud = false;
		b_tunnel = false;
		b_surfaceMesh = false;
		b_videoPlayer = false;
	}
}