/*
This program generate a gaussian noise for both colour and grayscale image.
Selecting if a colour image is use.
Adding a password in order to add the gaussian noise in the image.
*/
#include <stdio.h>

#include <opencv2\opencv.hpp>
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;

// Variables

int grayImage; // Take value of 0 for colour image and 1 for gray image

Mat  carrierImage; // Carrier image (grayscale or colour)
string image;

Mat  noiseImage; // Image with the gaussian noise
string noise;

string password;  

int C; // Number of columns in the image
int R; // Number of rows in the image

// For generate a same random integer with a same seed
unsigned long seed;
double standardDeviation;

// noise pixel value of the noiseImage
int noisePixel;

vector<int> params;

// This following function is obtained thanks to http://www.cse.yorku.ca/~oz/hash.html 
unsigned long Hash(const char *str)
{
	unsigned long hash = 5381;
	int c;

	while (c = *str++)
		hash = ((hash << 5) + hash) + c; // hash * 33 + c 

	return hash;
}

int main()
{
	cout << "You are in the noise program. This program will add gaussian noise in an image thanks to a password and the standard deviation. " << endl;

	// Instructions to follows
	cout << "It is a grayscale image ? (0 for false and 1 for true)" << endl;
	cin >> grayImage;
	cout << "Chose a Carrier image in ImageSrc folder (add also the folder if it's necessary): " << endl;
	cin >> image;
	cout << "Chose the name of the new image with gaussian noise (add also the folder if it's necessary): " << endl;
	cin >> noise;
	cout << "Chose a character string password in order to hidde this message : " << endl;
	cin >> password;
	cout << "Chose the standard deviation (integer) : " << endl;
	cin >> standardDeviation;

	// Load the carrier image
	carrierImage = imread("ImageSrc/" + image, CV_LOAD_IMAGE_UNCHANGED);
	if (carrierImage.empty())
	{
		cout << "Sorry, the image " << image << " does not exist." << endl;
		return -1;
	}
	imshow("Carrier image", carrierImage);

	// Change the password to 64 bit integer using the hash function
	seed = Hash(password.c_str());

	// Image resolution (C*R)
	C = carrierImage.cols;
	R = carrierImage.rows;

	// Generating a random integer from a given seed
	RNG randomNumberGenerator(seed);

	// Create  a random noise image from a given password
	noiseImage = carrierImage.clone();
	for (int i = 0; i < R; i++)
	{
		for (int j = 0; j < C; j++)
		{
			if (grayImage == 0) // colour image
			{
				for (int k = 0; k < 3; k++)
				{
					// Noise given by the password and the standard deviation
					noisePixel = noiseImage.at<Vec3b>(i, j)[k] + randomNumberGenerator.gaussian(standardDeviation);
					// Noise can be positive or negative
					// Case of integer overflow
					if (noisePixel > 255)
					{
						noisePixel = 255;
					}
					else if (noisePixel < 0)
					{
						noisePixel = 0;
					}
					// Add at each colour channel a noise pixel
					noiseImage.at<Vec3b>(i,j)[k] = noisePixel;
				}
				
			}
			else if (grayImage == 1) // gray image
			{
				// Noise given by the password and the standard deviation
				noisePixel = noiseImage.at<uchar>(i, j) + randomNumberGenerator.gaussian(standardDeviation);
				// Noise can be positive or negative
				// Case of integer overflow
				if (noisePixel > 255)
				{
					noisePixel = 255;
				}
				else if (noisePixel < 0)
				{
					noisePixel = 0;
				}
				// Add at each colour channel a noise pixel
				noiseImage.at<uchar>(i, j) = noisePixel;
			}
		}
	}
	imshow("Noise image", noiseImage);

	// Writeout the noise image in a PNG file
	params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	params.push_back(95);
	imwrite("ImageSrc/" + noise, noiseImage, params);

	waitKey(0);
	return 0;
}