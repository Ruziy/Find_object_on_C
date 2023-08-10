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
	else if(abs(varior_1) < 0.2 && abs(varior_2) < 0.2){
		return true;
	}
	else {
		return false;
	}
}

Mat src = imread("images/DZ.jpg");
Mat img_res = src.clone();
Mat img_res_max = src.clone();
Mat src_gray;
bool logik = false;
vector< int > arr;
int kernel_size =2;
int thresh = 5;
void thresh_callback(int, void*);
int main(int argc, char** argv)
{
	
	if (src.empty())
	{
		cout << "Could not open or find the image!\n" << endl;
		cout << "Usage: " << argv[0] << " <Input image>" << endl;
		return -1;
	}
	cvtColor(src, src_gray, COLOR_BGR2GRAY);
	thresh_callback(0, 0);
	waitKey();
	return 0;
}
void thresh_callback(int, void*)
{
	Mat canny_output;
	Canny(src_gray, canny_output, thresh, thresh*2);
	vector<vector<Point> > contours;
	Mat kernel = getStructuringElement(MORPH_RECT, Size(kernel_size, kernel_size));
	Mat img_dill;
	Mat box;
	
	dilate(canny_output, img_dill, kernel);
	findContours(img_dill, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
	vector<vector<Point> >hull(contours.size());
	
	for (size_t i = 0; i < contours.size(); i++)
	{
		convexHull(contours[i], hull[i]);
	}
	for (size_t i = 0; i < contours.size(); i++)
	{
		Scalar color = Scalar(0,255,0);
		approxPolyDP(hull[i], box, 10, true);
		if (box.rows == 4)
		{
			bool res = Parall(box);
			if (res) {
				arr.push_back(int(contourArea(box)));
				line(img_res, Point(box.at<int>(0, 0), box.at<int>(0, 1)), Point(box.at<int>(1, 0), box.at<int>(1, 1)), Scalar(0, 165, 255), 3);
				line(img_res, Point(box.at<int>(2, 0), box.at<int>(2, 1)), Point(box.at<int>(3, 0), box.at<int>(3, 1)), Scalar(0, 165, 255), 3);
				line(img_res, Point(box.at<int>(0, 0), box.at<int>(0, 1)), Point(box.at<int>(3, 0), box.at<int>(3, 1)), Scalar(0, 165, 255), 3);
				line(img_res, Point(box.at<int>(1, 0), box.at<int>(1, 1)), Point(box.at<int>(2, 0), box.at<int>(2, 1)), Scalar(0, 165, 255), 3);
			}
		}
		drawContours(src, hull, (int)i, color, 2);
	}
	if (logik == false) {
		thresh = thresh + 5;
		if (kernel_size > 8) {
			logik = true;
			thresh_callback(0, 0);
		}
		if (thresh > 100) {
			kernel_size++;
			thresh = 10;
			cout << "KERNEL=========" << kernel_size<<endl;
			thresh_callback(0, 0);
		}
		thresh_callback(0, 0);
	}
	int max = *max_element(arr.begin(), arr.end());
	for (size_t i = 0; i < contours.size(); i++)
	{
		approxPolyDP(hull[i], box, 10, true);
		if ((int(contourArea(box))) == max)
		{
				line(img_res_max, Point(box.at<int>(0, 0), box.at<int>(0, 1)), Point(box.at<int>(1, 0), box.at<int>(1, 1)), Scalar(255, 165, 255), 3);
				line(img_res_max, Point(box.at<int>(2, 0), box.at<int>(2, 1)), Point(box.at<int>(3, 0), box.at<int>(3, 1)), Scalar(255, 165, 255), 3);
				line(img_res_max, Point(box.at<int>(0, 0), box.at<int>(0, 1)), Point(box.at<int>(3, 0), box.at<int>(3, 1)), Scalar(255, 165, 255), 3);
				line(img_res_max, Point(box.at<int>(1, 0), box.at<int>(1, 1)), Point(box.at<int>(2, 0), box.at<int>(2, 1)), Scalar(255, 165, 255), 3);
		}
	}
	imshow("Hull demo", src);
	imshow("Edges", img_dill);
	imshow("Result", img_res);
	imshow("Result_max", img_res_max);
}
