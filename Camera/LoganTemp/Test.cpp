#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <memory>

#include "FrameProcessor.h"
#include "Processor.h"


using namespace cv;

int main(int argc, char**argv) {
	//std::cout << "araf " << argv[1] << std::endl;
	Mat img = imread(argv[1]);
	//imshow("zsdfads", img); 
	std::unique_ptr<FrameProcessor> processor(new Processor());//new WhiteRectangleDetection());
	if (!img.empty())
		resize(img, img, Size(640, 480));
		processor->Process(img);
	//processor->Display();
	//std::cout << "done" << std::endl;
	//waitKey(0);
	return 0;
}

int main1() {
	std::unique_ptr<FrameProcessor> processor(new Processor());//new WhiteRectangleDetection());
	Mat frame;

	frame = imread("C:\\Users\\User\\Documents\\Visual Studio 2013\\Projects\\HardwareCompetition2014\\Camera\\TestSet\\test\\image_10.jpg");
	while (!frame.empty()) {
		resize(frame, frame, Size(640, 480));
		processor->Process(frame);
		processor->Display();
		waitKey(100);
	}
}
