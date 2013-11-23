#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "FrameProcessor.h"

void FrameProcessor::Display() {
	cv::imshow(window_name, img);
}

