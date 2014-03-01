#define _CRT_SECURE_NO_DEPRECATE

//objectTrackingTutorial.cpp

//Written by  Kyle Hounslow 2013

//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software")
//, to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
//and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

//The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
//IN THE SOFTWARE.

#include <sstream>
#include <string>
#include <iostream>
#include <opencv\highgui.h>
#include <opencv\cv.h>

using namespace cv;

#include <windows.h>
bool firstrun = 1;
int x = 0;
int y = 0;

//initial min and max HSV filter values.
//these will be changed using trackbars
int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;
//default capture width and height
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS = 50;
//minimum and maximum object area
const int MIN_OBJECT_AREA = 20 * 20;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH / 1.5;
//names that will appear at the top of each window
const string windowName = "Original Image";
const string windowName1 = "HSV Image";
const string windowName2 = "Thresholded Image";
const string windowName3 = "After Morphological Operations";
const string trackbarWindowName = "Trackbars";


bool isFirstRun;

Scalar redLow;
Scalar redHigh;
Scalar whiteLow;
Scalar whiteHigh;

Vec3b averageRedHSV;
Vec3b redTolerance;
Vec3b averageWhiteHSV;
Vec3b whiteTolerance;

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
	return ((vec - upper) == Vec3b(0, 0, 0)) && ((lower - vec) == Vec3b(0, 0, 0));
}

Vec3b getAverage(Mat img, Vec3b previousAverage, Vec3b tolerance) {
	int rows = img.rows;
	int cols = img.cols;

	Vec3b sum = Vec3b(0, 0, 0);
	int numPixels = 0;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			Vec3b pixelVals = img.at<Vec3b>(i, j);
			if (isWithin(pixelVals, previousAverage, tolerance)) {
				sum += img.at<Vec3b>(i, j);
				numPixels++;
			}
		}
	}

	if (numPixels < rows * cols / 10) { //at least 10% of pixels
		return previousAverage;
	}
	return sum / numPixels;
}


void on_trackbar(int, void*)
{//This function gets called whenever a
	// trackbar position is changed
}

void createTrackbars(){
	//create window for trackbars
	namedWindow(trackbarWindowName, 0);
	//create memory to store trackbar name on window
	char TrackbarName[50];
	sprintf(TrackbarName, "H_MIN", H_MIN);
	sprintf(TrackbarName, "H_MAX", H_MAX);
	sprintf(TrackbarName, "S_MIN", S_MIN);
	sprintf(TrackbarName, "S_MAX", S_MAX);
	sprintf(TrackbarName, "V_MIN", V_MIN);
	sprintf(TrackbarName, "V_MAX", V_MAX);
	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH), 
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->      
	createTrackbar("H_MIN", trackbarWindowName, &H_MIN, H_MAX, on_trackbar);
	createTrackbar("H_MAX", trackbarWindowName, &H_MAX, H_MAX, on_trackbar);
	createTrackbar("S_MIN", trackbarWindowName, &S_MIN, S_MAX, on_trackbar);
	createTrackbar("S_MAX", trackbarWindowName, &S_MAX, S_MAX, on_trackbar);
	createTrackbar("V_MIN", trackbarWindowName, &V_MIN, V_MAX, on_trackbar);
	createTrackbar("V_MAX", trackbarWindowName, &V_MAX, V_MAX, on_trackbar);
}

int main0() {
	//Matrix to store each frame of the webcam feed
	Mat cameraFeed;
	//matrix storage for HSV image
	Mat HSV;
	//matrix storage for binary threshold image
	Mat threshold;
	//x and y values for the location of the object
	//int x=0, y=0;
	//create slider bars for HSV filtering
	createTrackbars();
	//video capture object to acquire webcam feed
	VideoCapture capture;
	//open capture object at location zero (default location for webcam)
	capture.open(0);
	//set height and width of capture frame
	capture.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
	//start an infinite loop where webcam feed is copied to cameraFeed matrix
	//all of our operations will be performed within this loop

	int r1, r2, b1, b2, g1, g2;
	Vec3b avg, t;
	Mat mask1, mask2, mask3, mask4;

	while (1){
		//filter HSV image between values and store filtered image to
		//threshold matrix
		//inRange(HSV, Scalar(H_MIN, S_MIN, V_MIN), Scalar(H_MAX, S_MAX, V_MAX), threshold);waitKey(100);

		if (firstrun) {
			firstrun = 0;
			moveWindow(windowName, 0, 0);
			moveWindow(windowName1, x, 0);
			moveWindow(windowName2, 0, y);
			moveWindow(windowName3, x, y);

			while (true) {
				capture.read(cameraFeed);

				cvtColor(cameraFeed, HSV, COLOR_BGR2HSV);
				inRange(HSV, Scalar(H_MIN, S_MIN, V_MIN), Scalar(H_MAX, S_MAX, V_MAX), threshold);
				imshow(windowName2, threshold);
				if ((char)waitKey(100) == ' ') {
					b1 = H_MIN;
					b2 = H_MAX;
					g1 = S_MIN;
					g2 = S_MAX;
					r1 = V_MIN;
					r2 = V_MAX;
					avg = toVec3b(Scalar((b1 + b2) / 2, (g1 + g2) / 2, (r1 + r2) / 2));
					t = toVec3b(Scalar((b2 - b1) / 2, (g2 - g1) / 2, (r2 - r1) / 2));
					break;
				}
			}
		}
		else {
			capture.read(cameraFeed);

			cvtColor(cameraFeed, HSV, COLOR_BGR2HSV);
			//avg = getAverageHSV(cameraFeed, avg, t);
			inRange(HSV, Scalar(8, 0, 0), Scalar(127, 255, 255), mask1);
			inRange(HSV, Scalar(0, 0, 64), Scalar(255, 131, 255), mask2);
			inRange(cameraFeed, Scalar(H_MIN, S_MIN, V_MIN), Scalar(H_MAX, S_MAX, V_MAX), mask3);
			//inRange(cameraFeed, Scalar(0, 0, 100), Scalar(255, 255, 255), mask4);
			threshold = ~mask1 & ~mask2 & ~mask3;
			erode(threshold, threshold, Mat());
			dilate(threshold, threshold, Mat());
			imshow(windowName2, threshold);
		}

		int key = waitKey(100);
		if (key == 27) {
			break;
		}
		else if (key == 13) {
			imwrite("C:\\Users\\User\\Desktop\\image.jpg", cameraFeed);
		}
	}
	return 0;
}
