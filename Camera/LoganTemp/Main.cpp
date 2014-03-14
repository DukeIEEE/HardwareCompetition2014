#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <memory>

#include "Processor.h"
#include "SimpleProcessor.h"
#include "NewProcessor.h"

using namespace cv;

int main0() {
	std::unique_ptr<FrameProcessor> processor(new NewProcessor(""));//new WhiteRectangleDetection());
	CvCapture* capture;
	Mat frame;

	capture = cvCaptureFromCAM(0);
	if (!capture) {
		capture = cvCaptureFromCAM(-1);
	}
	if (capture) {
		processor->PreProcess(capture);
	}
	else {
		return 0;
	}

	while (true) {
		frame = cvQueryFrame(capture);
		if (!frame.empty())
			processor->Process(frame);
		else {
			break;
		}

		processor->Display();
		int c = waitKey(100);
		if ((char)c == 'q') {
			break;
		}
	}
	return 0;
}
