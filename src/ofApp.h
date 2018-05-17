#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxLivingRoom.h"

class Glow : public ofxCv::RectFollower {
protected:
	ofColor color;
	float startedDying;
	ofPolyline all;
    //const ofxLivingRoom& room;
    bool send;
    
public:
    ofVec2f cur, smooth;
	Glow()
		:startedDying(0) {
            send = false;
	}
	void setup(const cv::Rect& track);
	void update(const cv::Rect& track);
	void kill();
	void draw();
    void setRoom(const ofxLivingRoom& _room);
    void setSend(bool _send);
};

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	
    ofVideoGrabber cam;
    
    ofxCvColorImage color;
    
    ofxCvGrayscaleImage hue, sat, bri;

	ofxCv::ContourFinder contourFinder;
	ofxCv::RectTrackerFollower<Glow> tracker;
    
    ofxLivingRoom room;
};
