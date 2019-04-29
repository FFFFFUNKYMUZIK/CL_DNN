#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//gpu struct
#include "gpu_struct.h"

//tester
#include "tester.h"

//include cv for webcam
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
#define INPUT_W 640
#define INPUT_H 480


void main() {

	//init gpu struct
	init();

	//cv test
	cv::VideoCapture capture(0);

	if (!capture.isOpened())
	{
		std::cerr << "Could not open camera" << std::endl;
		return;
	}

	capture.set(CAP_PROP_FRAME_WIDTH, INPUT_W);
	capture.set(CAP_PROP_FRAME_HEIGHT, INPUT_H);

	Mat frame, in_frame(INPUT_H, INPUT_W, CV_8UC1), process_frame(INPUT_H, INPUT_W, CV_8UC1);
	bool frame_valid;
	
	Scalar color1(0, 255, 0);
	Scalar color2(255, 0, 0);
	int thickness1 = 1;
	int thickness2 = 2;
	double threshold = 50;

	clock_t start, end;
	double result1, result2;
	int iter = 0;
	
	while (1) {

		frame_valid = true;
		try {
			capture >> frame; // get a new frame from webcam
		}
		catch (cv::Exception& e)
		{
			std::cerr << "Exception occurred. Ignoring frame... " << e.err << std::endl;
			frame_valid = false;
		}

		//processing
		cvtColor(frame, in_frame, COLOR_RGB2GRAY);
		run(in_frame.data, process_frame.data, in_frame.cols, in_frame.rows);


		if (frame_valid) {
			try {
				//imshow("Input", frame);
				imshow("Input", in_frame);
				imshow("Process", process_frame);
			}
			catch (cv::Exception& e)
			{
				std::cerr << "Exception occurred. Ignoring frame... " << e.err << std::endl;
			}
		}
		if (waitKey(30) >= 0) break;

		iter++;

	}

	deinit();

	return;
}

