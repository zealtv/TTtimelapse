class snapshot : public ofThread {
  
public:
  
  ofFbo *fbo;
  ofPixels *pixels;
  ofImage image;
  int numFrames;
  string outPath;
  string camName;
  bool doDateStamp;
  
  
  
  void setup( ofPixels *inPix, int i, string path, string cname, bool dateStamp )
  {
    doDateStamp = dateStamp;
    //doDateStamp = true;
    numFrames = i;
    outPath = path;
    pixels = inPix;
    camName = cname;
    image.setUseTexture( false );
  }
  

  
  // the thread function
  void threadedFunction() {
  
    while(isThreadRunning()) {

      lock();
      image.setFromPixels( *pixels );
      unlock();
    
    
      string intString;
      ostringstream ss;
      ss << std::setw( 4 ) << std::setfill( '0' ) << numFrames;
      intString = ss.str();
      
      ostringstream monthstream;
      monthstream << std::setw( 2 ) << std::setfill( '0' ) << ofGetMonth();
      ostringstream minutestream;
      minutestream << std::setw( 2 ) << std::setfill( '0' ) << ofGetMinutes();
      ostringstream secondstream;
      secondstream << std::setw( 2 ) << std::setfill( '0' ) << ofGetSeconds();
      
      
      string date = ofToString( ofGetYear() ) + "-" + monthstream.str() + "-" + ofToString( ofGetDay() );
      string time = ofToString( ofGetHours() ) + "-" + minutestream.str() + "-" + secondstream.str();
    
      string outString = "";
    
      if( doDateStamp )
      {
        outString = outPath + camName + " - " +  date + " " + time +  ".png";
      }
      else
      {
        outString = outPath + camName + intString +  ".png";
      }
    
    
      cout << outString << endl;
      ofLog( OF_LOG_NOTICE, "Writing: " +  outString );
      image.saveImage( outString );
      numFrames++;
    
      stopThread();
      
    }
  }
};