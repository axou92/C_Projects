/*
This program decode a message image in a colour image with or without password and with or without noise (choice of the user).
In case where noise is adding, use the gaussian noise with the password for adding it to the carrier image.
Subracting the encoded image to the carrier image (with or without noise) to obtain the binary message.
If password is used, shuffling back the binary message thanks to the password.
Hence, obtaining the message image.
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

Mat encodedImage; // Encoded image
string hiddenMessage;

Mat shuffleMessage; 

string password; // Password used to hidde the message
int standardDeviation;

Mat decodedImage; // Message image
string newImage;

// Assume that both the carrier and message images are the same dimension
int C; // Number of columns
int R; // Number of rows

// For generate a same random integer with a same seed
unsigned long seed;

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
	cout << "You are in the decoder program. This program will decode a message from a colour image with Gaussian noise thanks to a password. " << endl;

	// Instructions to follows
	cout << "Chose the model: (0 for not use a password, 1 for use it, 2 for adding noise)" << endl;
	cin >> model;
	cout << "Chose the colour Carrier reference image in ImageSrc folder (add also the folder if it's necessary): " << endl;
	cin >> image;
	cout << "Chose the encoded image in ImageSrc folder (add also the folder if it's necessary): " << endl;
	cin >> hiddenMessage;
	if (model > 0)
	{
		cout << "Chose the string character password in order to decode this image : " << endl;
		cin >> password;
		if (model > 1)
		{
			cout << "Chose the standard deviation (integer) : " << endl;
			cin >> standardDeviation;
		}
	}
	cout << "Chose the name of the decoded image (This image will create in the ImageSrc folder):  " << endl;
	cin >> newImage;
	
	// Load the carrier image
	colourCarrierImage = imread("ImageSrc/" + image, CV_LOAD_IMAGE_UNCHANGED);
	if (colourCarrierImage.empty())
	{
		cout << "Sorry, the image " << image << " does not exist." << endl;
		return -1;
	}
	imshow("Colour Carrier", colourCarrierImage);

	// Load the encoded image
	encodedImage = imread("ImageSrc/" + hiddenMessage, CV_LOAD_IMAGE_UNCHANGED);
	if (encodedImage.empty())
	{
		cout << "Sorry, the image  " << hiddenMessage << " does not exist." << endl;
		return -1;
	}
	imshow("Encoded image", encodedImage);

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


	// Decoded image

	// Case where password is not used
	if (model == 0)
	{
		// First, decoding the image 
		decodedImage = encodedImage - colourCarrierImage;

		// Then, recovering the message 
		for (int i = 0; i < R; i++)
		{
			for (int j = 0; j < C; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					if (decodedImage.at<Vec3b>(i, j)[k] == 1)
					{
						decodedImage.at<Vec3b>(i, j)[k] = 0;
					}
					else
					{
						decodedImage.at<Vec3b>(i, j)[k] = 255;
					}
				}
			}
		}
	}

	// Case where password is used
	else if (model == 1)
	{
		// First, clonning the Carrier image in order to have the same dimension for the decoded image, the shuffled message
		shuffleMessage = colourCarrierImage.clone();
		decodedImage = colourCarrierImage.clone();

		// Next, finding the shuffle message 
		for (int i = 0; i < R; i++)
		{
			for (int j = 0; j < C; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					shuffleMessage.at<Vec3b>(i, j)[k] = encodedImage.at<Vec3b>(i, j)[k] - colourCarrierImage.at<Vec3b>(i, j)[k];
				}
			}
		}
		
		// Then, shuffling bach this message
		for (int i = 0; i < R; i++)
		{
			for (int j = 0; j < C; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					decodedImage.at<Vec3b>(i, j)[k] = shuffleMessage.at<Vec3b>(randomRow[i], randomColumn[j])[randomColour[k]];
				}
			}
		}

		// Now, decoding the hidden message
		for (int i = 0; i < R; i++)
		{
			for (int j = 0; j < C; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					if (decodedImage.at<Vec3b>(i, j)[k] == 1)
					{
						decodedImage.at<Vec3b>(i, j)[k] = 0;
					}
					else
					{
						decodedImage.at<Vec3b>(i, j)[k] = 255;
					}
				}
			}
		}
	}

	// case where password is use and image have noise
	else if (model == 2)
	{
		// First, clonning the Carrier image in order to have the same dimension for the decoded image, the shuffled message and the noised image 
		shuffleMessage = colourCarrierImage.clone();
		noisedImage = colourCarrierImage.clone();
		decodedImage = colourCarrierImage.clone();

		// Next, adding noise at the carrier image 
		for (int i = 0; i < R; i++)
		{
			for (int j = 0; j < C; j++)
			{
				// First, adding the gaussian noise to the colour image 
				for (int k = 0; k < 3; k++)
				{
					noisePixel = colourCarrierImage.at<Vec3b>(i, j)[k] + randomNumberGenerator.gaussian(standardDeviation);
					if (noisePixel > 255)
					{
						noisePixel = 255;
					}
					else if (noisePixel < 0)
					{
						noisePixel = 0;
					}
					noisedImage.at<Vec3b>(i, j)[k] = noisePixel;
				}
			}
		}

		// Then, finding back the shuffle image and recovering it
		for (int i = 0; i < R; i++)
		{
			for (int j = 0; j < C; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					shuffleMessage.at<Vec3b>(i, j)[k] = encodedImage.at<Vec3b>(i, j)[k] - noisedImage.at<Vec3b>(i, j)[k];
				}
			}
		}
		for (int i = 0; i < R; i++)
		{
			for (int j = 0; j < C; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					decodedImage.at<Vec3b>(i, j)[k] = shuffleMessage.at<Vec3b>(randomRow[i], randomColumn[j])[randomColour[k]];
				}
			}
		}

		// Now, decoding the hidden message
		for (int i = 0; i < R; i++)
		{
			for (int j = 0; j < C; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					if (decodedImage.at<Vec3b>(i, j)[k] == 1)
					{
						decodedImage.at<Vec3b>(i, j)[k] = 0;
					}
					else
					{
						decodedImage.at<Vec3b>(i, j)[k] = 255;
					}
				}
			}
		}
	}
	
	imshow("Decoded image", decodedImage);

	// Writeout the noise image in a PNG file
	params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	params.push_back(95);
	imwrite("ImageSrc/" + newImage, decodedImage, params);

	waitKey(0);
	return 0;
}

/*
Image encoded with the password "test" and a standard deviation on 20
*/