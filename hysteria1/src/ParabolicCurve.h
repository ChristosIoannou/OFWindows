#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include <boost/optional.hpp>

class Axes
{
public:

	Axes()
	{
        regenerateAxes(6, 20);
    }

    ofPolyline xaxis;
    ofPolyline yaxis;
    ofPoint intersection;
    std::vector<ofPolyline> lines;

    void regenerateAxes(int proportion, int numPoints)
    {
        lines.clear();
        std::vector<ofPoint> axes = generateAxesEnds(proportion);
        xaxis = equallySpacedPoints(axes[0], axes[1], numPoints);
        yaxis = equallySpacedPoints(axes[2], axes[3], numPoints);
        intersection = axes[4];
        lines = fillLines(numPoints);
    }

    void draw()
    {
        xaxis.draw();
        yaxis.draw();

        for (const auto l : lines)
            l.draw();
    }

private:
    std::vector<ofPoint> generateAxesEnds(int proportion)
    {
        // Generate four random points in the outer 25% of the screen
        int lw = ofGetWidth() / proportion;
        int lh = ofGetHeight() / proportion;
        int hw = ofGetWidth() / 2;
        int hh = ofGetHeight() / 2;

        ofPoint p0, p1, p2, p3;

        boost::optional<ofPoint> intersection = boost::none;

        // If not, keep generating new points until we find ones that do
         do {

             p0 = ofPoint(ofRandom(-hw, lw - hw), ofRandom(lh - hh, hh));
             p2 = ofPoint(ofRandom(lw - hw, hw), ofRandom(hh - lh, hh));
             p1 = ofPoint(ofRandom(hw - lw, hw), ofRandom(-hh, hh - lh));
             p3 = ofPoint(ofRandom(-hw, hw - lh), ofRandom(-hh, lh - hh));

            intersection = checkIfLinesCross(p0, p1, p2, p3);
         } while (intersection == boost::none);

        std::vector<ofPoint> axes{ p0, p1, p2, p3, intersection.value() };

        return axes;
    }

    boost::optional<ofPoint> checkIfLinesCross(ofPoint p1, ofPoint p2, ofPoint q1, ofPoint q2) {
        float denominator = ((q2.y - q1.y) * (p2.x - p1.x)) - ((q2.x - q1.x) * (p2.y - p1.y));
        float ua = (((q2.x - q1.x) * (p1.y - q1.y)) - ((q2.y - q1.y) * (p1.x - q1.x))) / denominator;
        float ub = (((p2.x - p1.x) * (p1.y - q1.y)) - ((p2.y - p1.y) * (p1.x - q1.x))) / denominator;

        if (ua >= 0 && ua <= 1 && ub >= 0 && ub <= 1) {

            float x = p1.x + (ua * (p2.x - p1.x));
            float y = p1.y + (ua * (p2.y - p1.y));

            return ofPoint(x, y);
        }
        else {
            return boost::none;
        }
    }

	ofPolyline equallySpacedPoints(ofPoint start, ofPoint end, int numPoints) {
		ofPolyline line;
        float stepSize = 1.0f / (numPoints + 1);

		for (int i = 0; i <= numPoints; i++) {
			float t = i * stepSize;
			ofPoint p = start.getInterpolated(end, t);
			line.addVertex(p);
		}

		return line;
	}

    std::vector<ofPolyline> fillLines(int numPoints)
    {
        std::vector<ofPolyline> ls;
        for (int i = 0; i < xaxis.getVertices().size(); ++i)
        {
            ofPolyline pl;
            pl.addVertex(xaxis.getVertices()[i]);
            pl.addVertex(yaxis.getVertices()[numPoints - i]);
            ls.push_back(pl);
        }
        return ls;
    }
};

class ParabolicCurve {

public:
	ParabolicCurve() {}

	void setup();
	void update();
	void draw();
    void drawFbo();
    void resize();

    ofFbo fbo;
    ofColor tunnelColor;

    ofPoint getLineIntersection(Axes axes);
    
    ofParameter<bool> regenerate;
    ofParameter<int> spacing;

    Axes axes;

};