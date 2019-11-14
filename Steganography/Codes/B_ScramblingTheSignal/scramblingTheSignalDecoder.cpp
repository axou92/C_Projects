/*
This program decode a hidden message image, thanks to a password, in a grayscale image in substracting the encoded image and the original image
If the result pixel is equal to 1, it's mean that we have a shuffle message at this pixel.
If the message pixel is equal to 0, we have no shuffle message at this pixel.
Shuffling back the message pixel in using the password.
In changing the 0 value to 255 and 1 value to 0 we obtain the correct message image.
*/
#include <stdio.h>

#include <opencv2\opencv.hpp>
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;


// Variables
Mat  grayscaleCarrierImage; // Grayscale image
string image;

Mat encodedImage; // Encoded image with the hidden message
string hiddenMessage;

Mat decodedImage; // Message image 
string message;

Mat shufflDecodedImage; // Shuffle back the decoded image thaks to the password

string password;

// Assume that both the carrier and message images are the same dimension
int C; // Number of columns
int R; // Number of rows

// For generate a same random integer with a same seed
unsigned long seed;

// Access to the first pixel of the carrier and the encoded image.
int pixelCarrier;
int pixelEncoded;

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
	cout << "You are in the decoder program. This program will decode a message from a grayscale image thanks to a password. " << endl;

	// Instructions to follows
	cout << "Chose a Grayscale Carrier image in ImageSrc folder (add also the folder if it's necessary): " << endl;
	cin >> image;
	cout << "Chose the encoded grayscale image in ImageSrc folder (add also the folder if it's necessary): " << endl;
	cin >> hiddenMessage;
	cout << "Select the password used to hidde this message : " << endl;
	cin >> password;
	cout << "Chose the name of the decoded image (This image will create in the ImageSrc folder):  " << endl;
	cin >> message;

	// Change the password to 64 bit integer using the hash function
	seed = Hash(password.c_str());

	// Load the carrier image
	grayscaleCarrierImage = imread("ImageSrc/" + image, CV_LOAD_IMAGE_GRAYSCALE);
	if (grayscaleCarrierImage.empty())
	{
		cout << "Sorry, the image " << image << "  does not exist." << endl;
		return -1;
	}
	imshow("Grayscale Carrier", grayscaleCarrierImage);

	// Load the encded image
	encodedImage = imread("ImageSrc/" + hiddenMessage, CV_LOAD_IMAGE_GRAYSCALE);
	if (encodedImage.empty())
	{
		cout << "Sorry, the image " << hiddenMessage << "  does not exist." << endl;
		return -1;
	}
	imshow("Encoded image", encodedImage);

	// Resolution of the image
	C = encodedImage.cols;
	R = encodedImage.rows;

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

	// Decoded image
	
	// First, decoding the message
	decodedImage = encodedImage - grayscaleCarrierImage;
	/*
	Next, clonning the grayscaleCarrier image in order to
	have the same dimension for the shuffleDecodedImage
	*/
	shufflDecodedImage = grayscaleCarrierImage.clone();
	// Then, shuffling back the decoded image in order to obtain the hidden message
	for (int i = 0; i < R; i++)
	{
		for (int j = 0; j < C; j++)
		{
			shufflDecodedImage.at<uchar>(randomRow[i], randomColumn[j]) = decodedImage.at<uchar>(i, j);
		}
	}
	// Now, changing 1 pixel value to 0 and other in 255 in order to obtain the message
	for (int i = 0; i < R; i++)
	{
		for (int j = 0; j < C; j++)
		{
			if (shufflDecodedImage.at<uchar>(i, j) == 1)
			{
				shufflDecodedImage.at<uchar>(i, j) = 0;
			}
			else
			{
				shufflDecodedImage.at<uchar>(i, j) = 255;
			}
		}
	}
	imshow("Message ", shufflDecodedImage);
	
	// Writeout the encoded image in a PNG file
	params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	params.push_back(95);
	imwrite("ImageSrc/" + message, shufflDecodedImage, params);

	waitKey(0);
	return 0;
}
// The image "encoded.png" is coded with the password: test