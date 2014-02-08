#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

#include "RedWhiteProcessor.h"

using namespace cv;

//variables----------------------------------------------------------------

bool isFirstRun;

Scalar redLow;
Scalar redHigh;
Scalar whiteLow;
Scalar whiteHigh;

Vec3b averageRedHSV;
Vec3b redTolerance;
Vec3b averageWhiteHSV;
Vec3b whiteTolerance;

RedWhiteProcessor::RedWhiteProcessor(): FrameProcessor("Output") {
	isFirstRun = true;
}

//functions----------------------------------------------------------------

Vec3b toVec3b(Scalar s) {
	Vec3b vec;
	for (int i = 0; i < 3; i++) {
		vec.val[i] = s.val[i];
	}
	return vec;
}

Scalar toScalar(Vec3b vec) {
	Scalar s;
	for (int i = 0; i < 3; i++) {
		s.val[i] = vec.val[i];
	}
	return s;
}

bool areEqual(Vec3b v1, Vec3b v2) {
	for (int i = 0; i < 3; i++) {
		if (v1.val[i] != v2.val[i]) {
			return false;
		}
	}
	return true;
}

bool isWithin(Vec3b vec, Vec3b previousVec, Vec3b tolerance) {
	Vec3b lower = previousVec - tolerance;
	Vec3b upper = previousVec + tolerance;
	//works because vec.val[i] cannot be lower than 0
	return ((vec - upper)==Vec3b(0,0,0)) && ((lower-vec) == Vec3b(0, 0, 0));
}

Vec3b getAverageHSV(Mat img, Vec3b previousAverage, Vec3b tolerance) {
	int rows = img.rows;
	int cols = img.cols;

	//vector<Mat> channels;
	//split(previousImg, channels);
	//Mat HImg = channels[0];
	//Mat SImg = channels[1];
	//Mat VImg = channels[2];

	Vec3b sumHSV = Vec3b(0, 0, 0);
	int numPixels = 0;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			Vec3b pixelVals = img.at<Vec3b>(i, j);
			if (isWithin(pixelVals, previousAverage, tolerance)) {
				sumHSV += img.at<Vec3b>(i, j);
				numPixels++;
			}
		}
	}

	if (numPixels < rows * cols / 10) { //at least 10% of pixels
		return previousAverage;
	}
	return sumHSV / numPixels;
}

