#include "ofApp.h"

using namespace std;

// Disk class implementation
void Disk::setup(float mass, float radius, ofColor color) {
    this->mass = mass;
    this->radius = radius;
    this->color = color;
    this->isActive = true;
    
    // Random initial position (within safe bounds)
    position.set(ofRandom(radius, ofGetWidth() - radius),
                 ofRandom(radius, ofGetHeight() - radius));
    
    // Random initial velocity
    velocity.set(ofRandom(-100, 100), ofRandom(-100, 100));
    
    acceleration.set(0, 0);
}

void Disk::update(const ofVec2f& force, float dt) {
    if (!isActive) return;
    
    // Euler
    acceleration = force / mass; // F = m * a  => a = F / m
    velocity += acceleration * dt; // v = v0 + a * dt
    position += velocity * dt; // p = p0 + v * dt
}

void Disk::draw() {
    if (!isActive) return;
    
    ofSetColor(color);
    ofDrawCircle(position, radius);
    ofSetColor(ofRandom(255), ofRandom(255), ofRandom(255), 100);
    ofDrawLine(position, position + velocity * 0.1f);
}

void Disk::checkBoundaryCollision(float width, float height) {
    if (!isActive) return;
    if (position.x - radius < 0) {
        position.x = radius;
        velocity.x = -velocity.x * 0.95f; // slight energy loss
    }
    else if (position.x + radius > width) {
        position.x = width - radius;
        velocity.x = -velocity.x * 0.95f;
    }
    if (position.y - radius < 0) {
        position.y = radius;
        velocity.y = -velocity.y * 0.95f;
    }
    else if (position.y + radius > height) {
        position.y = height - radius;
        velocity.y = -velocity.y * 0.95f;
    }
}

void Disk::checkDiskCollision(Disk& other) {
    if (!isActive || !other.isActive) return;
    
    // Dystans miedzy dyskami
    ofVec2f delta = position - other.position;
    float distance = delta.length();
    float minDistance = radius + other.radius;
    
    // warunek odbicia
    if (distance < minDistance && distance > 0) {
        ofVec2f collisionNormal = delta.getNormalized();
        ofVec2f relativeVelocity = velocity - other.velocity;
        
        // iloczyn skalarny predkosci wzglednej i normalnej kolizji
        float velocityAlongNormal = relativeVelocity.dot(collisionNormal);
        if (velocityAlongNormal > 0) return;
        
        //miekkosc 
        float restitution = 0.9f;
        
        //kierunek odbicia
        float impulseScalar = -(1.0f + restitution) * velocityAlongNormal;
        impulseScalar /= (1.0f / mass + 1.0f / other.mass);
        
        // wektor odbicia
        ofVec2f impulse = collisionNormal * impulseScalar;
        velocity += impulse * (1.0f / mass);
        other.velocity -= impulse * (1.0f / other.mass);
        
        //korekcja pozycji aby uniknac zagniecen
        float penetration = minDistance - distance;
        ofVec2f correction = collisionNormal * (penetration / (mass + other.mass)) * 0.8f;
        position += correction * (1.0f / mass);
        other.position -= correction * (1.0f / other.mass);
    }
}

// ofApp implementation
void ofApp::setup() {
    ofSetFrameRate(60);
    ofSetWindowTitle("Disk Simulation - Physical Modeling");
    ofSetCircleResolution(32);
    ofEnableAlphaBlending();

    for (int i = 0; i < 1000; i++) {
        Disk disk;
        float mass = ofRandom(1.0f, 5.0f);
        float radius = ofRandom(5.0f, 15.0f);
        
        // Random hue with full saturation and brightness
        ofColor randomColor;
        randomColor.setHsb(ofRandom(255), 255, 255);
        disk.setup(mass, radius, randomColor);
        disks.push_back(disk);
    }
}

void ofApp::checkAllDiskCollisions() {
    if (!collisionsEnabled) return;
    for (size_t i = 0; i < disks.size(); i++) {
        for (size_t j = i + 1; j < disks.size(); j++) {
            disks[i].checkDiskCollision(disks[j]);
        }
    }
}

void ofApp::update() {
    float dt = ofGetLastFrameTime();
    for (auto& disk : disks) {
        ofVec2f force(0, 0);
        disk.update(force, dt);
        disk.checkBoundaryCollision(ofGetWidth(), ofGetHeight());
    }
    checkAllDiskCollisions();
}


void ofApp::draw() {
    ofBackground(15, 15, 25);
    for (auto& disk : disks) {
        disk.draw();
    }
    ofSetColor(255);
    string info = "Disks: " + ofToString(disks.size()) + 
                 " | FPS: " + ofToString(ofGetFrameRate()) +
                 " | Collisions: " + (collisionsEnabled ? "ON" : "OFF") +
                 "\nSPACE: hold for gravity at cursor | C: collisions | R: reset";
    ofDrawBitmapString(info, 20, 20);
}

void ofApp::keyPressed(int key) {
    if (key == 'c' || key == 'C') {
        collisionsEnabled = !collisionsEnabled;
        ofLog() << "Collisions: " << (collisionsEnabled ? "Enabled" : "Disabled");
    }
    if (key == 'r') {
        setup();
    }
    if (key == 'q') {
        ofExit();
    }
}

void ofApp::keyReleased(int key) {
}

void ofApp::mouseMoved(int x, int y) {
    mousePos.set(x, y);
}