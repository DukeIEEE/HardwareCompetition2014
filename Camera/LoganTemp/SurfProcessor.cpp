#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"

#include <iostream>
#include "SurfProcessor.h"

using namespace cv;

Mat reference, object;

void SurfProcessor::PreProcess(CvCapture* capture){
//	//while (true){
//		//Mat frame = cvQueryFrame(capture);
//		Mat frame = imread( "hackduke_tshirt.png", CV_LOAD_IMAGE_GRAYSCALE );
//		if(!frame.empty()){
//			frame.copyTo(reference);
//			frame.copyTo(object);
//			imshow("Reference", reference);
//		} else {
//			std::cout << "No image found";
//		}
//	//	if (waitKey(10) == 'c'){
//	//		break;
//	//	}
//	//}
//
}

void SurfProcessor::Process(cv::Mat frame) {
	std::vector<KeyPoint> keypoints_1, keypoints_2;
	std::vector< DMatch > matches;
	std::vector< DMatch > good_matches;

	Mat image = imread( "hackduke_tshirt_logo.png", CV_LOAD_IMAGE_GRAYSCALE );
	if(!image.empty()){
		image.copyTo(reference);
		image.copyTo(object);
		imshow("Reference", reference);
	} else {
		std::cout << "No image found";
	}

	Mat img_2;
	frame.copyTo(img_2);

	//-- Step 1: Detect the keypoints using SURF Detector
	int minHessian = 1000;

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


	std::vector<Point2f> obj;
    std::vector<Point2f> scene;
    std::vector<Point2f> scene_corners(4);

	std::vector<Point2f> obj_corners(4);

    //Get the corners from the object
    obj_corners[0] = cvPoint(0,0);
    obj_corners[1] = cvPoint( reference.cols, 0 );
    obj_corners[2] = cvPoint( reference.cols, reference.rows );
    obj_corners[3] = cvPoint( 0, reference.rows );

	if (good_matches.size() >= 4)
        {
            for( int i = 0; i < good_matches.size(); i++ )
            {
                //Get the keypoints from the good matches
                obj.push_back( keypoints_1[ good_matches[i].queryIdx ].pt );
                scene.push_back( keypoints_2[ good_matches[i].trainIdx ].pt );
            }

            Mat H = findHomography( obj, scene, CV_RANSAC );

            perspectiveTransform( obj_corners, scene_corners, H);

            //Draw lines between the corners (the mapped object in the scene image )
            line( img_matches, scene_corners[0] + Point2f( object.cols, 0), scene_corners[1] + Point2f( object.cols, 0), Scalar(0, 255, 0), 4 );
            line( img_matches, scene_corners[1] + Point2f( object.cols, 0), scene_corners[2] + Point2f( object.cols, 0), Scalar( 0, 255, 0), 4 );
            line( img_matches, scene_corners[2] + Point2f( object.cols, 0), scene_corners[3] + Point2f( object.cols, 0), Scalar( 0, 255, 0), 4 );
            line( img_matches, scene_corners[3] + Point2f( object.cols, 0), scene_corners[0] + Point2f( object.cols, 0), Scalar( 0, 255, 0), 4 );
        }

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