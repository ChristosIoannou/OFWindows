#pragma once

#include "ofMain.h"
#include "ofxCv.h"

class Axes
{
public:

	Axes()
	{
        regenerateAxes(6);
    }

    ofPolyline xaxis;
    ofPolyline yaxis;

private:
    void regenerateAxes(int proportion)
    {
        std::vector<ofPoint> axes = generateAxesEnds(proportion);
        xaxis = equallySpacedPoints(axes[0], axes[1], 20);
        yaxis = equallySpacedPoints(axes[2], axes[3], 20);
    }

    std::vector<ofPoint> generateAxesEnds(int proportion)
    {
        // Generate four random points in the outer 25% of the screen
        int lw = ofGetWidth() / proportion;
        int lh = ofGetHeight() / proportion;
        int hw = ofGetWidth() / 2;
        int hh = ofGetWidth() / 2;


        ofPoint p0(ofRandom(-hw, lw - hw), ofRandom(lh - hh, hh));
        ofPoint p2(ofRandom(lw - hw, hw), ofRandom(hh - lh, hh));
        ofPoint p1(ofRandom(hw - lw, hw), ofRandom(-hh, hh - lh));
        ofPoint p3(ofRandom(-hw, hw - lh), ofRandom(-hh, lh - hh));

        // Check if the lines formed by the pairs cross over each other
        bool isCrossing = checkIfLinesCross(p0, p1, p2, p3);

        // If not, keep generating new points until we find ones that do
        while (!isCrossing) {
            p0 = ofPoint(ofRandomWidth(), ofRandomHeight());
            p1 = ofPoint(ofRandomWidth(), ofRandomHeight());
            p2 = ofPoint(ofRandomWidth(), ofRandomHeight());
            p3 = ofPoint(ofRandomWidth(), ofRandomHeight());
            isCrossing = checkIfLinesCross(p0, p1, p2, p3);
        }

        std::vector<ofPoint> axes{ p0, p1, p2, p3 };

        return axes;
    }

    bool checkIfLinesCross(ofPoint p1, ofPoint p2, ofPoint q1, ofPoint q2) {
        float denominator = ((q2.y - q1.y) * (p2.x - p1.x)) - ((q2.x - q1.x) * (p2.y - p1.y));
        float ua = (((q2.x - q1.x) * (p1.y - q1.y)) - ((q2.y - q1.y) * (p1.x - q1.x))) / denominator;
        float ub = (((p2.x - p1.x) * (p1.y - q1.y)) - ((p2.y - p1.y) * (p1.x - q1.x))) / denominator;

        if (ua >= 0 && ua <= 1 && ub >= 0 && ub <= 1) {
            return true;
        }
        else {
            return false;
        }
    }

	ofPolyline equallySpacedPoints(ofPoint start, ofPoint end, int numPoints) {
		ofPolyline line;
		float stepSize = 1.0f / (numPoints + 1);

		for (int i = 1; i <= numPoints; i++) {
			float t = i * stepSize;
			ofPoint p = start * (1 - t) + end * t;
			line.addVertex(p);
		}

		return line;
	}
};

class ParabolicCurve {

public:
	ParabolicCurve() {}

	void setup();
	void update();
	void draw();

    Axes axes;

};