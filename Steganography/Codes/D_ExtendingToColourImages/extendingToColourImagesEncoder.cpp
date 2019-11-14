/*
This program encode a message image in a colour image with or without password and with or without noise (choice of the user).
In case where noise is adding, use the gaussian noise with the password for adding it to the carrier image.
In case where password is used, shuffling the binary message thanks to this password.
If the (shuffle) message pixel is black, adding 1 at the same pixel in the grayscale image.
If the (shuffle) message pixel is white, adding 0 at the same pixel in the grayscale image.
Hence, obtaining a new encoded image.
*/

#include <stdio.h>

#include <opencv2\opencv.hpp>
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;

// Variables

int model; // 0 for not use a password, 1 for use it, 2 for adding noise
Mat  colourCarrierImage; // Carrier image
string image;

Mat noisedImage; // Noise image of the carrier images

Mat binaryImage; // Message image
string message;

Mat shuffleBinaryImage; // Shuffle the binary image thanks to a the password
 
Mat encodedImage; // Encode the carrier image with the message a password 
string newImage;

string password; // Password for hidde the message  
int standardDeviation;

// Assume that both the carrier and message images are the same dimension
int C; // Number of columns
int R; // Number of rows

// For generate a same random integer with a same seed
unsigned long seed;

// encoded pixel for the colour image

// encoded pixel for the message image 
int encodedPixel;

