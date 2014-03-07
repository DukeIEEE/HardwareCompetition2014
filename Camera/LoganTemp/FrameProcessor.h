#pragma once

//#define SHOW_IMAGES
class FrameProcessor {
private:
	const std::string window_name;
	cv::Mat img;
public:
	explicit FrameProcessor(std::string window_name) : window_name(window_name) {
#ifdef SHOW_IMAGES
		cv::namedWindow(window_name);
#endif
	}
	//FrameProcessor() : FrameProcessor("Unnamed window") {}
	virtual void Process(cv::Mat)=0;
	
	virtual void PreProcess(CvCapture*){
	};
	void Display();

	std::string get_window_name() {
		return window_name;
	}
	void set_img(cv::Mat img) {
		this->img = img;
	}
	cv::Mat get_img() {
		return img;
	}
};

#include "Utilities.h"