#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Utilities.h"

using namespace cv;

// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
double angle(cv::Point&pt1, cv::Point&pt2, cv::Point&pt0)
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

double mean(std::vector<double>& vec) {
	double ret = 0.0;
	for(int i = 0; i < vec.size(); ++i)
		ret += vec[i];
	return ret/vec.size();
}

//computes the variance of vector of doubles
double variance(std::vector<double>& vec) {
	double mean = 0.0;
	for(int i = 0; i < vec.size(); ++i)
		mean += vec[i];
	mean /= vec.size();
	double ret = 0.0;
	for(int i = 0; i < vec.size(); ++i)
		ret += (vec[i] - mean)*(vec[i] - mean);
	return ret;
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
	//threshold(out, out, thresh, 255, CV_THRESH_BINARY_INV);
}


int gen_comb_norep_lex_init(vector<int>& vec, int n, int k) {
	int j; //index

	//test for special cases
	if(k > n)
	 return(GEN_ERROR);

	if(k == 0)
	 return(GEN_EMPTY);

	//initialize: vector[0, ..., k - 1] are 0, ..., k - 1
	for(j = 0; j < k; j++)
	 vec.push_back(j);

	return(GEN_NEXT);
}

int gen_comb_norep_lex_next(vector<int>& vec, int n, int k) {
	int j; //index

	//easy case, increase rightmost element
	if(vec[k - 1] < n - 1)	{
		vec[k - 1]++;
		return(GEN_NEXT);
	}

	//find rightmost element to increase
	for(j = k - 2; j >= 0; j--)
		if(vec[j] < n - k + j)
			break;

	//terminate if vector[0] == n - k
	if(j < 0)
		return(GEN_TERM);

	//increase
	vec[j]++;

	//set right-hand elements
	while(j < k - 1) {
		vec[j + 1] = vec[j] + 1;
		j++;
	}

	return(GEN_NEXT);
}
