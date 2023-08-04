#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>
#include <cmath>
using namespace cv;
using namespace std;


void main()
{
	//string path = "images/avatar.jpg";
	//Mat image = imread(path);

	//CascadeClassifier faceCascade;
	//faceCascade.load("xmls/faces.xml");
	//if (faceCascade.empty()) {
	//	cout << "XML not loaded" << endl;
	//}
	//vector<Rect> faces;
	//faceCascade.detectMultiScale(image, faces, 1.1, 10);
	//for (int i = 0; i < faces.size(); i++)
	//{
	//	rectangle(image, faces[i].tl(), faces[i].br(), Scalar(0, 0, 255), 3);

	//	imshow("Image", image);
	//	waitKey(0);
	//}
	//Vudeo
	string path = "videos/videos-object.mp4";
	VideoCapture cap(path);
	Mat img;
	// Create a window
	namedWindow("My Window", 1);
	int iSliderValue1 = 50;
	createTrackbar("Track-1", "My Window", &iSliderValue1, 255);
	int iSliderValue2 = 50;
	createTrackbar("Track-2", "My Window", &iSliderValue2, 255);
	while (true) {
		cap.read(img);
		Mat img_resize,img_gray, img_blur, edges,img_dill,box,box_int;
		resize(img, img_resize, Size(800, 600));
		cvtColor(img_resize, img_gray, COLOR_RGB2GRAY);
		GaussianBlur(img_gray, img_blur,Size(9,9),0);
		Canny(img_blur, edges, iSliderValue1, iSliderValue2);
		Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
		dilate(edges, img_dill, kernel);
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		findContours(img_dill, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
		
		for (unsigned int i = 0; i < contours.size(); i++)
		{
			if (contourArea(contours[i]) > 10000) {
				RotatedRect rect = minAreaRect(contours[i]);
				boxPoints(rect,box);
				box.convertTo(box_int, CV_32S, 10000);
				box_int.convertTo(box_int, CV_32S, 0.0001);
				drawContours(img_resize, box_int, 0, Scalar(0, 255, 0), 2);

			}
			
		}
		
		imshow("Edges", edges);
		imshow("Dill", img_dill);
		imshow("Res", img_resize);
		waitKey(20);

			
	};
}
	