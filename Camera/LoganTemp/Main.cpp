#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <memory>

#include "WhiteRectangleDetection.h"

#include "SimpleProcessor.h"
#include "RedWhiteProcessor.h"

using namespace cv;

int main() {
	//std::unique_ptr<FrameProcessor> processor(new WhiteRectangleDetection());

	//std::unique_ptr<FrameProcessor> processor(new SimpleProcessor());
	std::unique_ptr<FrameProcessor> processor(new RedWhiteProcessor());

	CvCapture* capture;
	Mat frame;

   //grab camera capture
	capture = cvCaptureFromCAM(0);
   if( capture ) {
     while (true) {
		frame = cvQueryFrame( capture );
       if( !frame.empty() )
		   processor->Process(frame);
       else { 
		   std::cerr << " --(!) No captured frame -- Break!" << std::endl; 
		   break; 
	   }
	   processor->Display();
	   int c = waitKey();//10);
	   if ((char)c == 'q') break;
      }
   }
   return 0;
}
