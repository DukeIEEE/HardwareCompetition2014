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
	Mat img = imread(argv[1]);
	std::unique_ptr<FrameProcessor> processor(new Processor());//new WhiteRectangleDetection());
		if (!img.empty())
			processor->Process(img);
	return 0;
}
