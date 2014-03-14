#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "NewProcessor.h"

using namespace cv;
using namespace std;

//#define SHOW_IMAGES
#define SAVE_IMAGES
//#define COMPUTER_MODE

#if defined(SAVE_IMAGES) | defined(SHOW_IMAGES)
#define COMPUTER_MODE
#endif

#ifndef SHOW_IMAGES
#undef imshow
#define imshow(x,y) ;
#endif

#ifdef SAVE_IMAGES
#include <fstream>
#endif

//define the constants
const int NewProcessor::MIN_BLOCK_AREA = 100;
const double NewProcessor::areaWeight = 1;
const double NewProcessor::distanceWeight = 2;

void NewProcessor::GenerateMask(Mat& img, Mat& mask) {
	//split image for sobel processing
	vector<Mat> planes;
	split(img, planes);
	Mat sobelR, sobelG, sobelB, sobelSat, sobelMask;
	//calculate sobel for red, green, and blue planes
	generateSobelMask(planes[0], sobelB, 230);
	generateSobelMask(planes[1], sobelG, 230);
	generateSobelMask(planes[2], sobelR, 230);

	sobelMask = .34*sobelB + .34*sobelG + .34*sobelR;

	imshow("sobel", sobelMask);
#ifdef SAVE_IMAGES
	imwrite(std::string("output/sobel/") + filename, sobelMask);
#endif

	//threshold to bw image
	//threshold(sobelMask, sobelThresh, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	threshold(sobelMask, sobelMask, 250, 255, CV_THRESH_BINARY_INV);
	//imshow("sobel_thresholded", sobelMask);

	//get rid of salt and pepper noise
	medianBlur(sobelMask, sobelMask,5);

	imshow("sobel_thresholded_medianBlurred", sobelMask);
#ifdef SAVE_IMAGES
	imwrite(std::string("output/sobel2/") + filename, sobelMask);
#endif

	//run color detection filters
	Mat hsv; //hold hsv components
	cvtColor(img, hsv, CV_BGR2HSV);

	Mat mask_A, mask_B;
	inRange(hsv, Scalar(/*8*/ 16, 0, 0), Scalar(125, 255, 255), mask_A);
	inRange(hsv, Scalar(0, 0, 0), Scalar(127, 100, 255), mask_B);
	mask = ~mask_A & ~mask_B;
	imshow("mask", mask);
#ifdef SAVE_IMAGES
	imwrite(std::string("output/mask/") + filename, mask);
#endif

	mask &= ~sobelMask;
	imshow("mask and sobel", mask);

#ifdef SAVE_IMAGES
	imwrite(std::string("output/mask_final/") + filename, mask);
#endif
}

