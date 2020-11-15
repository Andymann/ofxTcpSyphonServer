#include "ofApp.h"
//using namespace ofxNDI::Recv;

float fFramerate = 25;
int iCounter=0;
Boolean bCompress = true;

static int COMPRESSION_TURBOJPEG = 0x04;
static int COMPRESSION_JPEG = 0x00;

int gSize = 0;

ofBuffer buf;
ofImage imgOut;
static std::vector<char> bufImg;


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
    int iCompression;
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
            iCompression=COMPRESSION_TURBOJPEG;
        }else{
            imgComp1 = img1;
            imgComp2 = img2;
            iCompression=COMPRESSION_JPEG;
        }
        
        //ofPixels px;
        //px = imgComp1.getPixels();
        //ofSaveImage(px, "test.jpg");
       // ofSaveImage(imgComp1.getTexture().readToPixels(px), "testout.jpg");
        
        imgComp1.setImageType(OF_IMAGE_GRAYSCALE);
        //char* syphonImage = NULL;
        /*char* syphonImage = */createImage( imgComp1, iCompression );
        /*
        char a = syphonImage[14];
        char b = syphonImage[15];
        char c = syphonImage[16];
        char d = syphonImage[17];
        */
        
        /*
        char s = syphonImage[gSize-1];
        char t = syphonImage[gSize];
        char u = syphonImage[gSize+1];
        char v = syphonImage[gSize+2];
        
        char w = syphonImage[gSize+14];
        char x = syphonImage[gSize+15];
        char y = syphonImage[gSize+16];
        char z = syphonImage[gSize+17];
        */
        
/*
        //----Testweises anzeigen
        char tmpBuff[gSize];
        memset(tmpBuff, 0, sizeof tmpBuff);
        //memcpy( tmpBuff, &syphonImage + 16, gSize );
        for(int i=0; i<gSize; i++){
            tmpBuff[i] = *&syphonImage[i+16];
        }
        buf.set((tmpBuff), gSize);
        imgOut.load(buf);
*/
        
        sendData( /*syphonImage*/ );
        //free(&syphonImage);
    }
}



void ofApp::sendData( /*char* pBuffer*/){
    
    //----Testweises anzeigen
    
    char tmpBuff[ bufImg.size()-16 ];
    memset(tmpBuff, 0, sizeof tmpBuff);
    //memcpy( tmpBuff, &syphonImage + 16, gSize );
    for(int i=0; i<gSize; i++){
        tmpBuff[i] = *&bufImg[i+16];
    }
    buf.set((tmpBuff), gSize);
    imgOut.load(buf);
    
    
    /*
    int iSize = pImage.getPixels().getTotalBytes();
    int imageBytesToSend = iSize;
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
    */
}




/*
    Image rein, buffer array raus, mit header daten
    adding 16byte-header to the imagedata.
 */
/*char * */ void ofApp::createImage(ofImage pImage, int pCompression){
    ofBuffer buff;
    ofSaveImage(pImage.getPixelsRef(),buff,OF_IMAGE_FORMAT_JPEG);
    //char bufImg[buff.size()+16];//= new char[buff.size()+16];
   
    
    /*
     //----mit array
    //char *p = (char *)buff.getData();
    char * p = buff.getBinaryBuffer();
    int size = buff.size() / sizeof(char);
    for (int i = 0; i < size; i++) {
        bufImg[i+16] = p[i];
    }
     
     bufImg[0] = pCompression;
     bufImg[4]= (int)pImage.getWidth() & 0xff;
     bufImg[5]= (int)pImage.getWidth() >> 8;
     bufImg[8]= (int)pImage.getHeight() & 0xff;
     bufImg[9]= (int)pImage.getHeight() >> 8;
     bufImg[12]= (int) (buff.size()+16) & 0xff;
     bufImg[13]= (int) (buff.size()+16) >> 8;

     */
    
    bufImg.clear();
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
    bufImg.push_back((int) (buff.size()+16) & 0xff);
    bufImg.push_back((int) (buff.size()+16) >> 8);
    bufImg.push_back(0);
    bufImg.push_back(0);
    
    char * p = buff.getBinaryBuffer();
    int size = buff.size() / sizeof(char);
    for (int i = 0; i < size; i++) {
        bufImg.push_back(p[i]);
    }

    //ofLogNotice("test" + bufImg[17]);
    //ofLogNotice("createImage:" + ofToString(pImage.getWidth()) + "*" + ofToString(pImage.getHeight()));
    
    //ofLogNotice("createImage:" + ofToString(bufImg[4]) + " " + ofToString(bufImg[5]) );
    
    //ofLogNotice("createImage size:" + ofToString(size));
    
    gSize = size;
    
    /*
    //----bis hierhin funktioniert es
    char tmpBuff[gSize];
    memcpy( tmpBuff, bufImg + 16, gSize );
    buf.set((tmpBuff), gSize);
    imgOut.load(buf);
    */
    
    
    
    //char *pRet = bufImg;
    //char *pRet = &bufImg[0];
    //return pRet;
    //return(&bufImg[0]);
    
    /*
    byte upper = ((Byte) (buff.size()+16) >> 8);
    byte lower = ((Byte) (buff.size()+16) & 0xff);
    */
    
}




//--------------------------------------------------------------
void ofApp::draw(){
    /*
    if(pixels_.isAllocated()) {
        //ofImage(imgComp1).resize(<#int newWidth#>, <#int newHeight#>)
       ofImage(imgComp1).draw(0,0);
       ofImage(imgComp2).draw(imgComp2.getWidth(),imgComp2.getHeight());
    }
    */
/*
        if(imgOut.isAllocated())
            imgOut.draw(0,0);

*/
    
    if(imgOut.isAllocated())
        imgOut.draw(0,0);
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
