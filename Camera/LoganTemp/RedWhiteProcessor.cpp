#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

//#define UPDATE_RED_MASK
//#define CALIBRATE_HSV
//#define CALIBRATE_RED_MASK
//#define CALIBRATE_WHITE_MASK

#include "RedWhiteProcessor.h"

using namespace cv;

RedWhiteProcessor::RedWhiteProcessor(): FrameProcessor("Output") {
#ifdef CALIBRATE
	createTrackbar("hbegin", get_window_name(), &hbegin, 179);
	createTrackbar("hend", get_window_name(), &hend, 179);
	createTrackbar("slow", get_window_name(), &slow, 255);
	createTrackbar("shigh", get_window_name(), &shigh, 255);
	createTrackbar("vlow", get_window_name(), &vlow, 255);
	createTrackbar("vhigh", get_window_name(), &vhigh, 255);
#endif
}

void RedWhiteProcessor::Process(cv::Mat frame) {
	flip(frame, frame, 1); //flip frame across y-axis
	
	Mat equalized = equalizeIntensity(frame);

	Mat hsv; //hold hsv components

	cvtColor(equalized, hsv, CV_BGR2HSV);

	//find red objects
	Mat red_mask1, red_mask2, red_mask, white_mask;

#ifdef CALIBRATE_RED_MASK
	inRange(hsv, Scalar(0, slow, vlow), Scalar(hbegin, shigh, vhigh), red_mask1);
	inRange(hsv, Scalar(hend, slow, vlow), Scalar(179, shigh, vhigh), red_mask2);
#else
	//inRange(hsv, Scalar(0, 146, 21), Scalar(17, 253, 255), red_mask1);
	//inRange(hsv, Scalar(165, 21, 21), Scalar(179,235, 237), red_mask2);
#endif

	//red_mask = red_mask1;// | red_mask2;

#ifdef CALIBRATE_WHITE_MASK
	inRange(hsv, Scalar(hbegin, slow, vlow), Scalar(hend, shigh, vhigh), white_mask);
#else
	//inRange(hsv, Scalar(83, 5, 186), Scalar(148, 127, 255), white_mask);
#endif

	//Mat mask = red_mask ^ white_mask;

	//namedWindow("White Mask");
	//namedWindow("Red Mask");
	//namedWindow("Mask");

//variables----------------------------------------------------------------

	bool isFirstRun = true;

	int medianRedH, toleranceRedH;
	int medianRedS, toleranceRedS;
	int medianRedV, toleranceRedV;

	int medianWhiteH, toleranceWhiteH;
	int medianWhiteS, toleranceWhiteS;
	int medianWhiteV, toleranceWhiteV;

	if (!isFirstRun) {
		//adaptive median red filter
		////Scalar medianRedH = getMedianH(frame);
		//adaptive median white filter

	}

	Scalar redLow = Scalar(medianRedH - toleranceRedH, medianRedS - toleranceRedS, medianRedV - toleranceRedV);
	Scalar redHigh = Scalar(medianRedH + toleranceRedH, medianRedS + toleranceRedS, medianRedV + toleranceRedV);

	Scalar whiteLow = Scalar(medianWhiteH - toleranceWhiteH, medianWhiteS - toleranceWhiteS, medianWhiteV - toleranceWhiteV);
	Scalar whiteHigh = Scalar(medianWhiteH + toleranceWhiteH, medianWhiteS + toleranceWhiteS, medianWhiteV + toleranceWhiteV);

	if (isFirstRun) {
		redLow = Scalar(0, 146, 21);
		redHigh = Scalar(17, 253, 255);
		whiteLow = Scalar(83, 5, 186);
		whiteHigh = Scalar(148, 127, 255);
	}

	inRange(hsv, redLow, redHigh, red_mask);
	inRange(hsv, whiteLow, whiteHigh, white_mask);


	blur(white_mask, white_mask, Size(5, 5));
	threshold(white_mask, white_mask, 128, 255, CV_THRESH_BINARY);
	//imshow("White Mask", white_mask);
	dilate(red_mask, red_mask, Mat());
	imshow("Red Mask", red_mask);
	//imshow("Mask", mask);

	Mat grayscale;
	cvtColor(equalized, grayscale, CV_BGR2GRAY);
	//namedWindow("Grayscale");
	//imshow("Grayscale", grayscale);

	Mat out;
	threshold(grayscale, out, 200, 255, CV_THRESH_BINARY);
	erode(out, out, Mat());
	dilate(out, out, Mat());
	namedWindow("Threshold");
	imshow("Threshold", out);

	Mat combined = out & white_mask;
	//namedWindow("Combined");
	//imshow("Combined", combined);

	//----------------------------------------------------------------Sobel

	//split image for sobel processing
	vector<Mat> planes;
	split(equalized, planes);
	Mat sobelR, sobelG, sobelB, sobelSat, sobelMask;
	//calculate sobel for red, green, and blue planes
	generateSobelMask(planes[0], sobelB, 230);
	generateSobelMask(planes[1], sobelG, 230);
	generateSobelMask(planes[2], sobelR, 230);

	planes.clear();
	split(hsv, planes);
	//calculate for saturation plane as well
	generateSobelMask(planes[1], sobelSat, 230);

	//combine the masks
	sobelMask = .6 * sobelR + .6 * sobelG + .6 * sobelB + .6 * sobelSat;
	//imshow("SobelR", sobelR);
	//imshow("SobelG", sobelG);
	//imshow("SobelB", sobelB);
	//imshow("SobelSat", sobelSat);

	//threshold(sobelMask, sobelMask, 180, 255, CV_THRESH_BINARY);
	//imshow("SobelMask", sobelMask);

	//----------------------------------------------------------------Find center

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	vector<Point> contour_center;

	RNG rng(12345);
	/// Find contours
	findContours(red_mask, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	/// Draw contours
	//Mat drawing = Mat::zeros( sobelMask.size(), CV_8UC3 );
	for (int i = 0; i< contours.size(); i++)
	{
		double contour_area = contourArea(contours[i]);
		if (contour_area < 1000) continue;
		Moments m = moments(contours[i]);
		Point center(m.m10 / m.m00, m.m01 / m.m00);
		contour_center.push_back(center);
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		circle(equalized, center, 3, Scalar(0, 255, 0), -1, 8, 0);
		drawContours(equalized, contours, i, color, 2, 8, hierarchy, 0, Point());
	}

	if (contour_center.size() > 0) {
		//find contour center
		Point center(0, 0);
		for (vector<Point>::const_iterator iter = contour_center.cbegin(); iter != contour_center.cend(); ++iter)
			center += *iter;
		center.x /= contour_center.size();
		center.y /= contour_center.size();
		circle(equalized, center, 3, Scalar(255, 0, 0), -1, 8, 0);
	}

	/*vector<Vec4i> lines;
	Mat sobel, sobelX, sobelY;
	Sobel(out,sobelX,CV_8U,1,0);
	Sobel(out,sobelY,CV_8U,0,1);
	sobel = sobelX + sobelY;
	HoughLinesP(sobel, lines, 1, CV_PI/180, 50, 50, 10 );
	for( size_t i = 0; i < lines.size(); i++ )
	{
	Vec4i l = lines[i];
	line( equalized, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
	}
	namedWindow("sobel");
	imshow("sobel", sobel);*/


	//findContours(out, contours, 
	set_img(equalized);
}
