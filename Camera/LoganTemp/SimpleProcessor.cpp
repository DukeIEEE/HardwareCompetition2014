#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include "SimpleProcessor.h"

using namespace cv;

SimpleProcessor::SimpleProcessor() : FrameProcessor("Output") {
#ifdef CALIBRATE
	createTrackbar("hbegin", get_window_name(), &hbegin, 179);
	createTrackbar("hend", get_window_name(), &hend, 179);
	createTrackbar("slow", get_window_name(), &slow, 255);
	createTrackbar("shigh", get_window_name(), &shigh, 255);
	createTrackbar("vlow", get_window_name(), &vlow, 255);
	createTrackbar("vhigh", get_window_name(), &vhigh, 255);
#endif
}

void SimpleProcessor::Process(cv::Mat frame) {
	flip(frame, frame, 1); //flip frame across y-axis


	Mat equalized = equalizeIntensity(frame);

	Mat hsv; //hold hsv components
	cvtColor(equalized, hsv, CV_BGR2HSV);

	//find red objects
	Mat red_mask1, red_mask2, red_mask, white_mask;
#ifdef CALIBRATE_RED_MASK
	inRange(hsv, Scalar(0, slow, vlow), Scalar(hbegin, shigh, vhigh), red_mask1);
	inRange(hsv, Scalar(hend,slow,vlow), Scalar(179,shigh,vhigh), red_mask2);
#else
	inRange(hsv, Scalar(0, 26, 77), Scalar(14,141,204), red_mask1);
	inRange(hsv, Scalar(159, 26, 77), Scalar(179,141,204), red_mask2);
#endif

	red_mask = red_mask1 | red_mask2;

#ifdef CALIBRATE_WHITE_MASK
	inRange(hsv, Scalar(hbegin, slow, vlow), Scalar(hend, shigh, vhigh), white_mask);
#else
	inRange(hsv, Scalar(83,5,186), Scalar(148,127,255), white_mask);
#endif

	//Mat mask = red_mask ^ white_mask;

	//namedWindow("White Mask");
	//namedWindow("Red Mask");
	//namedWindow("Mask");

	blur(white_mask, white_mask, Size(5,5));
	threshold(white_mask, white_mask, 128, 255, CV_THRESH_BINARY);
	imshow("White Mask", white_mask);
	dilate(red_mask, red_mask, Mat());
	imshow("Red Mask", red_mask);
	//imshow("Mask", white_mask);
	
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
	sobelMask = .6 * sobelR + .6 * sobelG + .6 * sobelB ;//+ .6 * sobelSat;
	//imshow("SobelR", sobelR);
	//imshow("SobelG", sobelG);
	//imshow("SobelB", sobelB);
	//imshow("SobelSat", sobelSat);

	//threshold(sobelMask, sobelMask, 180, 255, CV_THRESH_BINARY);
	imshow("SobelMask", sobelMask);
	

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	vector<Point> contour_center;

	RNG rng(12345);
	/// Find contours
  findContours( red_mask, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

  struct Block{
	double x;
	double y;
	double area;
	Block(double x, double y, double area) : x(x), y(y), area(area) {}
  };

  vector<Block> blocks;
  /// Draw contours
  //Mat drawing = Mat::zeros( sobelMask.size(), CV_8UC3 );
  for( int i = 0; i< contours.size(); i++ )
     {
	   double contour_area = contourArea(contours[i]);
	   if(contour_area < 1000) continue;
	   Moments m = moments(contours[i]);
	   Point center(m.m10/m.m00,m.m01/m.m00);
	   contour_center.push_back(center);
	   blocks.push_back(Block(center.x, center.y, contour_area));
       Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
	   circle( equalized, center, 3, Scalar(0,255,0), -1, 8, 0 );
       drawContours( equalized, contours, i, color, 2, 8, hierarchy, 0, Point() );
     }

  //try every combination of 4 blocks and find the ones closest in position and area
  if(blocks.size() >= 4) {
	  vector<int> combos;
	  vector<int> temp;
	  Point center;
	  double dist_squared = 1e10;
	  int res = gen_comb_norep_lex_init(combos, blocks.size(), 4);
	  while(res == GEN_NEXT) {
		//find dist
		double temp_dist_squared = 0.0f;
		for(int i = 0; i < combos.size(); ++i) {
			for(int j = i+1; j < combos.size(); ++j) {
				Block b1 = blocks[combos[i]];
				Block b2 = blocks[combos[j]];
				temp_dist_squared += (b1.area - b2.area)*(b1.area - b2.area);// + (b1.x-b2.x)*(b1.x-b2.x) + (b1.y-b2.y)*(b1.y-b2.y);
			}
		}
		if(temp_dist_squared < dist_squared) {
			dist_squared = temp_dist_squared;
			//find center
			center.x = center.y = 0;
			for(int i = 0; i < combos.size(); ++i) {
				center.x += blocks[combos[i]].x;
				center.y += blocks[combos[i]].y;
			}
			center.x /= combos.size();
			center.y /= combos.size();
			temp.clear();
			for(int i = 0; i < combos.size(); ++i)
				temp.push_back(combos[i]);
		}
		res = gen_comb_norep_lex_next(combos, blocks.size(), 4);
	  }
	  for(int i = 0; i < temp.size(); ++i)
		  circle(equalized, Point(blocks[temp[i]].x, blocks[temp[i]].y), 3, Scalar(255, 0, 255), -1, 8);
	  circle(equalized, center, 3, Scalar(255,0,0), -1, 8, 0);
  }

  /*if(contour_center.size() > 0) {
	  //find contour center
	  Point center(0,0);
	  for(vector<Point>::const_iterator iter = contour_center.cbegin(); iter != contour_center.cend(); ++iter)
		center += *iter;
	  center.x /= contour_center.size();
	  center.y /= contour_center.size();*/
	  //circle(equalized, center, 3, Scalar(255,0,0), -1, 8, 0);
  //}

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