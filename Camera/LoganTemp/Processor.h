#pragma once
#include "FrameProcessor.h"
//#define RED_HSV
//#define DIST_AREA
//#define HORIZONTAL_VERTICAL
#if defined RED_HSV || defined DIST_AREA || defined HORIZONTAL_VERTICAL
#define CALIBRATE
#endif

class Processor : public FrameProcessor {
private:
	int hlow = 3, hhigh = 170;
	int slow = 0, shigh = 0;
	int vlow = 0, vhigh = 121;

	int areaWeight = 1, distanceWeight = 2;

	int hMaxDiff = 14, vMaxDiff = 14;
public:
	Processor();
	virtual void Process(cv::Mat frame);
};
