/*
This program detect vehicles around a railways.
It detect if a vehicles is coming or leaving.
It can also detect if vehicles or pedestrians is on the track.
It also detect if the barrier is deploying and if a train is present on the track.
To conclude, this program can detect all events in the same time or if no event is present.
Please, notice that you have to modify the roi dimension if you want to use this program for another place
*/

#include <opencv2\opencv.hpp>
#include "opencv2/highgui/highgui.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace cv;
using namespace std;

/// Variables
int isVideo;
int speed; 

/// Reference Image 
Mat refImage;
Mat roiRef;

/// List of images  
vector<Mat> images, binaryImages, roiImages;

/// Video case
Mat videoFrame, roiFrame, grayFrame, binaryFrame;
int frameByframe = 0; /// Read the video frame by frame or not (0 for no and 1 for yes).

/// Find contours
vector<vector<Point>> contours;
vector<Vec4i> hierarchy;

/// Zones
vector<Point> zoneA, zoneB1, zoneB2, zoneC1, zoneC2;
int na, nb1, nb2, nc1, nc2; /// Size of areas.

/// Output Images
Mat objects;

/// Functions header
Mat RefImage(Mat refImage);
Mat ClearDetail(Mat binaryImage);
void DrawZone(Mat image);
Mat Detection(Mat image, Mat binaryImage);

