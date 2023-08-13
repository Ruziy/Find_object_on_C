#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <cmath>


using namespace cv;
using namespace std;
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
Mat src = imread("images/dz-2.jpg");
Mat img_res = src.clone();
Mat img_res_max = src.clone();
Mat src_gray;
bool logic = false;
vector< int > arr;
int kernel_size = 1;
int thresh = 10;
void My_callback();
int main()
{
	auto start = chrono::steady_clock::now();//~1500
	if (src.empty())
	{
		cout << "Could not open or find the image!\n" << endl;
		return -1;
	}
	cvtColor(src, src_gray, COLOR_BGR2GRAY);
	My_callback();
	auto end = chrono::steady_clock::now();
	auto diff = end - start;
	cout << chrono::duration <double, milli>(diff).count() << " ms" << endl;
	waitKey();
	return 0;
}
void My_callback()
{
	while (logic == false) {
		Mat canny_output;
		Canny(src_gray, canny_output, thresh, thresh * 2);
		vector<vector<Point> > contours;
		Mat kernel = cv::getStructuringElement(MORPH_RECT, Size(kernel_size, kernel_size));
		Mat img_dill;
		Mat box;
		cv::dilate(canny_output, img_dill, kernel);
		findContours(img_dill, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
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
					arr.push_back(int(cv::contourArea(box)));
					if (cv::contourArea(box) > 5000) { //~1000
						logic = true;
						line(img_res, Point(box.at<int>(0, 0), box.at<int>(0, 1)), Point(box.at<int>(1, 0), box.at<int>(1, 1)), Scalar(0, 165, 255), 3);
						line(img_res, Point(box.at<int>(2, 0), box.at<int>(2, 1)), Point(box.at<int>(3, 0), box.at<int>(3, 1)), Scalar(0, 165, 255), 3);
						line(img_res, Point(box.at<int>(0, 0), box.at<int>(0, 1)), Point(box.at<int>(3, 0), box.at<int>(3, 1)), Scalar(0, 165, 255), 3);
						line(img_res, Point(box.at<int>(1, 0), box.at<int>(1, 1)), Point(box.at<int>(2, 0), box.at<int>(2, 1)), Scalar(0, 165, 255), 3);
					}

				}
			}
			drawContours(src, hull, (int)i, color, 2);
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
	imshow("Hull demo", src);
	//imshow("Edges", img_dill);
	imshow("Result", img_res);
	//imshow("ch1", ch1);
	//imshow("ch2", ch2);
	//imshow("Result_max", img_res_max);
}





