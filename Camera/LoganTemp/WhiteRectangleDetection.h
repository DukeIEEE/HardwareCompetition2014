#pragma once
#include "FrameProcessor.h"

#define CALIBRATE_WHITE_MASK
//#define CALIBRATE_RED_MASK
#if defined CALIBRATE_RED_MASK || defined CALIBRATE_WHITE_MASK 
	#define CALIBRATE
#endif

class WhiteRectangleDetection : public FrameProcessor {
private:
#ifdef CALIBRATE
	int hbegin, hend, slow, shigh, vlow, vhigh;
#endif
public:
	WhiteRectangleDetection();// : FrameProcessor("WhiteRectangleDetection") {}
	virtual void Process(cv::Mat frame);
};