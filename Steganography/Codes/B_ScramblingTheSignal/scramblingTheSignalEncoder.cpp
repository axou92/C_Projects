/*
This program encode a message image in a grayscale image with a password.
Shuffuling the message image thanks to a password. 
If the shuffle message pixel is black, adding 1 at the same pixel in the grayscale image.
If the shuffle message pixel is white, adding 0 at the same pixel in the grayscale image.
*/
#include <stdio.h>

#include <opencv2\opencv.hpp>
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;

// Variables
Mat grayscaleCarrierImage; // Grayscale image
string image;

Mat binaryImage; // Binary message image
string message;

Mat shuffleBinaryImage; // Shuffle the binary image thanks to a the password

Mat encodedImage; // Result image 
string newImage;

string password;

// Assume that both the carrier and message images are the same dimension
int C; // Number of columns
int R; // Number of rows

// For generate a same random integer with a same seed
unsigned long seed;

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
	cout << "You are in the encoder program. This program will encode a grayscale image and a message thanks to a password. " << endl;

	// Instructions to follows
	cout << "Chose a Grayscale Carrier image in ImageSrc folder (add also the folder if it's necessary): " << endl;
	cin >> image;
	cout << "Chose the hidden mesage in ImageSrc folder (add also the folder if it's necessary): " << endl;
	cin >> message;
	cout << "Chose a character string password in order to hidde this message : " << endl;
	cin >> password;
	cout << "Chose the name of the encoded image (This image will create in the ImageSrc folder):  " << endl;
	cin >> newImage;

	// Load the carrier image
	grayscaleCarrierImage = imread("ImageSrc/" + image, CV_LOAD_IMAGE_GRAYSCALE);
	if (grayscaleCarrierImage.empty())
	{
		cout << "Sorry, the image " << image << "  does not exist." << endl;
		return -1;
	}
	imshow("Grayscale Carrier", grayscaleCarrierImage);

	// Load the message image
	binaryImage = imread("ImageSrc/" + message, CV_LOAD_IMAGE_GRAYSCALE);
	if (binaryImage.empty())
	{
		cout << "Sorry, the image " << message << "  does not exist." << endl;
		return -1;
	}
	imshow("Binary image", binaryImage);

	// Change the password to 64 bit integer using the hash function
	seed = Hash(password.c_str());

	// Resolution of the image
	C = binaryImage.cols;
	R = binaryImage.rows;

	// Aray for indicate the column
	vector<int> randomColumn(C);
	for (int i = 0; i < randomColumn.size(); i++)
	{
		randomColumn.at(i) = i;
	}
	// Array for indcate the row 
	vector<int> randomRow(R);
	for (int i = 0; i < randomRow.size(); i++)
	{
		randomRow.at(i) = i;
	}

	// Generating a random integer from a given seed (given by the password)
	RNG randomNumberGenerator(seed);

	// Shuffle indexes of the colomn and row arrays thanks to the password
	random_shuffle(randomColumn.begin(), randomColumn.end(), randomNumberGenerator);
	random_shuffle(randomRow.begin(), randomRow.end(), randomNumberGenerator);

	// Encoded image

	// First, hidding the message image 
	for (int i = 0; i < R; i++)
	{
		for (int j = 0; j < C; j++)
		{		
			if (binaryImage.at<uchar>(i, j) == 255)
			{
				binaryImage.at<uchar>(i, j) = 0;
			}
			else
			{
				binaryImage.at<uchar>(i, j) = 1;
			}
		}
	}
	/*
	Next, clonning the grayscale carrier image in order to
	have the same dimension for the shuffleBinaryImage
	*/
	shuffleBinaryImage = grayscaleCarrierImage.clone();
	// Then, adding the shuffle message at each pixel of the shuffleBinaryImage
	for (int i = 0; i < R; i++)
	{
		for (int j = 0; j < C; j++)
		{
			// Shuffle the message
			shuffleBinaryImage.at<uchar>(i, j) = binaryImage.at<uchar>(randomRow[i], randomColumn[j]);
		}
	}
	// Now, encoding the image
	for (int i = 0; i < R; i++)
	{
		for (int j = 0; j < C; j++)
		{
			if (grayscaleCarrierImage.at<uchar>(i, j) != 255)
			{
				encodedImage = grayscaleCarrierImage + shuffleBinaryImage;
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

// The image "encoded.png" is coded with the password: test