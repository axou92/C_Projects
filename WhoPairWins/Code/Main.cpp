#include <opencv2\opencv.hpp>
#include "opencv2/highgui/highgui.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <thread>  
#include<time.h>
#include <windows.h>

using namespace cv;
using namespace std;

/// Main function.
int main()
{
	/// Variables
	int n = 0; 
	int dispImages = 11;
	vector<String> imageList;

	int timeMax = 0;
	cout << " Enter the maximum time (in seconds) for pairing images: ";
	cin >> timeMax;

	glob("Images/*", imageList, false);

	n = imageList.size();
	cout << n << endl;

	vector <Mat> img(dispImages);
	vector <Mat> resizeImg(dispImages);

	/// Instructions in the first window
	Mat instruction = Mat::zeros(Size(1300, 600), CV_8UC3);
	putText(instruction, "Welcome to the 'Who Pair Wins' game !", Point(350, 40), cv::FONT_HERSHEY_DUPLEX, 0.8, Scalar(0, 0, 255), 1, 16);
	
	putText(instruction, "Instructions:", Point(5, 100), cv::FONT_HERSHEY_DUPLEX, 0.8, Scalar(0, 255, 0), 1, 12);
	putText(instruction, format("You have %i seconds for parings images between 1 and %i", timeMax, dispImages), Point(25, 130), cv::FONT_HERSHEY_DUPLEX, 0.8, Scalar(255, 255, 255), 1, 10);
	putText(instruction, "Press any key for stopping the timer and discuss about result", Point(25, 160), cv::FONT_HERSHEY_DUPLEX, 0.8, Scalar(255, 255, 255), 1, 10);
	putText(instruction, "Press any key for changing images when the timer is in pause", Point(25, 190), cv::FONT_HERSHEY_DUPLEX, 0.8, Scalar(255, 255, 255), 1, 10);
	
	putText(instruction, "Score:", Point(5, 250), cv::FONT_HERSHEY_DUPLEX, 0.8, Scalar(0, 255, 0), 1, 12);
	putText(instruction, "Adding to your score the number of players which have the same pair", Point(25, 280), cv::FONT_HERSHEY_DUPLEX, 0.8, Scalar(255, 255, 255), 1, 10);
	putText(instruction, "Adding to your score the double of the number of players which have the same image alone", Point(25, 310), cv::FONT_HERSHEY_DUPLEX, 0.8, Scalar(255, 255, 255), 1, 10);
	
	putText(instruction, "At any moment, press echap key for quitting the game", Point(5, 420), cv::FONT_HERSHEY_DUPLEX, 0.8, Scalar(255, 0, 0), 1, 10);
	putText(instruction, "Press a key for beginning the game ", Point(5, 460), cv::FONT_HERSHEY_DUPLEX, 0.8, Scalar(255, 0, 0), 1, 10);
	putText(instruction, format("If the reamning time is lock at %i, just press a key for loading new images ",timeMax), Point(5, 500), cv::FONT_HERSHEY_DUPLEX, 0.8, Scalar(255, 0, 0), 1, 10);
	imshow("Instruction", instruction);

	waitKey(0);
	while (1)
	{
		int key = cvWaitKey(33);
		if (key == 27)
		{
			break;
		}

		else
		{
			/// Initialise random seed for changing images randomly.
			srand(time(NULL)); 
			int seed = rand() % 1001;
			RNG rng(seed);
			random_shuffle(imageList.begin(), imageList.end(), rng);

			for (int i = 0; i < dispImages; i++)
			{
				img[i] = imread(imageList[i]);
				resize(img[i], img[i], Size(450, 270));
				resizeImg[i] = Mat(img[i].rows + 30, img[i].cols, img[i].type(), Scalar(0));

				/// Specify the size of the copy and its offset.
				Rect offset_rect = cv::Rect(0, 0, img[i].cols, img[i].rows);

				/// Copy to the output Mat.
				img[i].copyTo(resizeImg[i](offset_rect));
				putText(resizeImg[i], format("%d", i + 1), cvPoint(resizeImg[i].cols / 2, resizeImg[i].rows - 10), FONT_HERSHEY_COMPLEX_SMALL, 1.1, Scalar(255, 255, 255), 1, 8);
			}

			/// Create a big image. 
			Mat game(Size(resizeImg[0].cols * 4, resizeImg[0].rows * 3), CV_8UC3);
			resizeImg[0].convertTo(game(Rect(resizeImg[0].cols * 0, resizeImg[0].rows * 0, resizeImg[0].cols, resizeImg[0].rows)), 0);
			resizeImg[1].convertTo(game(Rect(resizeImg[0].cols * 1, resizeImg[0].rows * 0, resizeImg[0].cols, resizeImg[0].rows)), 0);
			resizeImg[2].convertTo(game(Rect(resizeImg[0].cols * 2, resizeImg[0].rows * 0, resizeImg[0].cols, resizeImg[0].rows)), 0);
			resizeImg[3].convertTo(game(Rect(resizeImg[0].cols * 3, resizeImg[0].rows * 0, resizeImg[0].cols, resizeImg[0].rows)), 0);
			resizeImg[4].convertTo(game(Rect(resizeImg[0].cols * 0, resizeImg[0].rows * 1, resizeImg[0].cols, resizeImg[0].rows)), 0);
			resizeImg[5].convertTo(game(Rect(resizeImg[0].cols * 1, resizeImg[0].rows * 1, resizeImg[0].cols, resizeImg[0].rows)), 0);
			resizeImg[6].convertTo(game(Rect(resizeImg[0].cols * 2, resizeImg[0].rows * 1, resizeImg[0].cols, resizeImg[0].rows)), 0);
			resizeImg[7].convertTo(game(Rect(resizeImg[0].cols * 3, resizeImg[0].rows * 1, resizeImg[0].cols, resizeImg[0].rows)), 0);
			resizeImg[8].convertTo(game(Rect(resizeImg[0].cols * 0, resizeImg[0].rows * 2, resizeImg[0].cols, resizeImg[0].rows)), 0);
			resizeImg[9].convertTo(game(Rect(resizeImg[0].cols * 1, resizeImg[0].rows * 2, resizeImg[0].cols, resizeImg[0].rows)), 0);
			resizeImg[10].convertTo(game(Rect(resizeImg[0].cols * 2, resizeImg[0].rows * 2, resizeImg[0].cols, resizeImg[0].rows)), 0);
			
			bool keyPressed = false;
			int start = time(0);
			while (keyPressed == false)
			{
				int key2 = cvWaitKey(33);
				if (key2 != -1)
				{
					cout << "Key pressed !" << endl;
					keyPressed = true;
				}
				else if (key2 == 27)
				{
					break;
				}

				int t = time(0) - start;
				int remaningTime = timeMax - t;

				/// Display the time in different color.
				Mat timer = Mat::zeros(Size(resizeImg[0].cols, resizeImg[0].rows), CV_8UC3);
				int b, g, r = 0;
				if (remaningTime < 15)
				{
					b = 0;
					g = 0;
					r = 255;
				}
				else if (remaningTime >= 15 && remaningTime < 30)
				{
					b = 0;
					g = 255;
					r = 255;
				}
				else if (remaningTime >= 30 && remaningTime < 45)
				{
					b = 255;
					g = 255;
					r = 0;
				}
				else if (remaningTime >= 45 && remaningTime < 60)
				{
					b = 0;
					g = 255;
					r = 0;
				}
				else
				{
					b = 255;
					g = 255;
					r = 255;
				}
				Scalar color(b, g, r);
				putText(timer, "Remaning time: ", cvPoint(timer.cols / 3, timer.rows / 2 - 10), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(255, 255, 255), 1, 8);
				putText(timer, format("%i", remaningTime), cvPoint(timer.cols / 2, timer.rows / 2 + 30), FONT_HERSHEY_COMPLEX_SMALL, 2, color, 1, 20);
				timer.convertTo(game(Rect(resizeImg[0].cols * 3, resizeImg[0].rows * 2, resizeImg[0].cols, resizeImg[0].rows)), 0);
				
				/// Time information with Beep.
				if (remaningTime % 10 == 0 && remaningTime > 0)
				{
					cout << "Beep" << endl;
					Beep(600, 500);
				}
				if (remaningTime == 3)
				{
					cout << "Beep" << endl;
					Beep(400, 1000);
				}
				if (remaningTime == 2)
				{
					cout << "Beep" << endl;
					Beep(400, 1000);
				}
				if (remaningTime == 1)
				{
					cout << "Beep" << endl;
					Beep(400, 1000);
				}
				if (remaningTime == 0)
				{
					cout << "Beep" << endl;
					Beep(300, 2000);
					keyPressed = true;
					cout << keyPressed << endl;
				}
				/// Display result. 
				namedWindow("Game", CV_WINDOW_NORMAL);
				setWindowProperty("Game", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
				imshow("Game", game);

				cvWaitKey(1);
			}
			cout << "Wait next image" << endl;
			
			cvWaitKey(0);
		}
	}

	cout << "Thanks for playing !" << endl;
	cvWaitKey(0);
	return 0;
}