/// Main function
int main(int argc, const char **argv)
{
	roiRef = RefImage(refImage);
	Mat refGray(roiRef.size(), CV_LOAD_IMAGE_UNCHANGED);
	cvtColor(roiRef, refGray, CV_BGR2GRAY);

	/// Second instruction (0 for image and 1 for video)
	isVideo = atof(argv[1]);

	if (isVideo == 0)
	{
		/// Get all images 
		vector<String> imageList;
		glob(argv[2], imageList, false);
		int Im = imageList.size(); /// number of png files in images folder

		///  ------------ Load all images and convert it in a binary images ------------
		std::cout << "Loading images...     ";
		for (int i = 0; i < Im; i++)
		{
			/// Name of all images. 
			images.push_back(imread(imageList[i]));
			
			/// ROI image.
			Rect Rec(25, 55, 668, 407);
			rectangle(images[i], Rec, Scalar(0), 0, 0, 0);
			roiImages.push_back(images[i](Rec));

			/// Difference between the current gray image and the reference image.
			Mat grayImage(roiImages[i].size(), CV_LOAD_IMAGE_UNCHANGED);
			cvtColor(roiImages[i], grayImage, CV_BGR2GRAY);
			binaryImages.push_back(grayImage);
			absdiff(grayImage, refGray, binaryImages[i]);
			threshold(binaryImages[i], binaryImages[i], 75, 255, THRESH_BINARY);

			/// Add Gaussian blur for accentuate white lines details.
			GaussianBlur(binaryImages[i], binaryImages[i], Size(5, 5), 0);

			/// Clear small details.
			binaryImages[i] = ClearDetail(binaryImages[i]);

			/// Add more white in the binary image Thanks to https://docs.opencv.org/2.4/doc/tutorials/imgproc/imgtrans/hough_lines/hough_lines.html .
			vector<Vec4i> lines;
			HoughLinesP(binaryImages[i], lines, 1, CV_PI / 180, 50, 50, 10);
			for (size_t j = 0; j < lines.size(); j++)
			{
				Vec4i l = lines[j];
				line(binaryImages[i], Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 255, 255), 3, CV_AA);
			}

		}
		std::cout << "  Done" << std::endl;
		std::cout << "======================================" << std::endl;
		
		///  -------------------------------------------------------------------------------------------------

		/// Display the result.
		for (int i = 0; i < Im; i++)
		{
			int key = cvWaitKey(33);
			if (key == 27)
			{
				break;
			}

			else
			{
				std::cout << "File: " << imageList[i] << std::endl;
				/// Draw zones.
				DrawZone(roiImages[i]);
				/// Detect objects.
				objects = Detection(roiImages[i], binaryImages[i]);
				imshow("Output image", objects);

				// waitKey(50); To display quickly the result

				/// 1 second between each image for see the result.
				waitKey(1000); 
			}
		}
		std::cout << "End of images " << std::endl;
	}
	else if (isVideo == 1)
	{
		VideoCapture capVideo;
		capVideo.open(argv[2]);
		if (!capVideo.isOpened())
		{
			std::cout << "ERROR ! Video file not found ! " << std::endl;
			return 1;
		}

		std::cout << "Video is open " << std::endl;
		
		std::cout << "Do you want to read this video frame by frame ? (0 for no and 1 for yes)" << std::endl;
		cin >> frameByframe;
		if (frameByframe == 0)
		{
			int fps = capVideo.get(CV_CAP_PROP_FPS);
			// std::cout << "Frames per second using video.get(CV_CAP_PROP_FPS) : " << fps << std::endl;   Max fps
			std::cout << "Enter the number of FPS that you want less than " << fps + 1 << endl;
			cin >> speed;
		}

		else
		{
			speed = capVideo.get(CV_CAP_PROP_FPS);
		}

		std::cout << " File: " << argv[2] << endl;
		std::cout << "======================================" << endl;
		while (capVideo.isOpened())
		{
			int key = cvWaitKey(33);
			if (key == 27)
			{
				break;
			}

			else
			{
				///  ------- Load video and convert it in a binary image. --------------------------------
				if ((capVideo.get(CV_CAP_PROP_POS_FRAMES) + 1) < capVideo.get(CV_CAP_PROP_FRAME_COUNT))
				{
					capVideo.read(videoFrame);
				}
				else
				{
					std::cout << "End of video" << std::endl;
					break;
				}
				/// ROI image. 
				roiFrame = videoFrame.clone();
				Rect Rec(25, 55, 668, 407);
				rectangle(roiFrame, Rec, Scalar(0), 0, 0, 0);
				roiFrame = roiFrame(Rec);

				/// Difference between the current gray image and the reference image.
				grayFrame = roiFrame.clone();
				binaryFrame = roiFrame.clone();
				cvtColor(roiFrame, grayFrame, CV_BGR2GRAY);

				absdiff(grayFrame, refGray, binaryFrame);
				threshold(binaryFrame, binaryFrame, 75, 255, THRESH_BINARY);
				GaussianBlur(binaryFrame, binaryFrame, Size(5, 5), 0);

				/// Clear small details.
				binaryFrame = ClearDetail(binaryFrame);

				/// Add more white in the binary imageThanks to https://docs.opencv.org/2.4/doc/tutorials/imgproc/imgtrans/hough_lines/hough_lines.html .
				vector<Vec4i> lines;
				HoughLinesP(binaryFrame, lines, 1, CV_PI / 180, 50, 50, 10);
				for (size_t j = 0; j < lines.size(); j++)
				{
					Vec4i l = lines[j];
					line(binaryFrame, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 255, 255), 3, CV_AA);
				}

				/// ----------------------------------------------------------------

				///  Draw zones
				DrawZone(roiFrame);

				/// Detect objects
				objects = Detection(roiFrame, binaryFrame);
				imshow("Output Video", objects);
				
				if (frameByframe == 1)
				{
					waitKey(0);
				}
				else
				{
					waitKey(1000 / speed);
				}
			}
		}
	}
	return 0;
}

/// Create an image of reference.
Mat RefImage(Mat refImage)
{
	/// Load the source reference image.
	refImage = imread("Images/RefImage.png", CV_LOAD_IMAGE_UNCHANGED);
	/// Create a ROI of the reference image.
	Rect Rec(25, 55, 668, 407);
	rectangle(refImage, Rec, Scalar(0, 0, 0), 1, 0, 0);
	roiRef = refImage(Rec);
	return roiRef;
}

/// Clear details in binary image. 
Mat ClearDetail(Mat binaryImage)
{
	/// Create a Matrice to show the result.
	Mat drawing = binaryImage.clone();

	/// Find contours in the image.
	findContours(drawing, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);

	/// Iterate through each contour.
	for (int i = 0; i < contours.size(); i++)
	{
		///  Find the area of contour.
		double area = contourArea(contours[i], false);
		/// If area less than 300 pixel, adding black for delete it.
		if (area < 300)
		{
			drawContours(drawing, contours, i, Scalar(0, 0, 0), -1); /// (-1 to fill inside of the contour)
		}
	}
	return drawing;
}

