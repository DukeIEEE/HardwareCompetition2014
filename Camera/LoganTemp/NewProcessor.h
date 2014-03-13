#pragma once
#include "FrameProcessor.h"

class NewProcessor : public FrameProcessor {
private:
	struct Block{
	  double x;
	  double y;
	  double area;
	  Block(double x, double y, double area) : x(x), y(y), area(area) {}
	};
public:
	NewProcessor() : FrameProcessor("New Processor Output") {}
	virtual void Process(cv::Mat frame);
};