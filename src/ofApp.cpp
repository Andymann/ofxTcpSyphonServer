#include "ofApp.h"


int iCounter=0;
//--------------------------------------------------------------
void ofApp::setup(){
    //https://github.com/armadillu/ofxTurboJpeg/issues/12#issuecomment-406733837
   
    //turbo.load("loadTest.jpg", turboJpegLoadedImage);
    //turbo.load(turboJpegLoadedImage,"loadTest.jpg");
    float framerate = 5;
    ofBackground(0, 0, 0, 0);
    ofSetFrameRate(framerate);
    turbo.load(img1,"loadTest.jpg");
    turbo.load(img2,"loadTest2.jpg");
}

//--------------------------------------------------------------
void ofApp::update(){
    //turbo.load(turboJpegLoadedImage,"loadTest.jpg");
    //ofLogNotice("result" + b);
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    if(iCounter<5){
        turbo.compress(img1, 10, buff);
        imgComp1.load(buff);
        imgComp1.draw(0,0);
    }else if(iCounter<11){
        img2.draw(0,0);
        if(iCounter==10)
            iCounter=0;
    }
    
    
    iCounter++;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
