#define _CRT_SECURE_NO_DEPRECATE

//Based on objectTrackingTutorial.cpp by Kyle Hounslow, 2013

//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software")
//, to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
//and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

//The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

#include <sstream>
#include <string>
#include <iostream>
#include <windows.h>
#include <opencv\highgui.h>
#include <opencv\cv.h>
#include "Filters.h"
#include "ObjectTracking.h"

using namespace cv;

//variables--------------------------------------------------------------------------------------------------------------------------------

//default capture width and height
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;

//initial min and max HSVImage filter values
int H_MIN = 0; //red: 0-20, 200-255
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;

//names that will appear at the top of each window
const string windowName = "Original Image";
const string windowName1 = "HSVImage Image";
const string windowName2 = "Thresholded Image";
const string windowName3 = "After Morphological Operations";
const string trackbarWindowName = "Trackbars";

bool isFirstRun = true;

int halfScreenWidth = GetSystemMetrics(SM_CXSCREEN) / 2;
int halfScreenHeight = GetSystemMetrics(SM_CYSCREEN) / 2;

Mat cameraImage;
Mat HSVImage;
Mat filteredImage;
Mat thresholdedImage;

bool useTrackFilteredObject = false;
bool useMorphOps = false;

//functions--------------------------------------------------------------------------------------------------------------------------------

void on_trackbar(int, void*) { //This function gets called whenever a trackbar position is changed.
}

void createHSVTrackbars() {
	namedWindow(trackbarWindowName, 0); //create window for trackbars

	char TrackbarName[50]; //create memory to store trackbar name on window
	sprintf(TrackbarName, "H_MIN", H_MIN);
	sprintf(TrackbarName, "H_MAX", H_MAX);
	sprintf(TrackbarName, "S_MIN", S_MIN);
	sprintf(TrackbarName, "S_MAX", S_MAX);
	sprintf(TrackbarName, "V_MIN", V_MIN);
	sprintf(TrackbarName, "V_MAX", V_MAX);
	
	createTrackbar("H_MIN", trackbarWindowName, &H_MIN, H_MAX, on_trackbar); //create trackbars and insert them into window
	createTrackbar("H_MAX", trackbarWindowName, &H_MAX, H_MAX, on_trackbar);
	createTrackbar("S_MIN", trackbarWindowName, &S_MIN, S_MAX, on_trackbar);
	createTrackbar("S_MAX", trackbarWindowName, &S_MAX, S_MAX, on_trackbar);
	createTrackbar("V_MIN", trackbarWindowName, &V_MIN, V_MAX, on_trackbar);
	createTrackbar("V_MAX", trackbarWindowName, &V_MAX, V_MAX, on_trackbar);
}

//main()--------------------------------------------------------------------------------------------------------------------------------

int main() {
	int x = 0, y = 0; //x and y values for the location of the object

	createHSVTrackbars(); //create slider bars for HSVImage filtering

	VideoCapture capture; //video capture object to acquire webcam feed
	capture.open(0); //open capture object at location zero (default location for webcam)
	capture.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH); //set height and width of capture frame
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
	
	while (1) {
		capture.read(cameraImage); //store image to matrix
		
		cvtColor(cameraImage, HSVImage, COLOR_BGR2HSV); //convert frame from BGR to HSVImage colorspace

	//dynamically determine HSV ranges closest to red to track
	//filteredImage = findRed(HSVImage);
	//GaussianBlur(filteredImage, filteredImage, Size(6, 6), 2.0);
	//Sobel(HSVImage, filteredImage, 1.0, 1, 1);

		//filter HSVImage image between values and store filtered image to thresholdedImage matrix
		inRange(HSVImage, Scalar(H_MIN, S_MIN, V_MIN), Scalar(H_MAX, S_MAX, V_MAX), thresholdedImage);

		//perform morphological operations on thresholded image to eliminate noise and emphasize the filtered object(s)
		if (useMorphOps) {
			morphOps(thresholdedImage);
		}
		//pass in thresholded frame to our object tracking function
		//This function will return the x and y coordinates of the filtered object.
		if (useTrackFilteredObject) {
			trackFilteredObject(x, y, thresholdedImage, cameraImage);
		}

	//vector redCenters = centers of red spots
	//vector redSpots, whiteSpots
	//vector likelyRedCenters = redCenters of redSpots that border white spots
	//if likelyRedCenters.size()>=4, 

		imshow(windowName, cameraImage); //show frames
		imshow(windowName1, HSVImage);
		imshow(windowName2, thresholdedImage);
		if (isFirstRun) {
			isFirstRun = false;
			moveWindow(windowName, 0, 0);
			moveWindow(windowName1, halfScreenWidth, 0);
			moveWindow(windowName2, 0, halfScreenHeight);
			moveWindow(windowName3, halfScreenWidth, halfScreenHeight);
		}
		
		int key = waitKey(100); //image will not appear without this waitKey() command
		if (key == 27) { //esc
			break;
		}
		else if (key == 13) { //enter
			imwrite("C:\\Users\\User\\Desktop\\image.jpg", cameraImage);
		}
	}
	
	return 0;
}
