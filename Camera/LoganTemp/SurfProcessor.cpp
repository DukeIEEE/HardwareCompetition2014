#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include "SurfProcessor.h"

using namespace cv;

Mat reference;

void SurfProcessor::PreProcess(CvCapture* capture){
	while (true){
		Mat frame = cvQueryFrame(capture);
		if(!frame.empty()){
			frame.copyTo(reference);
			imshow("Reference", reference);
		}
		if (waitKey(10) == 'c'){
			break;
		}
	}

}

void SurfProcessor::Process(cv::Mat frame) {
	std::vector<KeyPoint> keypoints_1, keypoints_2;
	std::vector< DMatch > matches;
	std::vector< DMatch > good_matches;

	Mat img_2;
	frame.copyTo(img_2);

	//-- Step 1: Detect the keypoints using SURF Detector
	int minHessian = 400;

	SurfFeatureDetector detector(minHessian);

	detector.detect(reference, keypoints_1);
	detector.detect(img_2, keypoints_2);

	//-- Draw keypoints
	Mat img_keypoints_1; Mat img_keypoints_2;

	drawKeypoints(reference, keypoints_1, img_keypoints_1, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
	drawKeypoints(img_2, keypoints_2, img_keypoints_2, Scalar::all(-1), DrawMatchesFlags::DEFAULT);

	//-- Show detected (drawn) keypoints
	imshow("Keypoints 1", img_keypoints_1);
	imshow("Keypoints 2", img_keypoints_2);
	imshow("Reference", reference);
	//Calculate descriptors
	SurfDescriptorExtractor extractor;

	Mat descriptors_1, descriptors_2;

	extractor.compute(reference, keypoints_1, descriptors_1);
	extractor.compute(img_2, keypoints_2, descriptors_2);

	// Match descriptor vectors using FLANN matching
	FlannBasedMatcher matcher;
	matches.clear();
	matcher.match(descriptors_1, descriptors_2, matches);

	double max_dist = 0; double min_dist = 100;

	//-- Quick calculation of max and min distances between keypoints
	for (int i = 0; i < descriptors_1.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}

	printf("-- Max dist : %f \n", max_dist);
	printf("-- Min dist : %f \n", min_dist);
	good_matches.clear();

	for (int i = 0; i < descriptors_1.rows; i++)
	{
		if (matches[i].distance <= max(2 * min_dist, 0.02))
		{
			good_matches.push_back(matches[i]);
		}
	}

	//-- Draw only "good" matches
	Mat img_matches;
	drawMatches(reference, keypoints_1, img_2, keypoints_2,
		good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	//-- Show detected matches
	imshow("Good Matches", img_matches);

	for (int i = 0; i < (int) good_matches.size(); i++)
	{
		printf("-- Good Match [%d] Keypoint 1: %d  -- Keypoint 2: %d  \n", i, good_matches[i].queryIdx, good_matches[i].trainIdx);
	}

	//Mat grayscale;
	//cvtColor(frame, grayscale, CV_BGR2GRAY);
	//Mat sharpened;
	//double duration;
	//duration = static_cast<double>(getTickCount());
	////colorReduce(frame);
	//sharpen(grayscale, sharpened);
	//duration = static_cast<double>(getTickCount() - duration);
	//duration /= getTickFrequency(); 
	//cout << duration << "\n";
	//imshow("Camera", sharpened);
	set_img(reference);
}