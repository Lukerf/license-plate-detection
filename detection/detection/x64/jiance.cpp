#include <iostream>
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;
int main(){
	Mat srcImage=imread("./img/1.jpg");
	Mat srcShowImage;
	IplImage* image=0;
	RNG rng;
	srcImage.copyTo(srcShowImage);
	//imshow("a",srcImage);
	int i,j;
	int cPointB,cPointG,cPointR;
	for(i=1;i<srcImage.rows;i++)
		for(j=1;j<srcImage.cols;j++)
		{
			cPointB=srcImage.at<Vec3b>(i,j)[0];
			cPointG=srcImage.at<Vec3b>(i,j)[1];
			cPointR=srcImage.at<Vec3b>(i,j)[2];
			if(cPointB>80&cPointR<80&cPointG<80)    //��ȡ��ɫ��������������Ϊ��ɫ
			{
				srcImage.at<Vec3b>(i,j)[0]=0;
				srcImage.at<Vec3b>(i,j)[1]=0;
				srcImage.at<Vec3b>(i,j)[2]=0;
				continue;
			}
 
			else if(cPointB>200&cPointR>200&cPointG>200)  //��ȡ��ɫ����������Ϊ��ɫ
			{
				srcImage.at<Vec3b>(i,j)[0]=0;
				srcImage.at<Vec3b>(i,j)[1]=0;
				srcImage.at<Vec3b>(i,j)[2]=0;
				continue;
			}
 
			else
			{
				srcImage.at<Vec3b>(i,j)[0]=255;
				srcImage.at<Vec3b>(i,j)[1]=255;
				srcImage.at<Vec3b>(i,j)[2]=255;
				continue;
			}
 
		}

		//��ʾ��ȡ��ɫ���ͼƬ
		image = &IplImage(srcImage);
		cvShowImage("tupian",image);


		cvtColor(srcImage,srcImage, CV_BGR2GRAY);  //��srcImageת��Ϊ�Ҷ�ͼ
		threshold(srcImage,srcImage,127, 255,CV_THRESH_BINARY);   //��ͼ���ֵ��
		//ʹ�ò�ַ���ȥ������ص�����
		for(i=1;i<srcImage.rows;i++)
			for(j=1;j<srcImage.cols-1;j++)
			{
				srcImage.at<uchar>(i,j)=srcImage.at<uchar>(i,j+1)-srcImage.at<uchar>(i,j);
 
			}
 
			threshold(srcImage,srcImage,127, 255,CV_THRESH_BINARY_INV);//ͨ����ֵ���ķ�ʽ��ȡ����
			//erode(srcImage,srcImage,Mat(5,5,CV_8U),Point(-1,-1),2);  //��ʴ
			//	dilate(src,src,Mat(5,5,CV_8U),Point(-1,-1),2); //����
			//	morphologyEx(src,src,MORPH_OPEN,Mat(3,3,CV_8U),Point(-1,-1),1);   //������
			//	 morphologyEx(src,src,MORPH_CLOSE,Mat(3,3,CV_8U),Point(-1,-1),1);  //������
			erode(srcImage,srcImage,Mat(3,2,CV_8U),Point(-1,-1),5);
			threshold(srcImage,srcImage,127,255,CV_THRESH_BINARY_INV);
			imshow("a",srcImage);
			vector<vector<Point> > contours;
			vector<Vec4i> hierarchy;
			findContours(srcImage, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
			Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
			for( int i = 0; i < contours.size(); i++ )
			{  
				//ʹ�ñ߽��ķ�ʽ  
				CvRect aRect =  boundingRect(contours[i]);
				int tmparea=aRect.height*aRect.height;  
				if (((double)aRect.width/(double)aRect.height>2)&& ((double)aRect.width/(double)aRect.height<6)&& tmparea>=2000&&tmparea<=25000)  
				{  
					rectangle(srcShowImage,cvPoint(aRect.x,aRect.y),cvPoint(aRect.x+aRect.width ,aRect.y+aRect.height),color,2);  
					//cvDrawContours( dst, contours, color, color, -1, 1, 8 );  
				}  
			} 
 
}