#include "ofApp.h"
//using namespace ofxNDI::Recv;

float fFramerate = 25.;
int iCounter=0;
Boolean bCompress = true;

static int COMPRESSION_TURBOJPEG = 0x04;
static int COMPRESSION_JPEG = 0x00;


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
    grabber_.setup(ofxNDI::listSources()[0]);//----first ndi source
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
            processNDI(bCompress, 5);
        }
    }
}

void ofApp::processNDI(Boolean pCompress, int pPercent){
    ofPixels p1;
    ofPixels p2;
    pixels_.setImageType(OF_IMAGE_COLOR);
    pixels_ = grabber_.getPixels();
                
    pixels_.cropTo(p1, 0,0, pixels_.getWidth()/2, pixels_.getHeight()/2);
    pixels_.cropTo(p2, pixels_.getWidth()/2, pixels_.getHeight()/2, pixels_.getWidth()/2, pixels_.getHeight()/2);

    if(p1.size()>0){
        img1.setFromPixels(p1);
        img2.setFromPixels(p2);
        
        
        if(pCompress){
            //----that's some expensive sh*t
            turbo.compress(img1, pPercent, buff);
            imgComp1.load(buff);
            
            turbo.compress(img2, pPercent, buff);
            imgComp2.load(buff);
            
        }else{
            imgComp1 = img1;
            imgComp2 = img2;
        }
        
        //ofPixels px;
        //px = imgComp1.getPixels();
        //ofSaveImage(px, "test.jpg");
       // ofSaveImage(imgComp1.getTexture().readToPixels(px), "testout.jpg");
        imgComp1.setImageType(OF_IMAGE_GRAYSCALE);
        createImage( imgComp1 );
        sendData(imgComp1);
    }
}

/*
    Image rein, buffer array raus, mit header daten
    adding 16byte-header to the imagedata.
 */
ofBuffer ofApp::createImage(ofImage pImage){
    ofBuffer buff;
    ofSaveImage(pImage.getPixelsRef(),buff,OF_IMAGE_FORMAT_JPEG);
    char buff2[buff.size()+16];
    char *p = (char *)buff.getData();
    int size = buff.size() / sizeof(char);
    //for(int i=0; i<buff.size(); i++){
    for (int i = 0; i < size; i++) {
        buff2[i+16] = p[i];
    }
    
    
    //ofLogNotice( ofToString(buff.size()) );

    return (ofBuffer)buff2;
}


void ofApp::sendData(ofImage pImage){
    //ofImage img;
    //img.loadImage("tmp.jpg");
    int iSize = pImage.getPixels().getTotalBytes();
    int imageBytesToSend = /*7800*/ iSize;
    int totalBytesSent = 0;
    int messageSize = 200;
    while( imageBytesToSend > 1 )
    {

        if(imageBytesToSend > messageSize) {
            tcpServer.sendRawBytesToAll((char*) &pImage.getPixels()[totalBytesSent], messageSize);
            imageBytesToSend -= messageSize;
            totalBytesSent += messageSize;
        } else {
            tcpServer.sendRawBytesToAll( (char*) &pImage.getPixels()[totalBytesSent], imageBytesToSend);
            totalBytesSent += imageBytesToSend;
            imageBytesToSend = 0;
        }
    }
}


//--------------------------------------------------------------
void ofApp::draw(){
        
    if(pixels_.isAllocated()) {
        //ofImage(imgComp1).resize(<#int newWidth#>, <#int newHeight#>)
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
