#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

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
	inRange(hsv, Scalar(0, 21, 21), Scalar(16, 235, 237), red_mask1);
	inRange(hsv, Scalar(165, 21, 21), Scalar(179,235, 237), red_mask2);
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
	//imshow("White Mask", white_mask);
	//imshow("Red Mask", red_mask);
	//imshow("Mask", mask);
	
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
	sobelMask = .6 * sobelR + .6 * sobelG + .6 * sobelB + .6 * sobelSat;
	//imshow("SobelR", sobelR);
	//imshow("SobelG", sobelG);
	//imshow("SobelB", sobelB);
	//imshow("SobelSat", sobelSat);

	threshold(sobelMask, sobelMask, 180, 255, CV_THRESH_BINARY);
	imshow("SobelMask", sobelMask);
	set_img(equalized);
}