#include "ofxKinect.h"

class KinectPointCloud {

public:
	void setKinectMesh(ofMesh mesh);
	void setPlayMesh(ofMesh mesh);

private:
	ofVboMesh kinectMesh;
	ofVboMesh playMesh;

};