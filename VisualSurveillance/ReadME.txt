------------------------------------------------------ DESCRIPTION -------------------------------------------------------

This program was realised during my year in Cranfield (2018-2019).
It allows to detect vehicles around a railway.
It detects if vehicles are coming or leaving.
It can also detect if vehicles or pedestrians are present on the track.
It also detects if the barrier is deployed and if a train is present on the track.
To conclude, this program can detect all events at the same time or if no event is present.

--------------------------------------------------------- USAGE ----------------------------------------------------------

To run the programme, go into the Debug folder and type following parameters in the command window:
	1) Executable the file
	2) 0 if you use images or 1 if use video
	3) Path of your files: 
		YourPath\VisualSurveillance\Application>VisualSurveillance.exe 0 Images/*.png
		YourPath\VisualSurveillance\Application>VisualSurveillance.exe 1 Images/levelcrossing.mpg
	4) Follow the instruction if you select the video
	5) Analyse the data with the console and the images
	6) Hold down the escape key on the image window to stop the animation

------------------------------------------------------ INFORMATION ------------------------------------------------------

Some part of the code is commented because the user has to add one more information.
Thanks to this information, it's possible to adjust the number of fps of the video or
to read the video frame by frame while waiting for a key to be pressed.
Hence the user can easily see what happen during the video.

------------------------------------------------------- IMPORTANT--------------------------------------------------------

If you want to use this program for another databe, you have to:
	1) Define your area 
	2) Modify the roi dimension

You can also use a real video to remove the noice provided by shadows.
In this case, the frame of reference can be the previous fram of the video.