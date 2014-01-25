#include <opencv\highgui.h>
#include <opencv\cv.h>

using namespace cv;

void trackFilteredObject(int &x, int &y, Mat thresholdedImage, Mat &cameraRBGImage);
