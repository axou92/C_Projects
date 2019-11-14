/*
This program encode data message (pdf, doc, txt, png) in a colour image with or without password and with or without noise (choice of the user).
In case where noise is adding, use the gaussian noise with the password for adding it.
Converting the data message in bytes to bits (with 1 and 0).
In case where password is used, shuffling the binary message thanks to this password.
Adding this binary message to the image in order to have the encoded image.
*/

#include <stdio.h>
#include <opencv2\opencv.hpp>
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;

// Variables

int model;
Mat carrierImage; // Carrier image
string image;

// resolution of the image 
int C;
int R;

ifstream message;
string file; // File of the message

vector<int> binaryBuffer; // Binary buffer of the file
vector<int> shuffleBinaryBuffer; // Random binary buffer of he file 

string password;
int standardDeviation;

// For generate a same random integer with a same seed
unsigned long seed;

// Carrier image with Gausian noise 
Mat noisedImage;
int noisePixel;

// Encode the carrier image with the message a password 
Mat encodedImage; 
string newImage;

vector<int> params;

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
	cout << "You are in the encoder program. This program will encode an image and a message thanks to a password. " << endl;
	
	// Instructions to follows
	cout << "Enter the extension only for the message (ie message6.png). " << endl;
	cout << "Chose the model: (0 for not use a password, 1 for use it, 2 for adding noise)" << endl;
	cin >> model;
	cout << "Chose the colour Carrier reference image in ImageSrc/partE/carriers folder (add also the folder if it's necessary): " << endl;
	cin >> image;
	cout << "Chose the message in ImageSrc/parE/messages folder (add also the folder if it's necessary): " << endl;
	cin >> file;
	if (model > 0)
	{
		cout << "Chose the string character password in order to encode this image : " << endl;
		cin >> password;
		if (model > 1)
		{
			cout << "Chose the standard deviation (integer) : " << endl;
			cin >> standardDeviation;
		}
	}
	cout << "Chose the name of the encoded image (This image will create in the ImageSrc/partE folder):  " << endl;
	cin >> newImage;

	// Load the carrier image
	carrierImage = imread("ImageSrc/partE/carriers/" + image + ".png", CV_LOAD_IMAGE_UNCHANGED);
	if (carrierImage.empty())
	{
		cout << "Sorry, the image " << image <<" does not exist." << endl;
		return -1;
	}
	imshow("Carrier", carrierImage);
	// Resolution of the image 
	C = carrierImage.cols;
	R = carrierImage.rows;

	// Load the message file (can load all kind of file)
	message.open("ImageSrc/partE/messages/" + file, ios::binary); // Open the file in lecture and binary
	if (!message)
	{
		cout << "Sorry, file "<< file << " not found." << endl;
		return -1;
	}
	// Recover all character in an unsigned buffer (vector of unsigned char)
	vector<unsigned char> buffer;
	while (message)
	{
		char caractere;
		message.get(caractere);
		buffer.push_back(caractere);
	}

	// convert bytes in bits
	vector<unsigned char> test;
	test = buffer;
	for (int i = 0; i < test.size(); i++)
	{
		for (int j = 7; j >= 0; j--)
		{
			int value = pow(2, j);
			if (test[i] % value != test[i])
			{
				binaryBuffer.push_back(1);
				test[i] = test[i] % value;
			}
			else
			{
				binaryBuffer.push_back(0);
			}
		}
	}

	// It is possible to encode this message ? 
	if (binaryBuffer.size() > C*R*3)
	{
		cout << "Sorry, impossible to encode the message because it's too long for this carrier image" << endl;
		return -1;
	}

	// Password

	// Array of the size of the binary buffer
	vector<int> randomBinaryBuffer(binaryBuffer.size());
	for (int i = 0; i < binaryBuffer.size(); i++)
	{
		randomBinaryBuffer.at(i) = i;
	}

	// Change the password to 64 bit integer using the hash function
	seed = Hash(password.c_str());
	// Generating a random integer from a given seed (given by the password)
	RNG randomNumberGenerator1(seed);
	cout << randomNumberGenerator1(seed) << endl;

	// Shuffle indexes of the binnary buffer array thanks to the password
	random_shuffle(randomBinaryBuffer.begin(), randomBinaryBuffer.end(), randomNumberGenerator1);

	// clonning the carrier image into the noise image. Hence, if noise is not used, noisedImage = carrierImage. 
	noisedImage = carrierImage.clone();

	// Generating a random integer from a given seed (given by the password)
	RNG randomNumberGenerator2(seed);
	cout << randomNumberGenerator2(seed) << endl;
	// Generating a noise image if the user want to add some noise into the image
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
					noisePixel = carrierImage.at<Vec3b>(i, j)[k] + randomNumberGenerator2.gaussian(standardDeviation);
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
	// encoded the image 

	// case without password
	if (model == 0)
	{
		// clonning the carrier image into the encoded image 
		encodedImage = carrierImage.clone();
		// Encoding the message with the shuffle binary message. If we arrive at the end of the essage we add 2 at each new pixels
		for (int i = 0; i < R; i++)
		{
			for (int j = 0; j < C; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					if (i*C * 3 + j * 3 + k < binaryBuffer.size())
					{
						if (encodedImage.at<Vec3b>(i, j)[k] != 255)
						{
							encodedImage.at<Vec3b>(i, j)[k] = encodedImage.at<Vec3b>(i, j)[k] + binaryBuffer[i*C * 3 + j * 3 + k];
						}
						// Preventing overflow
						else
						{
							if (binaryBuffer[i*C * 3 + j * 3 + k] == 0)
							{
								encodedImage.at<Vec3b>(i, j)[k] = 255;
							}
							else
							{
								encodedImage.at<Vec3b>(i, j)[k] = 254;
							}
						}
					}
					// If no message to encode
					else
					{
						// Preventing overflow 
						if (encodedImage.at<Vec3b>(i, j)[k] < 254)
						{
							encodedImage.at<Vec3b>(i, j)[k] = carrierImage.at<Vec3b>(i, j)[k] + 2;
						}
						else
						{
							encodedImage.at<Vec3b>(i, j)[k] = carrierImage.at<Vec3b>(i, j)[k] - 2;
						}
					}
				}		
			}
		}
	}

	// case with password for change the order
	else
	{
		// Clonning the carrier image into the encoded image 
		encodedImage = noisedImage.clone();
		// Clonning the binarryBuffer into the shuffle BinaryBuffer inorder to have the same dimension
		shuffleBinaryBuffer = binaryBuffer;

		// Shuffling the binary buffer
		for (int i = 0; i < randomBinaryBuffer.size(); i++)
		{
			shuffleBinaryBuffer.at(i) = binaryBuffer.at(randomBinaryBuffer[i]);
		}

		// Encoding the message with the shuffle binary message. If we arrive at the end of the essage we add 2 at each new pixels
		for (int i = 0; i < R; i++)
		{
			for (int j = 0; j < C; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					if (i*C * 3 + j * 3 + k < shuffleBinaryBuffer.size())
					{
						if (encodedImage.at<Vec3b>(i, j)[k] != 255)
						{
							encodedImage.at<Vec3b>(i, j)[k] = encodedImage.at<Vec3b>(i, j)[k] + shuffleBinaryBuffer[i*C * 3 + j * 3 + k];
						}
						// Preventing overflow
						else
						{
							if (shuffleBinaryBuffer[i*C * 3 + j * 3 + k] == 0)
							{
								encodedImage.at<Vec3b>(i, j)[k] = 255;
							}
							else
							{
								encodedImage.at<Vec3b>(i, j)[k] = 254;
							}
						}
					}
					// If no message to encode
					else
					{
						// Preventing overflow
						if (encodedImage.at<Vec3b>(i, j)[k] < 254)
						{
							encodedImage.at<Vec3b>(i, j)[k] = encodedImage.at<Vec3b>(i, j)[k] + 2;
						}
						else
						{
							encodedImage.at<Vec3b>(i, j)[k] = encodedImage.at<Vec3b>(i, j)[k] - 2;
						}
					}
				}
			}
		}
	}
	// imshow("Encoded image", encodedImage);

	// Writeout the encoded image in a PNG file
	params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	params.push_back(95);
	imwrite("ImageSrc/partE/" + newImage + ".png", encodedImage, params);
	cout << "Image encoded" << endl;

	waitKey(0);
	return 0;
}

/*
Images are encoded with the password "test" and with a standard deviation of 20
*/