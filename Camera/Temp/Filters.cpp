#define _CRT_SECURE_NO_DEPRECATE

#include <opencv\highgui.h>
#include <opencv\cv.h>
#include "Filters.h"

using namespace cv;

//variables--------------------------------------------------------------------------------------------------------------------------------

bool useGaussianBlur = false;
bool useSobel = false;
bool useEqualize = false;
bool useNormalize = false;

//functions--------------------------------------------------------------------------------------------------------------------------------

void morphOps(Mat &thresh) {
	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle

	Mat erodeElement = getStructuringElement(MORPH_RECT, Size(3, 3));
	//dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement(MORPH_RECT, Size(8, 8));

	erode(thresh, thresh, erodeElement);
	erode(thresh, thresh, erodeElement);

	dilate(thresh, thresh, dilateElement);
	dilate(thresh, thresh, dilateElement);
}

void updateRedHSV(Mat &img) {
	//dynamically determine HSV ranges closest to red to track
}

Mat filterImage(Mat &img) {
	if (useGaussianBlur) {
		GaussianBlur(img, img, Size(6, 6), 2.0);
	}
	if (useSobel) {
		Sobel(img, img, 1.0, 1, 1);
	}
	if (useEqualize) {
		equalizeHist(img, img);
	}
	if (useNormalize) {
		normalize(img, img, 1.0, 1);
	}
}