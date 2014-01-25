#pragma once

class FrameProcessor {
private:
	const std::string window_name;
	cv::Mat img;
public:
	explicit FrameProcessor(std::string window_name) : window_name(window_name) {
		cv::namedWindow(window_name);
	}
	//FrameProcessor() : FrameProcessor("Unnamed window") {}
	virtual void Process(cv::Mat)=0;

	void Display();

	std::string get_window_name() {
		return window_name;
	}
	void set_img(cv::Mat img) {
		this->img = img;
	}
};

#include "Utilities.h"