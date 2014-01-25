#include <opencv\highgui.h>
#include <opencv\cv.h>

using namespace cv;

//variables--------------------------------------------------------------------------------------------------------------------------------

bool useGaussianBlur = false;
bool useErode = false;
bool useDilate = false;
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
