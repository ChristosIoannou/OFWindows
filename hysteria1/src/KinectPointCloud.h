#include "ofxKinect.h"

enum explodeStatus
{
	OFF = 0,
	EXPLODING = 1,
	CONTRACTING = 2

};

class KinectPointCloud {

public:

	KinectPointCloud(ofxKinect& kinect_)
		: kinect(kinect_) {}

	//KinectPointCloud();
	void setup();
	void update();
	void draw(ofEasyCam& cam);
	void getNewFrame();

	void getFullFrame(int step, int shift);
	void getReducedFrame();
	void drawTrapped();

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

	ofxKinect& kinect;
	ofMesh kinectMesh;
	ofMesh playMesh;
	std::vector<ofVec3f> explodeDirections;
	ofVec3f centroid;
	bool usePlayMesh = false;
	bool freezeKinectMesh = false;
	float startOffsetAngle = 0.0f;

	ofParameter<bool> b_remerge;
	ofParameter<bool> b_explode;
	ofParameter<bool> b_rotate;
	ofParameter<bool> b_shimmer;
	ofParameter<bool> b_trapped;
	ofParameter<int> lightPosX;
	ofParameter<int> lightPosY;
	ofParameter<int> lightPosZ;
	ofParameter<int> lightOriX;
	ofParameter<int> lightOriY;
	ofParameter<int> lightOriZ;

	ofMaterial material;
	ofLight light;
};