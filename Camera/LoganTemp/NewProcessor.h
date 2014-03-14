#pragma once
#include "FrameProcessor.h"
struct Block{
	double x;
	double y;
	double area;
	Block(double x, double y, double area) : x(x), y(y), area(area) {}
	double distance2(Block& b) {
	return (x - b.x)*(x - b.x) + (y - b.y)*(y - b.y);
	}
};
struct Combo {
	std::vector<int> combo;
	double combo_value;
	cv::Point center;
	Combo(std::vector<int> c, double v, cv::Point center) : combo(c), combo_value(v), center(center) {}
	bool operator<(Combo c) const {
		return combo_value > c.combo_value;
	}
};

class NewProcessor : public FrameProcessor {
private:
	static const int MIN_BLOCK_AREA;
	static const double distanceWeight;
	static const double areaWeight;
private:
	std::string filename;
private:
	void GenerateMask(cv::Mat& frame, cv::Mat& mask);
	void GenerateBlocks(std::vector<Block>&, cv::Mat& mask, cv::Mat& out);
	void ProcessBlocks(std::vector<Block>&, cv::Mat&);
public:
	NewProcessor(std::string filename) : FrameProcessor("New Processor Output"), filename(filename) {}
	virtual void Process(cv::Mat frame);
};