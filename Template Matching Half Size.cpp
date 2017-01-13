#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <Windows.h>
#define RATIO 0.5

using namespace std;
using namespace cv;

int main()
{
	int ROI_x = 0;
	int ROI_y = 0;
	int ROI_width = 0;
	int ROI_height = 0;
	int frameNum = 0;
	int matching_x = 0;
	int matching_y = 0;
	int alpha, alpha2;
	int center_x, center_y;

	vector<int> lst_value;
	vector<int> lst_value_after;
	int angle;
	/*
		Morning A1 : 163
		Morning B1 : 227
		Afternoon A1 : 162
		Afternoon B1 : 159
		EveningA1 : 290
		EveningB1 : 200
	*/
	int frameCount = 227;
	int startFrame = 164;
	VideoWriter video("EveningB1.wmv", CV_FOURCC('W', 'M', 'V', '2'), 5, Size(1280, 720), true);
	for (int i = startFrame; i <= frameCount; i++)
	{
		frameNum = i;
		cout << frameNum << endl;

		Mat temp_image, input_image;
		Mat img_ROI, result;
		Mat new_template_image;
		Mat final_result;

		input_image = imread(to_string(frameNum) + ".jpg", CV_LOAD_IMAGE_UNCHANGED);
		input_image.copyTo(final_result);// for display result in the end
		resize(input_image, input_image, Size(), RATIO, RATIO, 2); // resize by half for better processing
		temp_image = imread("template_" + to_string(frameNum) + ".bmp", CV_LOAD_IMAGE_UNCHANGED);
		int template_x = temp_image.size().width;
		int template_y = temp_image.size().height;

		//Template matching
		//Define new ROI area
		if (frameNum == startFrame)
		{
			input_image.copyTo(img_ROI);
		}
		else
		{
			try	{
				int margin = 10;
				ROI_x = matching_x / (1/RATIO) - margin;
				ROI_y = matching_y / (1 / RATIO) - margin; //input image half size so the height must be divided by 2 for ROI

				ROI_width = template_x + margin*(1 / RATIO);
				ROI_height = template_y + margin*(1 / RATIO);


				Rect myROI(ROI_x, ROI_y, ROI_height, ROI_width);
				img_ROI = input_image(myROI);
			}
			catch (Exception ex)
			{
				cout << "Error ROI Image" << endl;		
				break;
			}
		}
		// Write ROI image
		imwrite("roi_" + to_string(frameNum) + ".bmp", img_ROI);
		/// Create the result matrix
		int result_cols = img_ROI.cols - temp_image.cols + 1;
		int result_rows = img_ROI.rows - temp_image.rows + 1;

		result.create(result_rows, result_cols, CV_32FC1);

		/// Do the Matching and Normalize
		//  method = CV_TM_CCOEFF (4)
		matchTemplate(img_ROI, temp_image, result, 4);
		normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

		/// Localizing the best match with minMaxLoc
		double minVal; double maxVal; Point minLoc; Point maxLoc;
		Point matchLoc;
		minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
		matchLoc = maxLoc;

		/// Show me what you got			
		if (frameNum == startFrame)
		{
			matching_x = matchLoc.x * (1 / RATIO);
			matching_y = matchLoc.y * (1 / RATIO);
		}
		else
		{	
			matching_x = (matchLoc.x + ROI_x) * (1 / RATIO);
			matching_y = (matchLoc.y + ROI_y) * (1 / RATIO);
		}
		center_x = matching_x + temp_image.size().width;
		center_y = matching_y + temp_image.size().height;

		//Print on console
		cout << "Location x : " << to_string(matchLoc.x) << endl;
		cout << "Location y : " << to_string(matchLoc.y) << endl;
		cout << "matching x : " << to_string(matching_x) << endl;
		cout << "matching y : " << to_string(matching_y) << endl;
		cout << "center x : " << to_string(center_x) << endl;
		cout << "center y : " << to_string(center_y) << endl;	
				
		//Save New template image for next step
		Rect new_Roi_template(matching_x, matching_y, template_x*(1 / RATIO), template_y * (1 / RATIO));
		new_template_image = final_result(new_Roi_template);
		resize(new_template_image, new_template_image, Size(), RATIO, RATIO, 2);

		// write new template images for next step
		int new_distance = frameNum + 1;
		imwrite("template_" + to_string(new_distance) + ".bmp", new_template_image);

		//Add matched area in the results		
		/*putText(final_result, "Matched area", Point(matching_x - 10, matching_y - 10), 1, 1, Scalar(0, 255, 255), 1, 8, false);		
		putText(final_result, to_string(temp_image.cols) + "x" + to_string(temp_image.rows), Point(0, temp_image.rows + 10), 1, 1, Scalar(0, 0, 255), 1, 8, false);
		putText(img_ROI, "ROI " + to_string(img_ROI.cols) + "x" + to_string(img_ROI.rows), Point(matchLoc.x - 20, matchLoc.y - 5), 1, 1, Scalar(0, 0, 255), 1, 8, false);*/

		//add rectangle		
		rectangle(final_result, Point(matching_x, matching_y), Point(matching_x + temp_image.cols * (1 / RATIO), matching_y + temp_image.rows * (1 / RATIO)), Scalar(0, 255, 0), 2, 8, 0);
		rectangle(img_ROI, Point(matchLoc.x, matchLoc.y),Point(temp_image.size().width + matchLoc.x, temp_image.size().height + matchLoc.y), Scalar(0, 255, 0), 2, 8, 0);

		//add circle
		circle(img_ROI, Point(matchLoc.x, matchLoc.y), 5, Scalar(255, 0, 255), 2, 8, 0);
		circle(final_result, Point(matching_x, matching_y), 5, Scalar(255, 0, 255), 2, 8, 0);

		//Draw Crosschair
		line(final_result, Point(center_x, center_y - 15), Point(center_x, center_y + 15), Scalar(0, 0, 255), 2, 8, 0);
		line(final_result, Point(center_x - 15, center_y), Point(center_x + 15, center_y), Scalar(0, 0, 255), 2, 8, 0);
		circle(final_result, Point(center_x, center_y), 10, Scalar(0, 0, 255), 2, 8, 0);

		/*for (int i = 0; i < template_x; i++)
		{
			for (int j = 0; j < template_y; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					final_result.at<Vec3b>(i, j)[k] = temp_image.at<Vec3b>(i, j)[k];
				}
			}
		}
		if (frameNum >= 2)
		{
			for (int i = 0; i < img_ROI.size().width; i++)
			{
				for (int j = 0; j < img_ROI.size().height; j++)
				{
					for (int k = 0; k < 3; k++)
					{
						final_result.at<Vec3b>(i + template_x, j + template_y)[k] = img_ROI.at<Vec3b>(i, j)[k];
					}
				}
			}
		}*/

		imwrite("result_" + to_string(frameNum) + ".bmp", final_result); //write result images				
		video.write(final_result);		
		
		lst_value.clear();
		lst_value_after.clear();
		temp_image.release();
		input_image.release();
		img_ROI.release();
		result.release();
		new_template_image.release();
		final_result.release();
	}
	_getch();
	cv::waitKey(0);
	//video.release();
	return 0;
}