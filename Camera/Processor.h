#pragma once
#include "FrameProcessor.h"

class Processor: public FrameProcessor {
public:
	Processor() : FrameProcessor("Output") {}
	virtual void Process(cv::Mat frame);
};
