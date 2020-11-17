#include "ofApp.h"

static float FRAMERATE = 5.;

static Boolean COMPRESS = true;
static int MESSAGESIZE = 200;
static int COMPRESSION_TURBOJPEG = 0x04;
static int COMPRESSION_JPEG = 0x00;

ofImage imgOut;
static std::vector<unsigned char> bufImg;

bool bBlink=true;

//--------------------------------------------------------------
void ofApp::setup(){
    //https://github.com/armadillu/ofxTurboJpeg/issues/12#issuecomment-406733837
   
    
    NDIlib_initialize();
    ofSetBackgroundColor(0, 0, 0);
    //ofSetVerticalSync(true);
    ofSetFrameRate(FRAMERATE);
    ofSetEscapeQuitsApp(false);
    ofSetWindowTitle("ASD");
    
    grabber_.setUseTexture(false);
    grabber_.setup(ofxNDI::listSources()[0]);//----first ndi source
    
    setupServer("127.0.0.1",9999);

}

void ofApp::setupServer(string pIP, int pPort){
    ofxTCPSettings settings = ofxTCPSettings(pIP, pPort);
    tcpServer.setup(settings);
    ofLogNotice("setupServer(): Done");
}
//--------------------------------------------------------------


void ofApp::update(){
    if(grabber_.isConnected()) {
        grabber_.update();
        if(grabber_.isFrameNew()) {
            processNDI(COMPRESS, 5);
            if( bBlink ){
                ofSetBackgroundColor(0, 0, 0);
            }else{
                ofSetBackgroundColor(200, 0, 0);
            }
            
        }
    }
}


void ofApp::processNDI(Boolean pCompress, int pPercent){
    int iCompression;
    ofPixels p1;
    ofPixels p2;
    ofBuffer tmpBuff;
    //pixels_.setImageType(OF_IMAGE_COLOR);
    pixels_ = grabber_.getPixels();
                
    pixels_.cropTo(p1, 0,0, pixels_.getWidth()/2, pixels_.getHeight()/2);
    pixels_.cropTo(p2, pixels_.getWidth()/2, pixels_.getHeight()/2, pixels_.getWidth()/2, pixels_.getHeight()/2);

    if(p1.size()>0){
        img1.setFromPixels(p1);
        img2.setFromPixels(p2);
        
        
        if(pCompress){
            //----that's some expensive sh*t
            turbo.compress(img1, pPercent, tmpBuff);
            imgComp1.load(tmpBuff);
            
            turbo.compress(img2, pPercent, tmpBuff);
            imgComp2.load(tmpBuff);
            iCompression=COMPRESSION_TURBOJPEG;
        }else{
            imgComp1 = img1;
            imgComp2 = img2;
            iCompression=COMPRESSION_JPEG;
        }
        
        imgComp1.setImageType(OF_IMAGE_GRAYSCALE);
        createImage( imgComp1, iCompression );
        sendData(  );
    }
}

/*
    Image rein, buffer array raus, mit header daten
    adding 16byte-header to the image-data.
 */
