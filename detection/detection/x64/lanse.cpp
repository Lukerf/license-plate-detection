#include<iostream>
#include"opencv2/opencv.hpp"

using namespace cv;
using namespace std;
int main()
{
	Mat img = imread("chepai.jpg");
	for(int i=1;i<img.rows;i++)
	{
		for(int j=1;j<img.cols;i++)
		{
			cout<<"("<<img.at<Vec3b>(i,j)[0]<<","<<img.at<Vec3b>(i,j)[1]<<","<<img.at<Vec3b>(i,j)[2£¬Âð]<<")"<<endl;
		}
		if(i>80)
			break;
	}
	return 0;
}