// Noise pixel for decoded the message
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
	cout << "You are in the encoder program. This program will encode a colour image and a message and add Gaussian noise thanks to a password. " << endl;

	// Instructions to follows
	cout << "Chose the model: (0 for not use a password, 1 for use it, 2 for adding noise)" << endl;
	cin >> model;
	cout << "Chose a colour Carrier image in ImageSrc folder (add also the folder if it's necessary): " << endl;
	cin >> image;
	cout << "Chose the hidden mesage in ImageSrc folder (add also the folder if it's necessary): " << endl;
	cin >> message;
	if (model > 0)
	{
		cout << "Chose a character string password in order to hidde this message : " << endl;
		cin >> password;
		if (model > 1)
		{
			cout << "Chose the standard deviation (integer) : " << endl;
			cin >> standardDeviation;
		}
	}
	cout << "Chose the name of the encoded image (This image will create in the ImageSrc folder):  " << endl;
	cin >> newImage;
	
	// Load the carrier image
	colourCarrierImage = imread("ImageSrc/" + image, CV_LOAD_IMAGE_UNCHANGED);
	if (colourCarrierImage.empty())
	{
		cout << "Sorry, the image " << image <<" does not exist." << endl;
		return -1;
	}
	imshow("Colour Carrier", colourCarrierImage);

	// Load the message image
	binaryImage = imread("ImageSrc/" + message, CV_LOAD_IMAGE_UNCHANGED);
	if (binaryImage.empty())
	{
		cout << "Sorry, the image  " << message << " does not exist." << endl;
		return -1;
	}
	imshow("Binary image", binaryImage);

	// Change the password to 64 bit integer using the hash function
	seed = Hash(password.c_str());

	// Resolution of the image 
	C = colourCarrierImage.cols;
	R = colourCarrierImage.rows;

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

	// Array for indcate the colour 
	vector<int> randomColour(3);
	for (int i = 0; i < randomColour.size(); i++)
	{
		randomColour.at(i) = i;
	}

	// Generating a random integer from a given seed (given by the password)
	RNG randomNumberGenerator(seed);

	// Shuffle indexes of the colomn, row and colour arrays thanks to the password
	random_shuffle(randomColumn.begin(), randomColumn.end(), randomNumberGenerator);
	random_shuffle(randomRow.begin(), randomRow.end(), randomNumberGenerator);
	random_shuffle(randomColour.begin(), randomColour.end(), randomNumberGenerator);

	// Encoded image

	// Case where password is not used
	if (model == 0)
	{
		// First, clonning the Carrier image in order to have the same dimension for the encoded image
		encodedImage = colourCarrierImage.clone();
		for (int i = 0; i < R; i++)
		{
			for (int j = 0; j < C; j++)
			{
				// Next, hidding the message
				if (binaryImage.at<uchar>(i, j) == 255)
				{
					encodedPixel = 0;
				}
				else
				{
					encodedPixel = 1;
				}
				// Then, encoding the image
				for (int k = 0; k < 3; k++)
				{
					if (encodedImage.at<Vec3b>(i, j)[k] != 255)
					{
						encodedImage.at<Vec3b>(i, j)[k] = colourCarrierImage.at<Vec3b>(i, j)[k] + encodedPixel;
					}
					// Preventing overflow
					else
					{
						encodedImage.at<Vec3b>(i, j)[k] = 255;
					}
				}
			}
		}
	}

	// Case where password is used
	else if (model == 1)
	{
		// First, clonning the Carrier image in order to have the same dimension for the encoded image and the shuffle binary image
		shuffleBinaryImage = colourCarrierImage.clone();
		encodedImage = colourCarrierImage.clone();

		// Next, hidding the message
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
		
		// Then, shuffling the message thanks to the password
		for (int i = 0; i < R; i++)
		{
			for (int j = 0; j < C; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					shuffleBinaryImage.at<Vec3b>(randomRow[i], randomColumn[j])[randomColour[k]] = binaryImage.at<uchar>(i, j);
				}
			}
		}

		// Now, encoding the image with the shuffle message 
		for (int i = 0; i < R; i++)
		{
			for (int j = 0; j < C; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					if (colourCarrierImage.at<Vec3b>(i, j)[k] != 255)
					{
						encodedImage.at<Vec3b>(i, j)[k] = colourCarrierImage.at<Vec3b>(i, j)[k] + shuffleBinaryImage.at<Vec3b>(i, j)[k];
					}
					// Preventing overflow
					else
					{
						encodedImage.at<Vec3b>(i, j)[k] = 255;
					}
				}
			}
		}
	}

	// Case where password is used and for adding noise
	else if (model == 2)
	{
		// First, clonning the Carrier image in order to have the same dimension for the encoded image, the shuffled message and the noised image 
		shuffleBinaryImage = colourCarrierImage.clone();
		noisedImage = colourCarrierImage.clone();
		encodedImage = colourCarrierImage.clone();

		// Next, adding noise at the carrier image 
		for (int i = 0; i < R; i++)
		{
			for (int j = 0; j < C; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					// Noise given by the password and the standard deviation
					noisePixel = colourCarrierImage.at<Vec3b>(i, j)[k] + randomNumberGenerator.gaussian(standardDeviation);
					// Noise can be positive or negative,
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
					noisedImage.at<Vec3b>(i, j)[k] = noisePixel;
				}
			}
		}

		// Then, hidding the message and shuffling it
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
		for (int i = 0; i < R; i++)
		{
			for (int j = 0; j < C; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					shuffleBinaryImage.at<Vec3b>(randomRow[i], randomColumn[j])[randomColour[k]] = binaryImage.at<uchar>(i, j);
				}
			}
		}

		// Now, encoding the image with the noised image and the shuffle image 
		for (int i = 0; i < R; i++)
		{
			for (int j = 0; j < C; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					if (encodedImage.at<Vec3b>(i, j)[k] != 255)
					{
						encodedImage.at<Vec3b>(i, j)[k] = noisedImage.at<Vec3b>(i, j)[k] + shuffleBinaryImage.at<Vec3b>(i, j)[k];
					}
					// Preventing overflow
					else
					{
						encodedImage.at<Vec3b>(i, j)[k] = 255;
					}
				}
			}
		}
	}
	imshow("Encoded image", encodedImage);

	// Writeout the noise image in a PNG file
	params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	params.push_back(95);
	imwrite("ImageSrc/" + newImage, encodedImage, params);

	waitKey(0);
	return 0;
}

/*
Image encoded with the password "test" and a standard deviation on 20
*/