/// Draw the differents zone in the image thanks to a polygon (as a vector of points).
void DrawZone(Mat image)
{
	/// Clear each zone.
	zoneA.clear();
	zoneB1.clear();
	zoneB2.clear();
	zoneC1.clear();
	zoneC2.clear();

	/// A area (On track).
	zoneA.push_back(Point(0, 396));
	zoneA.push_back(Point(668, 26));
	zoneA.push_back(Point(668, 144));
	zoneA.push_back(Point(275, 407));
	zoneA.push_back(Point(0, 407));
	zoneA.push_back(Point(0, 396));

	const Point *ptsa = (const Point*)Mat(zoneA).data;
	int nptsa = Mat(zoneA).rows;

	/// B area (Entering).
	zoneB1.push_back(Point(0, 0));
	zoneB1.push_back(Point(0, 150));
	zoneB1.push_back(Point(180, 295));
	zoneB1.push_back(Point(400, 173));
	zoneB1.push_back(Point(100, 0));
	zoneB1.push_back(Point(0, 0));

	zoneB2.push_back(Point(668, 145));
	zoneB2.push_back(Point(668, 329));
	zoneB2.push_back(Point(518, 245));
	zoneB2.push_back(Point(668, 145));

	const Point *ptsb1 = (const Point*)Mat(zoneB1).data;
	int nptsb1 = Mat(zoneB1).rows;
	const Point *ptsb2 = (const Point*)Mat(zoneB2).data;
	int nptsb2 = Mat(zoneB2).rows;

	/// C Area (Leaving).
	zoneC1.push_back(Point(101, 0));
	zoneC1.push_back(Point(401, 172));
	zoneC1.push_back(Point(540, 95));
	zoneC1.push_back(Point(275, 0));
	zoneC1.push_back(Point(91, 0));

	zoneC2.push_back(Point(276, 407));
	zoneC2.push_back(Point(668, 407));
	zoneC2.push_back(Point(668, 330));
	zoneC2.push_back(Point(517, 246));
	zoneC2.push_back(Point(276, 407));

	const Point *ptsc1 = (const Point*)Mat(zoneC1).data;
	int nptsc1 = Mat(zoneC1).rows;
	const Point *ptsc2 = (const Point*)Mat(zoneC2).data;
	int nptsc2 = Mat(zoneC2).rows;

	na = zoneA.size();
	nb1 = zoneB1.size();
	nb2 = zoneB2.size();
	nc1 = zoneC1.size();
	nc2 = zoneC2.size();

	/// Draw polygons in the image
	polylines(image, &ptsa, &nptsa, 1, true, Scalar(0, 0, 255), 1, CV_AA, 0);
	polylines(image, &ptsb1, &nptsb1, 1, true, Scalar(255, 0, 0), 1, CV_AA, 0);
	polylines(image, &ptsb2, &nptsb2, 1, true, Scalar(255, 0, 0), 1, CV_AA, 0);
	polylines(image, &ptsc1, &nptsc1, 1, true, Scalar(0, 255, 0), 1, CV_AA, 0);
	polylines(image, &ptsc2, &nptsc2, 1, true, Scalar(0, 255, 0), 1, CV_AA, 0);
}

