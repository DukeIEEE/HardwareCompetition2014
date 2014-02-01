#pragma once

extern cv::Mat equalizeIntensity(const cv::Mat&);
extern void generateSobelMask(cv::Mat in, cv::Mat & out, int thresh);
//extern Scalar getMedianHSV(cv::Mat &inputImage, int previousMedianH, int tolerance);
