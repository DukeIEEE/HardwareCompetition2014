#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

#include "Processor.h"

using namespace cv;

Processor::Processor() : FrameProcessor("Output") {
	createTrackbar("areaWeight", get_window_name(), &areaWeight, 50);
	createTrackbar("distanceWeight", get_window_name(), &distanceWeight, 50);
}

struct Block{
  double x;
  double y;
  double area;
  Block(double x, double y, double area) : x(x), y(y), area(area) {}
};

void Processor::Process(cv::Mat frame) {
	//flip(frame, frame, 1); //flip frame across y-axis
	Mat equalized = frame.clone();// equalizeIntensity(frame);
	Mat hsv; //hold hsv components
	cvtColor(equalized, hsv, CV_BGR2HSV);

	Mat mask_A, mask_B, mask_sam;

	inRange(hsv, Scalar(8, 0, 0), Scalar(127, 255, 255), mask_A);
	inRange(hsv, Scalar(0, 0, 0), Scalar(255, 131, 255), mask_B);
	mask_sam = ~mask_A & ~mask_B;

	Mat mask;
	erode(mask_sam, mask, Mat());
	dilate(mask, mask, Mat());

	//imshow("Sam Mask", mask);

	//--------------------------------------------------------------------------------------------------------------------------------Contours

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	RNG rng(12345);
	/// Find contours
	findContours(mask, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	vector<Block> blocks;
	// Calculate contour centroids
	for (int i = 0; i< contours.size(); i++)
	{
		double contour_area = contourArea(contours[i]);
		if (contour_area < 1000) continue; //filter by area
		Moments m = moments(contours[i]);
		Point center(m.m10 / m.m00, m.m01 / m.m00);
		blocks.push_back(Block(center.x, center.y, contour_area));
    
    //draw contour and centroid
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		circle(equalized, center, 3, Scalar(0, 255, 0), -1, 8, 0);
		drawContours(equalized, contours, i, color, 2, 8, hierarchy, 0, Point());
	}

	//try every combination of 4 blocks and find the ones closest in position and area
	if (blocks.size() >= 4) {
		vector<int> combos;
		vector<int> temp;
		Point center;
		double dist_squared = INFINITY;
		double area_squared = INFINITY;
		int res = gen_comb_norep_lex_init(combos, blocks.size(), 4);
		while (res == GEN_NEXT) {
			//find dist
			double temp_dist_squared = 0.0f;
			double temp_area_squared = 0.0f;
			int horizontal = 0;
			int vertical = 0;
			for (int i = 0; i < combos.size(); ++i) {
				for (int j = i + 1; j < combos.size(); ++j) {
					Block b1 = blocks[combos[i]];
					Block b2 = blocks[combos[j]];
#ifdef HORIZONTAL_VERTICAL
					if (abs(b1.x - b2.x) > 5 * abs(b1.y - b2.y)){
						horizontal++;
					}
					else if (abs(b1.y - b2.y) >5 * abs(b1.x - b2.x)){
						vertical++;
					}
#endif
					temp_dist_squared += distanceWeight*((b1.x - b2.x)*(b1.x - b2.x) + (b1.y - b2.y)*(b1.y - b2.y))*((b1.x - b2.x)*(b1.x - b2.x) + (b1.y - b2.y)*(b1.y - b2.y)) + areaWeight*(b1.area - b2.area)*(b1.area - b2.area);//areaweight=1, distweight=2
				}

			}
#ifdef HORIZONTAL_VERTICAL
			temp_dist_squared *= 1 / 4 * (abs(2 - horizontal) + abs(2 - vertical)) + 1;
#endif
			if (temp_dist_squared < dist_squared) {
				dist_squared = temp_dist_squared;
				area_squared = temp_area_squared;
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
		std::cout << center.x << "," << center.y <<std::endl;
		set_img(equalized);
		return;
	}
	std::cout << "0,0" << std::endl;
	set_img(equalized);
}