void NewProcessor::GenerateBlocks(vector<Block>& blocks, Mat& mask, Mat& img) {
	static RNG rng(12345);

	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;

	/// Find contours
	findContours(mask, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	for (unsigned int i = 0; i < contours.size(); i++) {
		double contour_area = contourArea(contours[i]);
		if (contour_area < MIN_BLOCK_AREA) continue; //filter by area

		//find center
		Moments m = moments(contours[i]);
		Point center(m.m10 / m.m00, m.m01 / m.m00);
		blocks.push_back(Block(center.x, center.y, contour_area));
    
#ifdef COMPUTER_MODE
		//draw contour and centroid
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		circle(img, center, 3, Scalar(0, 255, 0), -1, 8, 0);
		drawContours(img, contours, i, color, 2, 8, hierarchy, 0, Point());
#endif
	}
}

#define SQ(x) ((x)*(x))
#ifdef SAVE_IMAGES
#define LOG(x) file << x;
//#define LOG(x) ;
#else
#define LOG(x) ;
#endif
void NewProcessor::ProcessBlocks(vector<Block>& blocks, Mat& img) {
	Point best_center(0,0);
#ifdef SAVE_IMAGES
	ofstream file;
	file.open(std::string("output/") + filename + std::string(".txt"));
#endif
	if(blocks.size() >= 4) {
		vector<int> combo; //the combination

		vector<int> best_combo; //save the best ones for now
		double best_combo_value = 1e20; //infinity for all intents and purposes
		LOG("begin loop\n");
		int res = gen_comb_norep_lex_init(combo, blocks.size(), 4);
		while (res == GEN_NEXT) {
			LOG("iter\n");
			//find dist
			double combo_value = 0.0;

			//find center and mean area
			Point center(0,0);
			double mean_area = 0.0f;
			for (int i = 0; i < combo.size(); ++i) {
				center.x += blocks[combo[i]].x;
				center.y += blocks[combo[i]].y;
				mean_area += blocks[combo[i]].area;
			}
			center.x /= combo.size();
			center.y /= combo.size();
			mean_area /= combo.size();
			LOG("center: "); LOG(center.x); LOG(","); LOG(center.y); LOG("; "); LOG(mean_area); LOG("\n");

			//calculate variance in x, y, and area
			double var_x = 0, var_y = 0, var_area = 0;
			for(int i = 0; i < combo.size(); ++i) {
				var_x += (blocks[combo[i]].x - center.x)*(blocks[combo[i]].x - center.x);
				var_y += (blocks[combo[i]].y - center.y)*(blocks[combo[i]].y - center.y);
				var_area += (blocks[combo[i]].area - mean_area)*(blocks[combo[i]].area - mean_area);
			}
			LOG("dist: "); LOG((var_x + var_y)); LOG("; area: "); LOG(var_area/mean_area); LOG("\n");
			combo_value = distanceWeight*(var_x + var_y) + areaWeight*var_area/mean_area;

			//go through all permutations and make sure it's a square object
			vector<int> combo_temp(combo);
			double combo_best_norm = 1e20;
			do {
				double combo_norm = 0.0;
				Block& top_left = blocks[combo_temp[0]];
				Block& bottom_left = blocks[combo_temp[1]];
				Block& top_right = blocks[combo_temp[2]];
				Block& bottom_right = blocks[combo_temp[3]];

				combo_norm += SQ(top_left.x - bottom_left.x) + SQ(top_left.y - top_right.y) +
								SQ(bottom_left.y - bottom_right.y) + SQ(top_right.x - bottom_right.x);

				vector<double> dist;
				dist.push_back(sqrt(top_left.distance2(bottom_left)));
				dist.push_back(sqrt(top_left.distance2(top_right)));
				dist.push_back(sqrt(bottom_right.distance2(bottom_left)));
				dist.push_back(sqrt(bottom_right.distance2(top_right)));
				combo_norm += variance(dist);

				if(combo_norm < combo_best_norm)
					combo_best_norm = combo_norm;
			} while(next_permutation(combo_temp.begin(), combo_temp.end()));

			LOG(combo_best_norm); LOG("\n");
			combo_value += combo_best_norm;
			combo_value /= mean_area;
			LOG(combo_value); LOG("\n");

			//see if combo is actually better
			if (combo_value < best_combo_value) {
				best_combo_value = combo_value;
				best_center = center;

				//copy into new vector
				best_combo.clear();
				for (int i = 0; i < combo.size(); ++i)
					best_combo.push_back(combo[i]);
			}
			res = gen_comb_norep_lex_next(combo, blocks.size(), 4);
		}
		LOG("best combo value: "); LOG(best_combo_value); LOG("\n");
#ifdef COMPUTER_MODE
		for (int i = 0; i < best_combo.size(); ++i)
			circle(img, Point(blocks[best_combo[i]].x, blocks[best_combo[i]].y), 5, Scalar(255, 0, 255), -1, 8);
		circle(img, best_center, 5, Scalar(255, 0, 0), -1, 8, 0);
#endif
	}
#ifdef SAVE_IMAGES
	file.close();
#endif
	std::cout << best_center.x << " " << best_center.y << std::endl;
}
#undef SQ

void NewProcessor::Process(Mat img) {
	Mat mask;
	vector<Block> blocks;

	GenerateMask(img, mask);
	GenerateBlocks(blocks, mask, img);
	ProcessBlocks(blocks, img);

	set_img(img);
}