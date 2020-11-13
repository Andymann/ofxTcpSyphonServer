#include "ofApp.h"
//using namespace ofxNDI::Recv;

float fFramerate = 25.;
int iCounter=0;
Boolean bCompress = false;
//--------------------------------------------------------------
void ofApp::setup(){
    //https://github.com/armadillu/ofxTurboJpeg/issues/12#issuecomment-406733837
   
    
    
    NDIlib_initialize();
    ofSetBackgroundColor(0, 0, 0);
    ofSetVerticalSync(true);
    ofSetFrameRate(fFramerate);
    //ofSetEscapeQuitsApp(false);
    ofSetWindowTitle("ASD");
    
    
    /*
    auto findSource = [](const string &name_or_url) {
       
        auto sources = ofxNDI::listSources();
        if(name_or_url == "") {
            return make_pair(ofxNDI::Source(), false);
        }
        auto found = find_if(begin(sources), end(sources), [name_or_url](const ofxNDI::Source &s) {
            return ofIsStringInString(s.p_ndi_name, name_or_url) || ofIsStringInString(s.p_url_address, name_or_url);
        });
        if(found == end(sources)) {
            ofLogWarning("ofxNDI") << "no NDI source found by string:" << name_or_url;
            return make_pair(ofxNDI::Source(), false);
        }
        return make_pair(*found, true);
    };
    
    string name_or_url = "";    // Specify name or address of expected NDI source. In case of blank or not found, receiver will grab default(which is found first) source.
    auto result = findSource(name_or_url);
    if(result.second ? receiver_.setup(result.first) : receiver_.setup()) {
        video_.setup(receiver_);
    }
    */
    
    grabber_.setUseTexture(false);
    grabber_.setup(ofxNDI::listSources()[0]);
    grabber_.setDesiredFrameRate(fFramerate);
    
    //pixels_.allocate(2047, 2048, 3);
    
    setupServer(9000);
    
}


void ofApp::setupServer(int pPort){
    tcpServer.setup(pPort);
}
//--------------------------------------------------------------
void ofApp::update(){
    
    if(grabber_.isConnected()) {
        grabber_.update();
        if(grabber_.isFrameNew()) {
            processNDI();
        }
    }
}

void ofApp::processNDI(){
    ofPixels p1;
    ofPixels p2;
    //pixels_.setImageType(OF_IMAGE_COLOR_ALPHA);
    pixels_ = grabber_.getPixels();
                
    pixels_.cropTo(p1, 0,0, pixels_.getWidth()/2, pixels_.getHeight()/2);
    pixels_.cropTo(p2, pixels_.getWidth()/2, pixels_.getHeight()/2, pixels_.getWidth()/2, pixels_.getHeight()/2);

    img1.setFromPixels(p1);
    img2.setFromPixels(p2);
    
    
    if(bCompress){
        turbo.compress(img1, 5, buff);
        imgComp1.load(buff);
        
        turbo.compress(img2, 5, buff);
        imgComp2.load(buff);
    }else{
        imgComp1 = img1;
        imgComp2 = img2;
    }
}


//--------------------------------------------------------------
void ofApp::draw(){
        
    if(pixels_.isAllocated()) {
       ofImage(imgComp1).draw(0,0);
       ofImage(imgComp2).draw(imgComp2.getWidth(),imgComp2.getHeight());
    }
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
