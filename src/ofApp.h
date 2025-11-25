#pragma once

#include "ofMain.h"
#include <vector>

class Disk {
public:
    void setup(float mass, float radius, ofColor color);
    void update(const ofVec2f& force, float dt);
    void draw();
    void checkBoundaryCollision(float width, float height);
    void checkDiskCollision(Disk& other);
    
    ofVec2f position;
    ofVec2f velocity;
    ofVec2f acceleration;
    float mass;
    float radius;
    ofColor color;
    bool isActive;
};

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    
private:
    std::vector<Disk> disks;
    bool collisionsEnabled;
    ofVec2f mousePos;
    
    void checkAllDiskCollisions();
};