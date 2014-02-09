#pragma once
#include "FrameProcessor.h"

class Processor: public FrameProcessor {
public:
	Processor();
	virtual void Process(cv::Mat frame);
};