/// Detection of all events.
Mat Detection(Mat image, Mat binaryImage)
{
	/// Events variables.
	int entering = 0;
	int leaving = 0;
	int ontrack = 0;
	int train = 0;
	int barrier = 0;

	/// Output images.
	Mat drawing = image.clone();

	/// Rectangle for showing what is detected.
	Rect bounding_rect;
	vector<Rect> selectedRectangle;

	/// Find contours in the image.
	findContours(binaryImage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);

	/// Iterate through each contour.
	for (int i = 0; i < contours.size(); i++)
	{
		///  Find the area of contour.
		double area = contourArea(contours[i], false);
		/// Detect only area with more than 500 pixels and ceate a bouding box around.
		if (area > 500)
		{
			drawContours(binaryImage, contours, i, Scalar(255, 255, 255), -1); // -1 for fill inside of the contour
			bounding_rect = boundingRect(contours[i]);
			selectedRectangle.push_back(boundingRect(contours[i]));
		}
	}

	/// Merge rectangles wich have common area. 
	vector<Rect> maximumRectangle;
	bool max = false;
	Rect maxRect;
	vector<Rect> myRect;

	while (max == false)
	{
		/// If at the end test is already equal to 0, it is not possible to merge more rectangle.
		int test = 0; 

		/// Compare 2 by 2 rectangles box.
		for (int i = 0; i < selectedRectangle.size(); i++)
		{
			for (int j = 0; j < selectedRectangle.size(); j++)
			{
				/// Find the minimum area between both of them.
				double areaMax = 0.0;
				double areaMin = 0.0;
				if (selectedRectangle[i].area() > selectedRectangle[j].area())
				{
					areaMax = selectedRectangle[i].area();
					areaMin = selectedRectangle[j].area();
				}
				else
				{
					areaMax = selectedRectangle[j].area();
					areaMin = selectedRectangle[i].area();
				}

				if (i != j)
				{
					/// Merge rectangle which have more than 50% of their area in common.
					if ((selectedRectangle[i] & selectedRectangle[j]).area() > 0.5*areaMin)
					{
						maxRect = (selectedRectangle[i] | selectedRectangle[j]);
						selectedRectangle.erase(selectedRectangle.begin() + j);
						selectedRectangle.push_back(maxRect);
						if (selectedRectangle[i].area() == selectedRectangle[j].area())
						{
							/// Erase one of these rectangles and add an empty rectangle in order to have the same size.
							selectedRectangle.erase(selectedRectangle.begin() + i);
							selectedRectangle.push_back(Rect());
						}
						/// If it is merging, test is different to 0 because it's necesary to verify if the new rectangle is not merge with another rectangle.
						test++;
					}
				}
			}
		}

		/// If no more merge is possible.
		if (test == 0)
		{
			for (int k = 0; k < selectedRectangle.size(); k++)
			{
				/// If the rectangle is not empty, draw it and add it to myRect in order to know what the rectangle is.
				if (selectedRectangle[k] != Rect())
				{
					myRect.push_back(selectedRectangle[k]);
					rectangle(drawing, selectedRectangle[k], Scalar(0, 0, 255), 1, 8, 0);
				}
			}
			/// Leave the While loop because all rectangle are merge.
			max = true;
		}
	}

	/// Find centers of rectangles to see the position.
	vector<Point> centers(myRect.size());
	for (int i = 0; i < myRect.size(); i++)
	{
		centers[i].x = (myRect[i].x + myRect[i].x + myRect[i].width) / 2;
		centers[i].y = (myRect[i].y + myRect[i].y + myRect[i].height) / 2;

		/// If the area is represent by  more than 80000 pixels, it is a train.
		if (myRect[i].area() > 80000)
		{
			train =1;
		}
		/// Result for each zone.
		else
		{
			/// Enter event. 
			if ((pointPolygonTest(zoneB1, centers[i], true) > 0) || (pointPolygonTest(zoneB2, centers[i], true) > 0))
			{
				if (myRect[i].width > 250 && (myRect[i].width > 2 * myRect[i].height) && train != 1)
				{
					barrier = 1;
				}
				else
				{
					entering += 1;
				}

			}
			/// Leave event.
			if ((pointPolygonTest(zoneC1, centers[i], true) > 0) || (pointPolygonTest(zoneC2, centers[i], true) > 0))
			{
				leaving += 1;
			}
			/// Ontrack event. 
			if (pointPolygonTest(zoneA, centers[i], true) > 0)
			{
				ontrack += 1;
			}
		}

	}

	/* 
	If event is detect, it is displaying in the console. 
	Events are not displaying if no vehicles are detected in the area of the event 
	*/
	if (train > 0)
	{
		std::cout << "Train event !" << std::endl;
		leaving = 0; /// Because it is impossible to have a leaving event in a same time. 
	}

	/// In this case is not a barrier but a vehicle because the barrier is hidding by the train.
	if (barrier > 0 && train == 1)
	{
		entering++;
	}

	if (entering > 0) 
	{
		std::cout << "Entering event: " << entering << std::endl;
	}

	if (leaving > 0)
	{
		std::cout << "Leaving event: " << leaving << std::endl;
	}

	if (ontrack > 0 && train == 0)
	{
		std::cout << "Ontrack event: " << ontrack << std::endl;
	}

	if (barrier > 0 && train == 0 && ontrack == 0)
	{
		std::cout << "Safety barrier deployed !" << std::endl;
	}

	if (train == 0 && ontrack == 0 && leaving == 0 && entering == 0 && barrier == 0)
	{
		std::cout << "No event" << std::endl;
	}

	std::cout << "======================================" << std::endl;
	return drawing;
}