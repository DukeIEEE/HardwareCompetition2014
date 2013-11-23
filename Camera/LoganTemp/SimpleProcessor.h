#pragma once
#include "FrameProcessor.h"

//#define CALIBRATE_WHITE_MASK

#if defined CALIBRATE_RED_MASK || defined CALIBRATE_WHITE_MASK 
#define CALIBRATE
#endif

class SimpleProcessor : public FrameProcessor {
private:
#ifdef CALIBRATE
	int hbegin, hend, slow, shigh, vlow, vhigh;
#endif
public:
	SimpleProcessor();
	virtual void Process(cv::Mat frame);
};