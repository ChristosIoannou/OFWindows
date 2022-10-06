#include "ofxKinect.h"

enum explodeStatus
{
	OFF = 0,
	EXPLODING = 1,
	CONTRACTING = 2

};

class KinectPointCloud {

public:

	//KinectPointCloud();
	void setupKinectPointCloud();
	void updateKinectPointCloud();
	void drawKinectPointCloud(ofEasyCam& cam);
	void getNewFrame(ofxKinect& kinect_);

	void setKinectMesh(ofMesh mesh);
	void setPlayMesh(ofMesh mesh);
	void transferKinectMeshToPlayMesh();
	void explode();
	void startExplode();
	void remerge();

	// Listeners
	void remergeListener(bool& b_pcRemerge);
	void explodeListener(bool& b_pcExpand);
	void rotateListener(bool& b_pcRotate);

	ofMesh kinectMesh;
	ofMesh playMesh;
	std::vector<ofVec3f> explodeDirections;
	ofVec3f centroid;
	bool usePlayMesh = false;
	bool b_explode = false;
	bool b_remerge = false;
	bool b_rotate = false;
	bool freezeKinectMesh = false;
	bool sparkle = false;
	float startOffsetAngle = 0.0f;
};