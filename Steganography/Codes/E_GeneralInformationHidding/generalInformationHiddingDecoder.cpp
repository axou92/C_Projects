/*
This program decode data message (pdf, doc, txt, png) in a colour image with or without password and with or without noise (choice of the user).
In case where noise is adding, use the gaussian noise with the password for adding it to the carrier image.
Subracting the encoded image to the carrier image (with or without noise) to obtain the binary message.
If password is used, shuffling back the binary message thanks to the password.
Conerting the binary message in bytes to obtain the hidden data message.
This program recover the format of the message.
*/
#include <stdio.h>
#include <opencv2\opencv.hpp>
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

// Variables
int model;
Mat  carrierImage; // Carrier image
string image;

// resolution of the image 
int C;
int R;

Mat encodedImage; // Encoded image with the hidden message
string hiddenMessage;
int pixelEncoded;

string password;
int standardDeviation;

// For generate a same random integer with a same seed
unsigned long seed;

ofstream decodedMessage;
string decodedFile;

vector<int> shuffleBinaryBuffer;// Random binary buffer of he file 
vector<int> binaryBuffer; // Binary buffer of the file 
vector<int> decodedBuffer; // decoded buffer in bytes

string extension; // format of the decoded file

// Carrier image with Gausian noise 
Mat noisedImage;
int noisePixel;

vector <unsigned char> message;


// Thanks to http://www.cse.yorku.ca/~oz/hash.html we have the following function:
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
	cout << "Chose the model: (0 for not use a password, 1 for use it, 2 for adding noise)" << endl;
	cin >> model;
	cout << "Chose the colour Carrier reference image in ImageSrc/partE/carriers folder (add also the folder if it's necessary): " << endl;
	cin >> image;
	cout << "Chose the encoded image in ImageSrc/parE folder (add also the folder if it's necessary): " << endl;
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
	cout << "Chose the name of the decoded file (This file will create in the ImageSrc/partE folder):  " << endl;
	cin >> decodedFile;

	// Load the carrier image
	carrierImage = imread("ImageSrc/partE/carriers/" + image + ".png", CV_LOAD_IMAGE_UNCHANGED);
	if (carrierImage.empty())
	{
		cout << "Sorry, the image "<< image <<" does not exist." << endl;
		return -1;
	}
	// Resolution of the image 
	C = carrierImage.cols;
	R = carrierImage.rows;
	
	// Load the encded image
	encodedImage = imread("ImageSrc/partE/" + hiddenMessage + ".png", CV_LOAD_IMAGE_UNCHANGED);
	if (encodedImage.empty())
	{
		cout << "Sorry, the image " << hiddenMessage <<" does not exist." << endl;
		return -1;
	}

	// Password

	// Change the password to 64 bit integer using the hash function
	seed = Hash(password.c_str());
	// Generating a random integer from a given seed (given by the password)
	RNG randomNumberGenerator1(seed);
	
	// clonning the carrier image into the noise image. Hence, if noise is not used, noisedImage = carrierImage. 
	noisedImage = carrierImage.clone();
	cout << randomNumberGenerator1(seed) << endl;
	if (model == 2)
	{
		// Adding noise at the carrier image
		for (int i = 0; i < R; i++)
		{
			for (int j = 0; j < C; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					// Noise given by the password and the standard deviation
					noisePixel = carrierImage.at<Vec3b>(i, j)[k] + randomNumberGenerator1.gaussian(standardDeviation);
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
	}

	// case without password
	if (model == 0)
	{
		for (int i = 0; i < R; i++)
		{
			for (int j = 0; j < C; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					pixelEncoded = encodedImage.at<Vec3b>(i, j)[k] - carrierImage.at<Vec3b>(i, j)[k];
					if (pixelEncoded == 0 || pixelEncoded == 1)
					{
						binaryBuffer.push_back(pixelEncoded);
					}
					else if (pixelEncoded == -1)
					{
						binaryBuffer.push_back(1);
					}
				}
			}
		}
	}

	// case with password for recover the order
	else 
	{
		// First, finding the shuffle binary buffer
		for (int i = 0; i < R; i++)
		{
			for (int j = 0; j < C; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					pixelEncoded = encodedImage.at<Vec3b>(i, j)[k] - noisedImage.at<Vec3b>(i, j)[k];
					if (pixelEncoded == 0 || pixelEncoded == 1)
					{
						shuffleBinaryBuffer.push_back(pixelEncoded);
					}
					else if (pixelEncoded == -1)
					{
						shuffleBinaryBuffer.push_back(1);
					}
				}
			}
		}

		// Array of the size of the binary buffer
		vector<int> randomBinaryBuffer(shuffleBinaryBuffer.size());
		for (int i = 0; i < shuffleBinaryBuffer.size(); i++)
		{
			randomBinaryBuffer.at(i) = i;
		}
		RNG randomNumberGenerator2(seed);
		cout << randomNumberGenerator2(seed) << endl;
		// Shuffle indexes of the binnary buffer array thanks to the password
		random_shuffle(randomBinaryBuffer.begin(), randomBinaryBuffer.end(), randomNumberGenerator2);

		// Then, shuffling back this message
		binaryBuffer = shuffleBinaryBuffer;
		for (int i = 0; i < shuffleBinaryBuffer.size(); i++)
		{
			binaryBuffer.at(randomBinaryBuffer[i]) = shuffleBinaryBuffer.at(i);
		}
	}

	// convert bytes in bits
	int sum = 0; 
	for (int i = 0; i < (binaryBuffer.size() / 8); i++)
	{
		for (int j = 0; j < 8; j++)
		{
			sum = sum + binaryBuffer[j + 8 * i] * pow(2, 7 - j);
		}
		decodedBuffer.push_back(sum);
		sum = 0;
	}

	// Export the message in an unsigned char vector 
	for (int i = 0; i < 10; i++)
	{
		message.push_back(decodedBuffer[i]);
	}

	// Find extension
	if (message[1] == 'P' && message[2] == 'N' && message[3] == 'G')
	{
		extension = ".png";
	}
	else if (message[1] == 'P' && message[2] == 'D' && message[3] == 'F')
	{
		extension = ".pdf";
	}
	else if (message[1] && message[2] && message[3] < 65 || message[1] && message[2] && message[3] > 122)
	{
		extension = ".doc";
	}
	else
	{
		extension = ".txt";
	}

	cout << "format: " << extension << endl;

	for (int i = 0; i < 10; i++)
	{
		cout << message[i] << endl;
	}
	
	// write the message
	message.clear();
	decodedMessage.open("ImageSrc/partE/" + decodedFile + extension, ios::binary);
	for (int i = 0; i < decodedBuffer.size(); i++)
	{
		message.push_back(decodedBuffer[i]);
		decodedMessage << message[i];
	}
	cout << "Message: " << message.size() << endl;

	cout << "Done" << endl;
	waitKey(0);
	return 0;
}


/*
Images are encoded with the password "test" and with a standard deviation of 20
*/