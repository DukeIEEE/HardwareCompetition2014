#pragma once
#include "FrameProcessor.h"

class WhiteRectangleDetection : public FrameProcessor {
public:
	WhiteRectangleDetection() : FrameProcessor("WhiteRectangleDetection") {}
	virtual void Process(cv::Mat frame);
};