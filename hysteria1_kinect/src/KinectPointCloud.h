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
	void drawKinectPointCloud();
	void getNewFrame(ofxKinect& kinect_);

	void setKinectMesh(ofMesh mesh);
	void setPlayMesh(ofMesh mesh);
	void transferKinectMeshToPlayMesh();
	void explode();
	void startExplode();
	void remergeExplode();

	// Listeners
	void explodeListener(bool& b_pcExpand);

	ofMesh kinectMesh;
	ofMesh playMesh;
	std::vector<ofVec3f> explodeDirections;
	bool usePlayMesh = false;
	bool b_explode = false;
	bool freezeKinectMesh = false;
};