void RedWhiteProcessor::Process(cv::Mat frame) {
	flip(frame, frame, 1); //flip frame across y-axis
	Mat equalized = equalizeIntensity(frame);
	Mat hsv; //hold hsv components
	cvtColor(equalized, hsv, CV_BGR2HSV); //find red and white objects

	if (isFirstRun) {
		redLow = Scalar(0, 204, 0);
		redHigh = Scalar(32, 240, 255);
		whiteLow = Scalar(83, 5, 186);
		whiteHigh = Scalar(148, 127, 255);

		averageRedHSV = (toVec3b(redHigh) + toVec3b(redLow)) / 2;
		redTolerance = (toVec3b(redHigh) - toVec3b(redLow)) / 2;
		averageWhiteHSV = (toVec3b(whiteHigh) + toVec3b(whiteLow)) / 2;
		whiteTolerance = (toVec3b(whiteHigh) - toVec3b(whiteLow)) / 2;
		std::cout << "a" << std::endl;
		isFirstRun = false;
	}
	else {
		//adaptive mean filter

		averageRedHSV = getAverageHSV(frame, averageRedHSV, redTolerance);
		averageWhiteHSV = getAverageHSV(frame, averageWhiteHSV, whiteTolerance);

		redLow = toScalar(averageRedHSV - redTolerance);
		redHigh = toScalar(averageRedHSV + redTolerance);
		whiteLow = toScalar(averageWhiteHSV - whiteTolerance);
		whiteHigh = toScalar(averageWhiteHSV + whiteTolerance);
	}

	Mat red_mask, white_mask, mask, mask2, temp_mask, mask_A, mask_B, mask_C, mask_sam;

	inRange(hsv, redLow, redHigh, temp_mask);

	inRange(hsv, Scalar(0, 0, 0), Scalar(4, 255, 255), mask);
	inRange(hsv, Scalar(173, 0, 0), Scalar(255, 255, 255), mask2);
	red_mask = (mask | mask2) & temp_mask;
	inRange(hsv, whiteLow, whiteHigh, white_mask);
	inRange(hsv, Scalar(8, 0, 0), Scalar(127, 255, 255), mask_A);
	inRange(hsv, Scalar(0, 0, 0), Scalar(255, 131, 255), mask_B);
	//inRange(hsv, Scalar(18, 0, 0), Scalar(133, 179, 179), mask_C);
	mask_A = ~mask_A;
	mask_B = ~mask_B;
	//mask_C = ~mask_C;
	mask_sam = mask_A & mask_B;// &mask_C;

	//Everything beyond here is copied code----------------------------------------------------------------

	blur(white_mask, white_mask, Size(5, 5));
	threshold(white_mask, white_mask, 128, 255, CV_THRESH_BINARY);
	//imshow("White Mask", white_mask);

	//dilate(red_mask, red_mask, Mat());
	//imshow("Red Mask", red_mask);

	dilate(mask, mask, Mat());
	//imshow("Not red Mask", mask);
	imshow("Sam Mask", mask_sam);

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

	//Sobel----------------------------------------------------------------

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

	//--------------------------------------------------------------------------------------------------------------------------------Contours

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	vector<Point> contour_center;

	RNG rng(12345);
	/// Find contours
	findContours(mask_sam, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	//--------------------------------------------------------------------------------------------------------------------------------old

	//for (int i = 0; i< contours.size(); i++)
	//{
	//	double contour_area = contourArea(contours[i]);
	//	if (contour_area < 1000) continue;
	//	Moments m = moments(contours[i]);
	//	Point center(m.m10 / m.m00, m.m01 / m.m00);
	//	contour_center.push_back(center);
	//	Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
	//	circle(equalized, center, 3, Scalar(0, 255, 0), -1, 8, 0);
	//	drawContours(equalized, contours, i, color, 2, 8, hierarchy, 0, Point());
	//}

	//if (contour_center.size() > 0) {
	//	//find contour center
	//	Point center(0, 0);
	//	for (vector<Point>::const_iterator iter = contour_center.cbegin(); iter != contour_center.cend(); ++iter)
	//		center += *iter;
	//	center.x /= contour_center.size();
	//	center.y /= contour_center.size();
	//	circle(equalized, center, 3, Scalar(255, 0, 0), -1, 8, 0);
	//}

	struct Block{
		double x;
		double y;
		double area;
		Block(double x, double y, double area) : x(x), y(y), area(area) {}
	};

	vector<Block> blocks;
	/// Draw contours
	//Mat drawing = Mat::zeros( sobelMask.size(), CV_8UC3 );
	for (int i = 0; i< contours.size(); i++)
	{
		double contour_area = contourArea(contours[i]);
		if (contour_area < 1000) continue;
		Moments m = moments(contours[i]);
		Point center(m.m10 / m.m00, m.m01 / m.m00);
		contour_center.push_back(center);
		blocks.push_back(Block(center.x, center.y, contour_area));
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		circle(equalized, center, 3, Scalar(0, 255, 0), -1, 8, 0);
		drawContours(equalized, contours, i, color, 2, 8, hierarchy, 0, Point());
	}

	//try every combination of 4 blocks and find the ones closest in position and area
	if (blocks.size() >= 4) {
		vector<int> combos;
		vector<int> temp;
		Point center;
		double dist_squared = 1e10;
		int res = gen_comb_norep_lex_init(combos, blocks.size(), 4);
		while (res == GEN_NEXT) {
			//find dist
			double temp_dist_squared = 0.0f;
			for (int i = 0; i < combos.size(); ++i) {
				for (int j = i + 1; j < combos.size(); ++j) {
					Block b1 = blocks[combos[i]];
					Block b2 = blocks[combos[j]];
					temp_dist_squared += (b1.area - b2.area)*(b1.area - b2.area);// + (b1.x-b2.x)*(b1.x-b2.x) + (b1.y-b2.y)*(b1.y-b2.y);
				}
			}
			if (temp_dist_squared < dist_squared) {
				dist_squared = temp_dist_squared;
				//find center
				center.x = center.y = 0;
				for (int i = 0; i < combos.size(); ++i) {
					center.x += blocks[combos[i]].x;
					center.y += blocks[combos[i]].y;
				}
				center.x /= combos.size();
				center.y /= combos.size();
				temp.clear();
				for (int i = 0; i < combos.size(); ++i)
					temp.push_back(combos[i]);
			}
			res = gen_comb_norep_lex_next(combos, blocks.size(), 4);
		}
		for (int i = 0; i < temp.size(); ++i)
			circle(equalized, Point(blocks[temp[i]].x, blocks[temp[i]].y), 3, Scalar(255, 0, 255), -1, 8);
		circle(equalized, center, 3, Scalar(255, 0, 0), -1, 8, 0);
	}
	
	set_img(equalized);
}
