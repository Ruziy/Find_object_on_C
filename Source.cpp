#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <cmath>


using namespace cv;
using namespace std;
Mat src = imread("images/dz-1.jpg");
Mat different_Channels[3];
Mat r;
//src[:, : , 0] = np.zeros([src.shape[0], src.shape[1]]);
Mat src_hsv;
Mat src_gray;
Mat src_thresh;
Mat img_res = src.clone();
Mat img_res_max = src.clone();
Mat mask, mask1, mask2;
Mat glassier;
Mat canny_output;
Mat img_dill;
bool logic = false;
vector< int > arr;
int kernel_size = 1;
int thresh = 10;
int max;
void My_callback(int max);
bool Parall(Mat box) {
	double res1 = double(box.at<int>(0, 1) - box.at<int>(1, 1)) / (box.at<int>(0, 0) - box.at<int>(1, 0));
	double res2 = double(box.at<int>(2, 1) - box.at<int>(3, 1)) / (box.at<int>(2, 0) - box.at<int>(3, 0));
	double res3 = double(box.at<int>(0, 1) - box.at<int>(3, 1)) / (box.at<int>(0, 0) - box.at<int>(3, 0));
	double res4 = double(box.at<int>(1, 1) - box.at<int>(2, 1)) / (box.at<int>(1, 0) - box.at<int>(2, 0));
	res1 = ceil(res1 * 10.0) / 10.0;
	res2 = ceil(res2 * 10.0) / 10.0;
	res3 = ceil(res3 * 10.0) / 10.0;
	res4 = ceil(res4 * 10.0) / 10.0;
	double varior_1 = ceil((res1 - res2) * 100.0) / 100.0;
	double varior_2 = ceil((res3 - res4) * 100.0) / 100.0;
	if (res1 == res2 && res3 == res4) {
		return true;
	}
	else if (abs(varior_1) < 0.2 && abs(varior_2) < 0.2) {
		return true;
	}
	else {
		return false;
	}
}
void Draw_on_sqr(Mat box) {
	line(img_res, Point(box.at<int>(0, 0), box.at<int>(0, 1)), Point(box.at<int>(1, 0), box.at<int>(1, 1)), Scalar(0, 165, 255), 3);
	line(img_res, Point(box.at<int>(2, 0), box.at<int>(2, 1)), Point(box.at<int>(3, 0), box.at<int>(3, 1)), Scalar(0, 165, 255), 3);
	line(img_res, Point(box.at<int>(0, 0), box.at<int>(0, 1)), Point(box.at<int>(3, 0), box.at<int>(3, 1)), Scalar(0, 165, 255), 3);
	line(img_res, Point(box.at<int>(1, 0), box.at<int>(1, 1)), Point(box.at<int>(2, 0), box.at<int>(2, 1)), Scalar(0, 165, 255), 3);
}
int main()
{

	auto start = chrono::steady_clock::now();//~1500
	if (src.empty())
	{
		cout << "Could not open or find the image!\n" << endl;
		return -1;
	}
	split(src, different_Channels);//splitting images into 3 different channels//  
	r = different_Channels[2];
	cvtColor(src, src_gray, COLOR_BGR2GRAY);
	cvtColor(src, src_hsv, COLOR_BGR2HLS);
	inRange(src_hsv, Scalar(0, 50, 20), Scalar(5, 255, 255), mask1);
	inRange(src_hsv, Scalar(175, 50, 20), Scalar(180, 255, 255), mask2);
	bitwise_or(mask1, mask2, mask);
	vector<vector<Point> > contours_hsv;
	findContours(mask, contours_hsv, RETR_TREE, CHAIN_APPROX_SIMPLE);
	Mat box_hsv;
	for (size_t i = 0; i < contours_hsv.size(); i++)
	{
		cv::approxPolyDP(contours_hsv[i], box_hsv, 10, true);
		int area = int(cv::contourArea(box_hsv));
		if (area > 300) {
			arr.push_back(area);
		}
	}
	const int max = *std::max_element(arr.begin(), arr.end());
	cout << "MAAAAAAAAAX" << max << endl;
	My_callback(max);
	auto end = chrono::steady_clock::now();
	auto diff = end - start;
	cout << chrono::duration <double, milli>(diff).count() << " ms" << endl;
	waitKey();
	return 0;
}
void My_callback(int max)
{
	int contours_size = 0;
	while (logic == false) {
		Canny(r, canny_output, thresh, thresh * 2);
		vector<vector<Point> > contours;
		Mat kernel = cv::getStructuringElement(MORPH_RECT, Size(kernel_size, kernel_size));
		Mat box;
		cv::dilate(canny_output, img_dill, kernel);
		findContours(img_dill, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
		contours_size += contours.size();
		//if (contours_size > 1000) {
		//	cout << "HERE" << endl;
		//	GaussianBlur(r, glassier, Size(5, 5), 0);
		//	Canny(glassier, canny_output, thresh, thresh * 2);
		//	cv::dilate(canny_output, img_dill, kernel);
		//	findContours(img_dill, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
		//}
		vector<vector<Point>>hull(contours.size());
		for (size_t i = 0; i < contours.size(); i++)
		{
			cv::convexHull(contours[i], hull[i]);
		}
		for (size_t i = 0; i < contours.size(); i++)
		{
			Scalar color = Scalar(0, 255, 0);
			cv::approxPolyDP(hull[i], box, 10, true);
			if (box.rows == 4)
			{
				bool res = Parall(box);
				if (res) {
					if (cv::contourArea(box) > max) {
						logic = true;
						Draw_on_sqr(box);
					}
				}
			}
			cv::drawContours(src, hull, (int)i, color, 2);
		}
		thresh += 10;
		if (kernel_size > 7) {
			logic = true;
		}
		if (thresh > 100) {
			kernel_size++;
			thresh = 10;
			cout << "KERNEL=========" << kernel_size << endl;
		}
	}
	cout << contours_size << endl;
	cv::imshow("Hull demo", src);
	cv::imshow("Canny",canny_output);
	cv::imshow("Result", img_res);
	cv::imshow("Result-mask", mask);
	imshow("Red Channel", r);
	imshow("img_dill", img_dill);
	//imshow("glassier", glassier);
}




