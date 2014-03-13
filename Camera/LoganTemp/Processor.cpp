#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

#include "Processor.h"

using namespace cv;

Processor::Processor() : FrameProcessor("Output") {
#ifdef CALIBRATE
	namedWindow("trackbars");
#endif

#ifdef RED_HSV
	createTrackbar("hlow", "trackbars", &hlow, 255);
	createTrackbar("hhigh", "trackbars", &hhigh, 255);
	createTrackbar("slow", "trackbars", &slow, 255);
	createTrackbar("shigh", "trackbars", &shigh, 255);
	createTrackbar("vlow", "trackbars", &vlow, 255);
	createTrackbar("vhigh", "trackbars", &vhigh, 255);
#endif

#ifdef DIST_AREA
	createTrackbar("areaWeight", "trackbars", &areaWeight, 50);
	createTrackbar("distanceWeight", "trackbars", &distanceWeight, 50);
#endif

#ifdef HORIZONTAL_VERTICAL
	createTrackbar("h", "trackbars", &hMaxDiff, 500);
	createTrackbar("v", "trackbars", &vMaxDiff, 500);
#endif
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

	Mat red_mask1, red_mask2, red_mask;

	inRange(hsv, Scalar(hlow, 0, 0), Scalar(hhigh, 255, 255), red_mask1);
	inRange(hsv, Scalar(0, slow, vlow), Scalar(255, shigh, vhigh), red_mask2);
//8-127,0-0,0-131 or 2-158,0-0,0-121

	red_mask = ~red_mask1 & ~red_mask2;
	Mat mask;
	erode(red_mask, mask, Mat());
	dilate(mask, mask, Mat());

#ifdef RED_HSV
	//namedWindow("mask");
	imshow("mask", mask);
#endif

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
		if (contour_area < 100) continue; //filter by area
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
					//if (abs(b1.x - b2.x) > 3 * abs(b1.y - b2.y)){
					//	horizontal++;
					//}
					//else if (abs(b1.y - b2.y) > 3 * abs(b1.x - b2.x)){
					//	vertical++;
					//}
					horizontal = abs(b1.x - b2.x);
					vertical = abs(b1.y - b2.y);
#endif

					temp_dist_squared += distanceWeight*((b1.x - b2.x)*(b1.x - b2.x) + (b1.y - b2.y)*(b1.y - b2.y))*((b1.x - b2.x)*(b1.x - b2.x) + (b1.y - b2.y)*(b1.y - b2.y));
					temp_area_squared += areaWeight*(b1.area - b2.area)*(b1.area - b2.area);//areaweight=1, distweight=2
				}

			}

#ifdef HORIZONTAL_VERTICAL
			//temp_dist_squared *= 1 / 4 * (abs(2 - horizontal) + abs(2 - vertical)) + 1;
			if (horizontal < hMaxDiff || vertical < vMaxDiff) {
#endif
				if (temp_dist_squared < dist_squared && temp_dist_squared < dist_squared) {
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
#ifdef HORIZONTAL_VERTICAL
			}
#endif

			res = gen_comb_norep_lex_next(combos, blocks.size(), 4);
		}
		for (int i = 0; i < temp.size(); ++i)
			circle(equalized, Point(blocks[temp[i]].x, blocks[temp[i]].y), 3, Scalar(255, 0, 255), -1, 8);
		circle(equalized, center, 3, Scalar(255, 0, 0), -1, 8, 0);
		std::cout << center.x * 2 << "," << center.y * 2 <<std::endl; //change based on resize
		set_img(equalized);
		return;
	}
	std::cout << "640,480" << std::endl;
	set_img(equalized);
}
