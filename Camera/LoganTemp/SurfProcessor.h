#pragma once
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "FrameProcessor.h"

class SurfProcessor : public FrameProcessor {
public:
	SurfProcessor() : FrameProcessor("SurfProcessor") {}
	virtual void Process(cv::Mat frame);
	virtual void PreProcess(CvCapture*);
};