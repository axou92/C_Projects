/*
This program decode a message image in a grayscale image in substracting the encoded image and the original image
If the result pixel is equal to 1, it's mean that we have a message at this pixel.
If the message pixel is equal to 0, we have no message at this pixel.
In changing the 0 value to 255 and 1 value to 0 we obtain the message image .
*/
#include <stdio.h>

#include <opencv2\opencv.hpp>
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

// Variables
Mat  grayscaleCarrierImage; // Grayscale image
string image;

Mat encodedImage; // Encoded image 
string encoded;

Mat hiddenMessage; // Binary hidden message image
string message;

// Assume that both the carrier and message images are the same dimension
int C; // Number of columns
int R; // Number of rows

vector<int> params;

int main(int argc, char** argv)
{
	cout << "You are in the decoder program. This program will decode a grayscale image and its message. " << endl;

	// Instructions to follows
	cout << "Chose a Grayscale Carrier image in ImageSrc (add also the folder if it's necessary): " << endl;
	cin >> image;
	cout << "Select the encoded image (add also the folder if it's necessary): " << endl;
	cin >> encoded;
	cout << "Chose the name of the message image (This image will create in the ImageSrc folder):  " << endl;
	cin >> message;

	// Load the carrier image
	grayscaleCarrierImage = imread("ImageSrc/" + image, CV_LOAD_IMAGE_GRAYSCALE);
	if (grayscaleCarrierImage.empty())
	{
		cout << "Sorry, the image " << image << "  does not exist." << endl;
		return -1;
	}
	imshow("Grayscale Carrier", grayscaleCarrierImage);

	// Load the encoded image
	encodedImage = imread("ImageSrc/" + encoded, CV_LOAD_IMAGE_GRAYSCALE);
	if (encodedImage.empty())
	{
		cout << "Sorry, the image " << encoded << "  does not exist." << endl;
		return -1;
	}
	imshow("Encoded image", encodedImage);

	// Resolution of images
	C = grayscaleCarrierImage.cols;
	R = grayscaleCarrierImage.rows;

	// Decoded image
	hiddenMessage = encodedImage - grayscaleCarrierImage;
	for (int i = 0; i < R; i++)
	{
		for (int j = 0; j < C; j++)
		{
			if (hiddenMessage.at<uchar>(i, j) == 1)
			{
				hiddenMessage.at<uchar>(i, j) = 0;
			}
			else if (hiddenMessage.at<uchar>(i, j) == 0)
			{
				hiddenMessage.at<uchar>(i, j) = 255;
			}
		}
	}
	imshow("Hidden message", hiddenMessage);
	
	// Writeout the encoded image in a PNG file
	params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	params.push_back(95);
	imwrite("ImageSrc/" + message, hiddenMessage, params);
	
	waitKey(0);
	return 0;
}

// Decoded message from testing.png:
/*
This is a secret message from a secret person.
If you can see this message then you program works
Well done - on to the next part of the practical ......

Cranfield University - AMAC, SOE, DIP 1 Practical
*/