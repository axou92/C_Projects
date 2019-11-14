
This program was realised during my year in Cranfield (2018-2019). 
This file contain a description of each part and informations in order to use all steganography executables.
Some instructions are shown when the program is using, please follow these instructions.

------------------------------------------------------------ DESCRIPTION -------------------------------------------------------------

	Part A: Simple steganography 
This program encode a message image in a grayscale image.
If the message pixel is black, adding 1 in the encoded image at the same pixel in the grayscale image.
If the message pixel is white, adding 0 in the encoded image at the same pixel in the grayscale image.

	Part B: Scambling the signal
This program encode a message image in a grayscale image with a password.
Shuffuling the message image thanks to a password. 
If the shuffle message pixel is black, adding 1 in the encoded image at the same pixel in the grayscale image.
If the shuffle message pixel is white, adding 0 in the encoded image at the same pixel in the grayscale image.

	Part C: Generating noise images
This program generate a gaussian noise for both colour and grayscale image.
Selecting if a colour image is use.
Adding a password in order to add the gaussian noise in the image.
Adding the standard deviation in order to have the maxim (or minimum) value. 

	Part D: Extending to colour images
This program encode a message image in a colour image.
The user can use password and noise if he wants.
In case where noise is adding, use the gaussian noise with the password for adding it to the carrier image.
In case where password is used, shuffling the binary message thanks to this password.
If the (shuffle) message pixel is black, adding 1 in the encoded image at the same pixel in the grayscale image.
If the (shuffle) message pixel is white, adding 0 in the encoded image at the same pixel in the grayscale image.

	Part E: General information hidding
This program encode data message (pdf, doc, txt, png) in a colour image,
with or without password and with or without noise (choice of the user).
In case where noise is adding, use the gaussian noise with the password for adding it.
Converting the data message in bytes to bits (with 1 and 0).
In case where password is used, shuffling the binary message thanks to this password.
Adding this binary message to the image in order to have the encoded image.
This program can also recover the extension of the message.


--------------------------------------------------------------- USAGE ----------------------------------------------------------------

1) For the first four programs, all information is retrieved in the imageSrc folder. 
So if you want to recover information in another folder, please indicate when you select your image.
ie: carriers/Grayscale/grayscale_carrier-01.png if you want this grayscale image.

2) For the first four programs, you have to add the extension .png when you select an image. 

3) From the part C to E you have to chose what model you want to use. 
Be careful, you have to use the same model for encode and decode the image.

4) For the last part, you have to add the extension only for the hidden message.
All new informations (images encoded or decoded messages) is retrieved in the imageSrc/partE folder.
By default, the folder is imageSrc/partE/carriers for carrier images and imageSrc/partE/messages for messages.
For the examples, four differents carriers images are using with noised and password. 

5) For all examples, if you need it, the password is test and the standard deviation is 20. 


------------------------------------------------------------- IMPORTANT--------------------------------------------------------------

How use it if you want to use only the files:
	1) Create two folders, one for the code (files.cpp) and one for the executable.

	2) In the executable folder, create one folder ImageSrc (or change the destination in all cpp files).

	3) In the ImageSrc folder add images of you want to encode or decode with part A to part D executables.

	4) For the part E, create a partE folder and in this folder create two folders: carriers and messages.

	5) In the carriers folder add carriers images of you want to use for encoded or decode an image.

	6) In the messages folder add messages files of you want to hide in the image. 

	These file can be (.pdf, .doc, .png, .txt), other kind of files are not tested but you can test it.

	7) The decoder of part E can find the extension of the message. 
	If a problem happening when you try to decode, please delete " + extension" in the code and add try to add your extension.

	8) Exemples are available withe the following form encodedImageLetter and messageLetter (where letter indicate the part (A,B,D or E).
	For the part C exemples are noidesColourImage.png and noisedGrayscaleImage.png
	Exemples using colour_carrier.png and grayscale_carrier.png in references and message.png in hidden message form the part A to D.
	For the part E, carrier1(to 4).png are using in references and 
	message1.txt, message6.png, message7.doc and message8.pdf are respectively hidden in carriers 1 to 4. 