void ofApp::createImage(ofImage pImage, int pCompression){
    ofBuffer ofBuff;
    
    //---Imagedata into Buffer;
    ofSaveImage(pImage.getPixelsRef(),ofBuff,OF_IMAGE_FORMAT_JPEG);
    
    bufImg.resize(0);
    bufImg.push_back(pCompression);
    bufImg.push_back(0);
    bufImg.push_back(0);
    bufImg.push_back(0);
    bufImg.push_back((int)pImage.getWidth() & 0xff);
    bufImg.push_back((int)pImage.getWidth() >> 8);
    bufImg.push_back(0);
    bufImg.push_back(0);
    bufImg.push_back((int)pImage.getHeight() & 0xff);
    bufImg.push_back((int)pImage.getHeight() >> 8);
    bufImg.push_back(0);
    bufImg.push_back(0);
    bufImg.push_back((int)(ofBuff.size()+16) & 0xff);
    bufImg.push_back((int)(ofBuff.size()+16) >> 8);
    bufImg.push_back(0);
    bufImg.push_back(0);
    
    
    char * p = ofBuff.getBinaryBuffer();
    int iImageSize = ofBuff.size() / sizeof(char);    //----Just the picture without header
    for (int i = 0; i < iImageSize; i++) {
        bufImg.push_back(p[i]);
    }
    
    //Original anzeigen
    imgOut = pImage;
    
    
    /*
    //----Sanity Test für die JPG-Magic Bytes
    char a = bufImg[16];
    char b = bufImg[17];
    char c = bufImg[bufImg.size()-2];
    char d = bufImg[bufImg.size()-1];
    
    string s;
    s.push_back(a);
    s.push_back(' ');
    s.push_back(b);
    s.push_back(' ');
    s.push_back(c);
    s.push_back(' ');
    s.push_back(d);
    s.push_back(' ');
    ofLogNotice(ofToString(s));
    */
    
    //----Test if we are working correctly:
    //int imageBytes = (int)((bufImg[13]<<8) + bufImg[12]); //Header + Image
    //ofLogNotice("Header:" + ofToString(imageBytes) + " " + ofToString(iImageSize));

}

void ofApp::sendData( ){
        
    //buffImg hold header + Image data
    int iBytesToSend = bufImg.size();
    int totalBytesSent = 0;
    
    bool bSent=false;
    bool bHasConnect = false;
    
    while( iBytesToSend > 1 ) {

        int iLastID = tcpServer.getLastID();
        if(iLastID==0){
            //----No clients, no Action
            return;
        }
        
        bHasConnect = false;
        for(int i = 0; i < iLastID ; i++){
            if( tcpServer.isClientConnected(i) ) { // check and see if it's still around
                bHasConnect = true;
                if(iBytesToSend >= MESSAGESIZE) {
                    tcpServer.sendRawBytes(i, (char*) &bufImg[totalBytesSent], MESSAGESIZE);
                    iBytesToSend -= MESSAGESIZE;
                    totalBytesSent += MESSAGESIZE;
                } else {
                     //We're padding so we don't have to deal with this on the client side
                    int iRest = MESSAGESIZE-iBytesToSend;
                     // Yeah, well ... whatever
                    for(int i=0; i<iRest; i++){
                        bufImg.push_back('\x0');
                    }
                    tcpServer.sendRawBytes(i, (char*) &bufImg[totalBytesSent], MESSAGESIZE);
                    
                    /*
                    // Send without padding
                    tcpServer.sendRawBytes(i, (char*) &bufImg[totalBytesSent], iBytesToSend);
                    */
                     totalBytesSent += iBytesToSend;
                    iBytesToSend = 0;
                }
                
                if(iBytesToSend==0){
                    bSent=true;
                }
            }
        }
        
        if(bHasConnect==false){
            //----None of the formerly available Clients is there
            //ofLogNotice("Kein Client, nichts zu senden");
            //iBytesToSend=0;
            return;
        }
    }//while
    if(bSent){
        //ofLogNotice("Server: Image sent. Size:" + ofToString( (bufImg[13]<<8) + bufImg[12] ) );
         bBlink=!bBlink;
    }
    
}

void ofApp::test(){
    //tmpBuff enthaelt auch den 16 byte Header
    //int iPictureSize= (int)((bufImg[13]<<8) + (bufImg[12]));
    int imageBytes = (int)((bufImg[13]<<8) + bufImg[12]); //Header + Image
    //----Testweises anzeigen
    char tmpBuff[ /*bufImg.size()*/imageBytes-16];
    memset(tmpBuff, 0, sizeof tmpBuff);
    //memcpy( tmpBuff, &syphonImage + 16, gSize );


    for(int i=0; i</*gSize*/imageBytes; i++){
        tmpBuff[i] = *&bufImg[i+16];
    }
    //    buf.set((tmpBuff), imageBytes-16/*sizeof tmpBuff/sizeof(char)*/);
    //    imgOut.load(buf);
}



//--------------------------------------------------------------
void ofApp::draw(){
    
    if(imgOut.isAllocated())
        imgOut.draw(10,10);
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
//------------------------------------------------
