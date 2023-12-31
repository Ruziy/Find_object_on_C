#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <cmath>
#include <fstream>



using namespace cv;
using namespace std;
Mat src;
Mat different_Channels[3];
Mat r;
Mat src_hsv, src_gray, src_thresh;
Mat img_res;
Mat mask, mask1, mask2;
Mat glassier;
Mat canny_output;
Mat box_hsv;
Mat img_dill;
Mat masked_gray;
Scalar meanBrightness;
bool logic = false;
bool find_object = false;
vector< int > arr;
int kernel_size = 1;
int thresh = 10;
int max;
const int regionWidth = 40;
const int regionHeight = 40;
const float min_temp = 0.0f;
const float max_temp = 100.0f;
Point poly_points[1][4];
const Point* ppt[1] = { poly_points[0] };
int npt[] = { 4 };
bool My_callback();
float normalizeToTemperature(int gray, float min_temp, float max_temp) {
	return (max_temp - min_temp) * (gray / 255.0) + min_temp;
}
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

	poly_points[0][0] = cv::Point(box.at<int>(0, 0), box.at<int>(0, 1));
	poly_points[0][1] = cv::Point(box.at<int>(1, 0), box.at<int>(1, 1));
	poly_points[0][2] = cv::Point(box.at<int>(2, 0), box.at<int>(2, 1));
	poly_points[0][3] = cv::Point(box.at<int>(3, 0), box.at<int>(3, 1));
	cv::polylines(img_res, ppt, npt, 1, true, cv::Scalar(0, 165, 255), 3);
}
int main()
{
	//auto start = chrono::steady_clock::now();
	VideoCapture cap(0);
	if (!cap.isOpened())
	{
		cout << "Could not open video capture!" << endl;
		return -1;
	}

	while (true)
	{
		// ��������� ���� � �����������
		cap >> src;

		// ���������, ������� �� ������� ����
		if (src.empty())
		{
			cout << "Could not read frame!" << endl;
			break;
		}

		// �������� ���� ������� My_callback() ��� ��������� �����
		My_callback();

		// ��� ������ �� ����� ������� ������� 'q'
		if (waitKey(1) == 'q')
			break;
	}

	//auto end = chrono::steady_clock::now();
	//auto diff = end - start;
	//cout << chrono::duration <double, milli>(diff).count() << " ms" << endl;
	return 0;
}
bool My_callback()
{
	img_res = src.clone();
	cv::split(src, different_Channels);
	r = different_Channels[2];
	double red_mean = cv::mean(r)[0];
	for (int i = 0; i < r.rows; i++) {
		for (int j = 0; j < r.cols; j++) {
			if (r.at<uchar>(i, j) < red_mean) {
				r.at<uchar>(i, j) = 0;
			}
		}
	}
	cvtColor(src, src_gray, COLOR_BGR2GRAY);
	cvtColor(src, src_hsv, COLOR_BGR2HLS);
	meanBrightness = cv::mean(src_hsv)[1];
	std::cout << "Average brightness: " << meanBrightness.val[0] << std::endl;
	cv:inRange(src_hsv, Scalar(0, 50, 20), Scalar(5, 255, 255), mask1);
	cv::inRange(src_hsv, Scalar(170, 50, 20), Scalar(180, 255, 255), mask2);
	cv::bitwise_or(mask1, mask2, mask);
	vector<vector<Point> > contours_hsv;
	findContours(mask, contours_hsv, RETR_TREE, CHAIN_APPROX_SIMPLE);

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
	int contours_size = 0;
	while (logic == false) {
		Canny(r, canny_output, thresh, thresh * 2);
		vector<vector<Point> > contours;
		Mat kernel = cv::getStructuringElement(MORPH_RECT, Size(kernel_size, kernel_size));
		Mat box;
		cv::dilate(canny_output, img_dill, kernel);
		findContours(img_dill, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
		contours_size += contours.size();
		if (contours_size > 3000) {
			if (meanBrightness.val[0] < 30) {
				GaussianBlur(r, glassier, Size(5, 5), 0);
			}
			else {
				GaussianBlur(r, glassier, Size(1, 1), 0);
			}
			Canny(glassier, canny_output, thresh, thresh * 2);
			cv::dilate(canny_output, img_dill, kernel);
			findContours(img_dill, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
		}
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
						find_object = true;
						Draw_on_sqr(box);
						Mat mask = Mat::zeros(src.rows, src.cols, CV_8UC1);
						cv::fillPoly(mask, ppt, npt, 1, cv::Scalar(255));
						src_gray.copyTo(masked_gray, mask);
						std::ofstream csvfile;
						csvfile.open("pixel_temperatures.csv");
						csvfile << "X,Y,Temperature\n";
						for (int y = 0; y < masked_gray.rows; y++) {
							for (int x = 0; x < masked_gray.cols; x++) {
								uchar grayValue = masked_gray.at<uchar>(y, x);

								if (grayValue > 0) {
									float temperature = normalizeToTemperature(grayValue, min_temp, max_temp);
									csvfile << x << "," << y << "," << temperature << "\n";
								}
							}
						}
						csvfile.close();

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
	cv::imshow("Hull demo", src);
	cv::imshow("Canny", canny_output);
	cv::imshow("Result", img_res);
	cv::imshow("Result-mask", mask);
	cv::imshow("Red Channel", r);
	if (find_object) {
		return true;
	}
	else {
		return false;
	}

}