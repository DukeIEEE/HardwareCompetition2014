#pragma once
#include "FrameProcessor.h"

class RedWhiteProcessor : public FrameProcessor {
public:
	RedWhiteProcessor();
	virtual void Process(cv::Mat frame);
};
