#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include "CombinedProcessor.h"

using namespace cv;
CombinedProcessor::CombinedProcessor() : FrameProcessor("Output") {
#ifdef CALIBRATE
	createTrackbar("hbegin", get_window_name(), &hbegin, 179);
	createTrackbar("hend", get_window_name(), &hend, 179);
	createTrackbar("slow", get_window_name(), &slow, 255);
	createTrackbar("shigh", get_window_name(), &shigh, 255);
	createTrackbar("vlow", get_window_name(), &vlow, 255);
	createTrackbar("vhigh", get_window_name(), &vhigh, 255);
#endif
}


void CombinedProcessor::Process(cv::Mat frame) {
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

	red_mask = red_mask1;// | red_mask2;

#ifdef CALIBRATE_WHITE_MASK
	inRange(hsv, Scalar(hbegin, slow, vlow), Scalar(hend, shigh, vhigh), white_mask);//8-79,0-37,196-255
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

	Mat grayscale;
	cvtColor(equalized, grayscale, CV_BGR2GRAY);
	//namedWindow("Grayscale");
	//imshow("Grayscale", grayscale);

	Mat out;
	threshold(grayscale, out, 200, 255, CV_THRESH_BINARY);
	erode(out, out, Mat());
	dilate(out, out, Mat());
	//namedWindow("Threshold");
	//imshow("Threshold", out);

	Mat combined = out & white_mask;
	//namedWindow("Combined");
	//imshow("Combined", combined


	vector<vector<Point> > white_contours;
	vector<Vec4i> white_hierarchy;

	vector<Point> white_contour_center;

	RNG rng(12345);
	/// Find contours
  findContours(white_mask, white_contours, white_hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

   //Draw contours
  //for( int i = 0; i< white_contours.size(); i++ )
  //   {
	 //  double contour_area = contourArea(white_contours[i]);
	 //  if(contour_area < 600 || contour_area > 1200) continue;
	 //  Rect bounding_rect = boundingRect(white_contours[i]);
	 //  double rect_area = bounding_rect.area();
	 //  double diff = abs((contour_area-rect_area)/rect_area);
	 //  if(diff > .4) continue;
	 //  Moments m = moments(white_contours[i]);
	 //  Point center(m.m10/m.m00,m.m01/m.m00);
	 //  white_contour_center.push_back(center);
  //     Scalar color = Scalar(0, 0, 0);
	 //  circle( equalized, center, 3, Scalar(0,255,0), -1, 8, 0 );
  //     drawContours( equalized, white_contours, i, color, 2, 8, white_hierarchy, 0, Point() );
  //   }

  struct Block{
	double x;
	double y;
	double area;
	Block(double x, double y, double area) : x(x), y(y), area(area) {}
  };

  vector<Block> white_blocks;
  /// Draw contours
  //Mat drawing = Mat::zeros( sobelMask.size(), CV_8UC3 );
  for( int i = 0; i< white_contours.size(); i++ )
     {
	   double contour_area = contourArea(white_contours[i]);
	   if(contour_area < 100) continue;
	   Moments m = moments(white_contours[i]);
	   Point center(m.m10/m.m00,m.m01/m.m00);
	   white_contour_center.push_back(center);
	   white_blocks.push_back(Block(center.x, center.y, contour_area));
       Scalar color = Scalar( 255, 255, 255);
	   circle( equalized, center, 3, Scalar(0,255,0), -1, 8, 0 );
       drawContours( equalized, white_contours, i, color, 2, 8, white_hierarchy, 0, Point() );
     }

  //try every combination of 4 blocks and find the ones closest in position and area
  if(white_blocks.size() >= 4) {
	  vector<int> combos;
	  vector<int> temp;
	  Point center;
	  double dist_squared = 1e10;
	  int res = gen_comb_norep_lex_init(combos, white_blocks.size(), 4);
	  while(res == GEN_NEXT) {
		//find dist
		double temp_dist_squared = 0.0f;
		int horizontal =0, vertical =0;
		for(int i = 0; i < combos.size(); ++i) {
			for(int j = i+1; j < combos.size(); ++j) {
				Block b1 = white_blocks[combos[i]];
				Block b2 = white_blocks[combos[j]];
				if(abs(b1.x-b2.x) > 10*abs(b1.y-b2.y)){
					horizontal = 1;
				} else if(abs(b1.y-b2.y) >10*abs(b1.x-b2.x)){
					vertical = 1;
				}
				temp_dist_squared += (b1.area - b2.area)*(b1.area - b2.area);// + (b1.x-b2.x)*(b1.x-b2.x) + (b1.y-b2.y)*(b1.y-b2.y);
			}
		}
		if(temp_dist_squared < dist_squared & horizontal & vertical) {
			dist_squared = temp_dist_squared;
			//find center
			center.x = center.y = 0;
			for(int i = 0; i < combos.size(); ++i) {
				center.x += white_blocks[combos[i]].x;
				center.y += white_blocks[combos[i]].y;
			}
			center.x /= combos.size();
			center.y /= combos.size();
			temp.clear();
			for(int i = 0; i < combos.size(); ++i)
				temp.push_back(combos[i]);
		}
		res = gen_comb_norep_lex_next(combos, white_blocks.size(), 4);
	  }
	  for(int i = 0; i < temp.size(); ++i)
		  circle(equalized, Point(white_blocks[temp[i]].x, white_blocks[temp[i]].y), 3, Scalar(255, 0, 255), -1, 8);
	  circle(equalized, center, 3, Scalar(255,0,0), -1, 8, 0);
  }

	set_img(equalized);
}