#pragma once

#include "ofMain.h"
#include "ofxTurboJpeg.h"
#include "ofxNDIReceiver.h"
#include "ofxNDIRecvStream.h"
#include "ofxNDIVideoGrabber.h"
#include "ofxTCPServer.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    void setupServer(int pPort);
    void processNDI();
    
    ofxTurboJpeg turbo;
    ofImage img1;    //turboJpg
    ofImage img2;
    ofImage imgComp1;
    ofImage imgComp2;
    ofTexture tex1;
    ofTexture tex2;
    
    ofBuffer buff;
    ofxTCPServer tcpServer;
    //ofxNDIreceiver ndiReceiver; // NDI receiver
private:
    ofxNDIReceiver receiver_;
    ofxNDIRecvVideoFrameSync video_;
    ofxNDIVideoGrabber grabber_;
    
    ofPixels pixels_;
    
		
};
