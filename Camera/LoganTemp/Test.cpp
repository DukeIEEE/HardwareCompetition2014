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
		processor->Process(img);
	//processor->Display();
	//std::cout << "done" << std::endl;
	//waitKey(0);
	return 0;
}
