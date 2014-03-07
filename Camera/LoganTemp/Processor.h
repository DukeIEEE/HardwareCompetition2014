#pragma once
#include "FrameProcessor.h"
//#define HORIZONTAL_VERTICAL

class Processor : public FrameProcessor {
private:
	std::string trackbarWindowName;
	int areaWeight, distanceWeight;
public:
	Processor();
	virtual void Process(cv::Mat frame);
};
