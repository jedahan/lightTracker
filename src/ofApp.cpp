#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

const float dyingTime = 1;

void Glow::setup(const cv::Rect& track) {
	color.setHsb(ofRandom(0, 255), 255, 255);
	cur = toOf(track).getCenter();
	smooth = cur;
}

void Glow::setSend(bool _send) {
    send = _send;
}

void Glow::update(const cv::Rect& track) {
	cur = toOf(track).getCenter();
	smooth.interpolate(cur, .5);
	all.addVertex(smooth.x, smooth.y);
}

void Glow::kill() {
	float curTime = ofGetElapsedTimef();
	if(startedDying == 0) {
		startedDying = curTime;
	} else if(curTime - startedDying > dyingTime) {
		dead = true;
	}
}

void Glow::draw() {
	ofPushStyle();
	float size = 16;
	ofSetColor(255);
	if(startedDying) {
		ofSetColor(ofColor::red);
		size = ofMap(ofGetElapsedTimef() - startedDying, 0, dyingTime, size, 0, true);
	}
	ofNoFill();
	ofDrawCircle(cur, size);
	ofSetColor(color);
	all.draw();
	ofSetColor(255);
	ofDrawBitmapString(ofToString(label), cur);
	ofPopStyle();
}

void ofApp::setup() {
	ofSetVerticalSync(true);
	ofBackground(0);
    room.setup();
	
    cam.setup(640, 480);
    color.allocate(cam.getWidth(), cam.getHeight());
    hue.allocate(cam.getWidth(), cam.getHeight());
    sat.allocate(cam.getWidth(), cam.getHeight());
    bri.allocate(cam.getWidth(), cam.getHeight());
    
	contourFinder.setMinAreaRadius(20);
	contourFinder.setMaxAreaRadius(100);
	//contourFinder.setAutoThreshold(true);
    //contourFinder.setSimplify(true);
    contourFinder.setThreshold(220);
	
	tracker.setPersistence(30);
	tracker.setMaximumDistance(50);
}

void ofApp::update() {
	cam.update();
	if(cam.isFrameNew()) {
		blur(cam, 10);
        
        color.setFromPixels(cam.getPixels());
        color.convertRgbToHsv();
        color.convertToGrayscalePlanarImages(hue, sat, bri);

		contourFinder.findContours(bri);
        
		tracker.track(contourFinder.getBoundingRects());
        const vector<cv::Rect> & contours = contourFinder.getBoundingRects();
        const vector<unsigned int>& labels = tracker.getCurrentLabels();
        for(int i = 0; i < labels.size(); i++) {
            auto label = labels[i];
            if (tracker.getAge(label) < 30) continue;

            auto previous = tracker.getPrevious(label);
            auto current = tracker.getCurrent(label);
            
            ofPoint p = toOf(previous.tl() + previous.br()) * 0.5;
            ofPoint c = toOf(current.tl() + current.br()) * 0.5;
            if (c.distance(p) < 10) return;
            
            c /= ofPoint(cam.getWidth(), cam.getHeight());
            
            std::string fact = "glow " + ofToString(label) + " is at (" + ofToString(c.x) + ", " + ofToString(c.y) + ")";
            ofLogNotice() << fact << std::endl;
            room.assertFact(fact);
        }
	}
}

void ofApp::draw() {	
	ofSetColor(255);
	cam.draw(0, 0);
	bri.draw(0, 0);
    contourFinder.draw();
	vector<Glow>& followers = tracker.getFollowers();
	for(int i = 0; i < followers.size(); i++) {
		followers[i].draw();
	}
}
