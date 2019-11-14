/*
This program encode a message image in a grayscale image.
If the message pixel is black, adding 1 at the same pixel in the grayscale image.
If the message pixel is white, adding 0 at the same pixel in the grayscale image.
*/
#include <stdio.h>

#include <opencv2\opencv.hpp>
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

// Variables
Mat  grayscaleCarrierImage; // Grayscale image
string image;

Mat  binaryImage; // Binary message image
string message;

Mat encodedImage; // Result image 
string newImage;

// Assume that both the carrier and message images are the same dimension
int C; // Number of columns
int R; // Number of rows

vector<int> params;

int main()
{
	cout << "You are in the encoder program. This program will encode a grayscale image and a message. " << endl;

	// Instructions to follows
	cout << "Chose a Grayscale Carrier image in ImageSrc (add also the folder if it's necessary): " << endl;
	cin >> image;
	cout << "Chose the hidden mesage in ImageSrc folder (add also the folder if it's necessary): " << endl;
	cin >> message;
	cout << "Chose the name of the encoded image (This image will create in the ImageSrc folder):  " << endl;
	cin >> newImage;

	// Load the carrier image
	grayscaleCarrierImage = imread("ImageSrc/" + image, CV_LOAD_IMAGE_GRAYSCALE);
	if (grayscaleCarrierImage.empty())
	{
		cout << "Sorry, the image " << image << "  does not exist."  << endl;
		return -1;
	}
	imshow("Grayscale Carrier", grayscaleCarrierImage);
	
	// Load the message image
	binaryImage = imread("ImageSrc/" + message, CV_LOAD_IMAGE_GRAYSCALE);
	if (binaryImage.empty())
	{
		cout << "Sorry, the message " << message << "  does not exist." << endl;
		return -1;
	}
	imshow("Binary image", binaryImage);
	
	// Resolution of images
	C = grayscaleCarrierImage.cols;
	R = grayscaleCarrierImage.rows;
	
	// Encoded image	
	for (int i = 0; i < R; i++)
	{
		for (int j = 0; j < C; j++)
		{
			// First change white pixel in 0 and black pixel in 1
			if (binaryImage.at<uchar>(i, j) == 255)
			{
				binaryImage.at<uchar>(i, j) = 0;
			}
			else
			{
				binaryImage.at<uchar>(i, j) = 1;
			}
			
			// Then we encode the image
			if (grayscaleCarrierImage.at<uchar>(i, j) != 255)
			{
				encodedImage = grayscaleCarrierImage + binaryImage;
			}
			// Preventing overflow
			else
			{
				encodedImage.at<uchar>(i, j) = 255;
			}	
		}
	}
	imshow("Encoded image", encodedImage);
	
	// Writeout the encoded image in a PNG file
	params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	params.push_back(95);
	imwrite("ImageSrc/" + newImage, encodedImage, params);

	waitKey(0);
	return 0;
}
