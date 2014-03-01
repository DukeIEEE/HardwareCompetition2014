#pragma once
#include "FrameProcessor.h"

class Processor : public FrameProcessor {
private:
	std::string trackbarWindowName = "area/distance";
	int areaWeight = 1, distanceWeight =2;
public:
	Processor();
	virtual void Process(cv::Mat frame);
};
