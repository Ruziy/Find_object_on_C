#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <cmath>

using namespace cv;
using namespace std;
bool parall(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) {
	double res1 = double(y1 - y2) / (x1 - x2);
	double res2 = double(y3 - y4) / (x3 - x4);
	res1 = ceil(res1 * 10.0) / 10.0;
	res2 = ceil(res2 * 10.0) / 10.0;
	if (res1 == res2) {
		return true;
	}
	else {
		return false;
	}
}

Mat src = imread("images/DZ.jpg");
Mat img_res = src.clone();
Mat src_gray;
int thresh = 67;
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
	Mat kernel = getStructuringElement(MORPH_RECT, Size(7, 7));
	Mat img_dill;
	
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
		Mat box;
		approxPolyDP(hull[i], box, 10, true);
		
		if (box.rows == 4)
		{
			int res = parall(box.at<int>(0,0), box.at<int>(0,1), box.at<int>(1, 0), box.at<int>(1, 1), 
				box.at<int>(2, 0), box.at<int>(2, 1), box.at<int>(3, 0), box.at<int>(3, 1));
			if (res == 1) {
				line(img_res, Point(box.at<int>(0, 0), box.at<int>(0, 1)), Point(box.at<int>(1, 0), box.at<int>(1, 1)), Scalar(0, 165, 255), 3);
				line(img_res, Point(box.at<int>(2, 0), box.at<int>(2, 1)), Point(box.at<int>(3, 0), box.at<int>(3, 1)), Scalar(0, 165, 255), 3);
				line(img_res, Point(box.at<int>(0, 0), box.at<int>(0, 1)), Point(box.at<int>(3, 0), box.at<int>(3, 1)), Scalar(0, 165, 255), 3);
				line(img_res, Point(box.at<int>(1, 0), box.at<int>(1, 1)), Point(box.at<int>(2, 0), box.at<int>(2, 1)), Scalar(0, 165, 255), 3);
			}
		}

		drawContours(src, hull, (int)i, color, 2);
	}
	imshow("Hull demo", src);
	imshow("Edges", img_dill);
	imshow("Result", img_res);
}
