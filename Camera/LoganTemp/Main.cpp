#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <memory>

#include "SimpleProcessor.h"
#include "WhiteRectangleDetection.h"
#include "SurfProcessor.h"
#include "RedWhiteProcessor.h"
using namespace cv;

int main() {
	std::unique_ptr<FrameProcessor> processor(new SurfProcessor());//new WhiteRectangleDetection());
	CvCapture* capture;
	Mat frame;

	//grab camera capture
	capture = cvCaptureFromCAM(0);
	if(!capture){
		capture = cvCaptureFromCAM(-1);
	}
	if (capture) {
		processor->PreProcess(capture);
		while (true) {
			frame = cvQueryFrame(capture);
			if (!frame.empty())
				processor->Process(frame);
			else {
				std::cerr << " --(!) No captured frame -- Break!" << std::endl;
				break;
			}

			processor->Display();
			int c = waitKey(10);
			if ((char) c == 'q') break;
		}
	}
	else {
		return 1;
	}
	return 0;
}
