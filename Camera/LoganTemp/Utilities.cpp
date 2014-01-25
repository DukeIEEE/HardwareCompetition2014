#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Utilities.h"

using namespace cv;

/* *
This function equalizes intensity for a color image (RGB)
*/
Mat equalizeIntensity(const Mat& inputImage)
{
    if(inputImage.channels() >= 3)
    {
        Mat ycrcb;

		//convert to YCrCb format since it is a digital image
        cvtColor(inputImage,ycrcb,CV_BGR2YCrCb);

        vector<Mat> channels;
		//split the channels so that we can grab Y plane
        split(ycrcb,channels);

		//equalize Y plane
        equalizeHist(channels[0], channels[0]);

        Mat result;
		//merge everything backtogether
        merge(channels,ycrcb);

		//convert back to RGB
        cvtColor(ycrcb,result,CV_YCrCb2BGR);

        return result;
    }
    return Mat();
}

//applies sobel edge detection and returns a mask based on the threshold
void generateSobelMask(Mat in, Mat & out, int thresh) {
	Mat sobelX, sobelY;
	// Compute norm of Sobel
	Sobel(in,sobelX,CV_16S,1,0);
	Sobel(in,sobelY,CV_16S,0,1);
	
	Mat sobel;
	//compute the L1 norm
	sobel = abs(sobelX) + abs(sobelY);

	// Find Sobel max value
	double sobmin, sobmax;
	minMaxLoc(sobel,&sobmin,&sobmax);
	// Conversion to 8-bit image
	sobel.convertTo(out,CV_8U,-255./sobmax,255);

	//threshold out edges
	threshold(out, out, thresh, 255, CV_THRESH_BINARY_INV);
}
