#include<winsock2.h>
#include <iostream>
#include "opencv2/opencv.hpp"
#include"stdlib.h"
#include<cstring>
#include "windows.h"

#pragma comment(lib,"ws2_32.lib")  

using namespace cv;
using namespace std;
int c_detection(IplImage* frame);
void open_mv();
void output(IplImage* img);
char * ReadFile(char *pathpic,int &pic_len);
void sendPic(char * host, char* picname, int port);
char* Encode(char* pic,size_t* datalen);
void Decode(char* str,size_t datalen);
void test();
int main() {
	/*Mat img = imread("./img/test_3.jpg",1);
	IplImage* image = &IplImage(img);
	c_detection(image);
	output(image);*/
	test();
	//open_mv();
	return 0;
}
//���Ƽ��
int c_detection(IplImage* frame){
	int t = 0;
	//����ͼƬ
	if(!frame) return 0;
	//cvNamedWindow("frame",1);
	//cvShowImage("frame",frame);
	//��ֵ�˲�
	cvSmooth(frame,frame,CV_MEDIAN);
	//�Ҷ�ͼ
	IplImage * gray = cvCreateImage(cvGetSize(frame),frame->depth,1);
	cvCvtColor(frame,gray,CV_BGR2GRAY);
	//cvNamedWindow("gray",1);
	//cvShowImage("gray",gray);
	//��Ե���
	IplImage * temp = cvCreateImage(cvGetSize(gray),IPL_DEPTH_16S,1);
	//x�����ݶȣ���ֱ��Ե
	cvSobel(gray,temp,2,0,3);
	IplImage * sobel = cvCreateImage(cvGetSize(temp),IPL_DEPTH_8U,1);
	cvConvertScale(temp,sobel,1,0);
	//cvNamedWindow("sobel",1);
	//cvShowImage("sobel",sobel);
	//��ֵ��
	IplImage * threshold = cvCreateImage(cvGetSize(sobel),gray->depth,1);
	cvThreshold(sobel,threshold,0,255,CV_THRESH_BINARY|CV_THRESH_OTSU);
	//cvNamedWindow("threshold",1);
	//cvShowImage("threshold",threshold);
	//��̬ѧ�仯
	IplConvKernel * kernal;
	IplImage * morph = cvCreateImage(cvGetSize(threshold),threshold->depth,1);
	//�Զ���1��3�ĺ˽��С���������͸�ʴ
	kernal = cvCreateStructuringElementEx(3,1,1,0,CV_SHAPE_RECT);
	cvDilate(threshold,morph,kernal,2);
	cvErode(morph,morph,kernal,1);
	cvDilate(morph,morph,kernal,14);
	//�Զ���3x1�ĺ˽���y��������͸�ʴ
	kernal = cvCreateStructuringElementEx(1,3,0,1,CV_SHAPE_RECT);
	cvErode(morph,morph,kernal,4);
	cvDilate(morph,morph,kernal,10);
	//cvNamedWindow("erode",1);
	//cvShowImage("erode",morph);
	//�������
	IplImage * frame_draw = cvCreateImage(cvGetSize(frame), frame->depth, frame->nChannels);
	cvCopy(frame, frame_draw);
	CvMemStorage * storage = cvCreateMemStorage(0);  
	CvSeq * contour = 0;   
	int count = cvFindContours(morph, storage, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );   
	CvSeq * _contour = contour;   
	for( ; contour != 0; contour = contour->h_next )  
	{  		
		double tmparea = fabs(cvContourArea(contour));  		 
		CvRect aRect = cvBoundingRect( contour, 0 ); 
		if(tmparea > ((frame->height*frame->width)/10))   
		{  
			cvSeqRemove(contour,0); //ɾ�����С���趨ֵ������,1/10   
			continue;  
		} 
		if (aRect.width < (aRect.height*2))  
		{  
			cvSeqRemove(contour,0); //ɾ����߱���С���趨ֵ������   
			continue;  
		}
		if ((aRect.width/aRect.height) > 4 )
		{  
			cvSeqRemove(contour,0); //ɾ����߱���С���趨ֵ������   
			continue;  
		}
		if((aRect.height * aRect.width) < ((frame->height * frame->width)/100))
		{  
			cvSeqRemove(contour,0); //ɾ����߱���С���趨ֵ������   
			continue;  
		}
		CvScalar color = CV_RGB( 255, 0, 0); 
		cvDrawContours(frame_draw, contour, color, color, 0, 3, 8 );//�����ⲿ���ڲ�������
		t = 1;
	}
	
	//cvNamedWindow("����", 1);
	//cvShowImage("����", frame_draw);
	
	waitKey(100);
	if(t == 0)
	{
		return 0;
	}
	else{
		*frame = *frame_draw;
		return 1;
		
	}
	
	return 0;
	//�������
}
//����Ƶ������֡���
void open_mv(){
	int flag = 0;
	int fps = -1;
	IplImage* image=0;
	Mat frame;
	VideoCapture capture(0);//�����Ǵ�����ͷ��0�ǵ�һ����1�ǵڶ�������ͷ���Դ�
	int rate = capture.get(CV_CAP_PROP_FPS);
	//cout<<rate;
	while(capture.read(frame))
	{
		fps = (fps+1)%(rate/10);//ÿ��0.1���ȡһ֡
		image = &IplImage(frame);//Mat->IplImage*
		cvShowImage("test.avi",image);
		waitKey(30);
		if(fps==0){
		flag = c_detection(image);
		if(flag==0)
			{
				cvShowImage("���Ƽ��",image);
				waitKey(100);
				continue;
			}
		else
			output(image); //�˴���image���Ǽ�⵽�˳���ʱ��������ͼ
		}
	}
	return ;
}
void output(IplImage* img){
	cvShowImage("���Ƽ��",img);
	waitKey(100);
}
char * ReadFile(char *pathpic,int &pic_len){
        //��ͼƬ��ȡ����\	
        FILE *fp = fopen(pathpic, "rb");     //���ļ�
        if (!fp){
            return NULL;
        }
        fseek(fp, 0, SEEK_END);  //һֱѰ�ҵ��ļ�β��
        pic_len = ftell(fp);  //�õ�ͼƬ�ĳ���
        rewind(fp);  //rewind���ļ�ָ��ָ��ͷ
        char *pic_buf = new char[pic_len + 1];  //����һ���ռ��ڶ���
        memset(pic_buf, 0, pic_len + 1);  //����ļ�ָ��
        //��ȡ�ļ�����
        fread(pic_buf,sizeof(char),pic_len,fp);
        //���Խ��ļ��ٱ�����D:��
        /*
        MessageBoxA(NULL, "�ļ���ʼ", 0, 0);
        FILE *fpw = fopen("C:\\AA.jpg","wb");
        fwrite(pic_buf,sizeof(char), pic_len, fpw);
        fclose(fpw); //�ر��ļ���
        MessageBoxA(NULL, "�ļ�����", 0, 0);
        */
        fclose(fp);  
        return pic_buf;
    }
void sendPic(char * host, char* pathpic, int port) //host = 47.100.121.23
{
        WORD sockVersion = MAKEWORD(2,2);
		WSADATA wsd;
		if (WSAStartup(sockVersion,&wsd) != 0) //�첽�׽�����������
													//����socketʵ��ϸ����Ϣ
		{
            system("WSAStartup failed!\n");
            system("pause");
            return ;
		}
		SOCKET sHost = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//�����׽���
		//�󶨶˿ںţ�������Ҫ���ʵķ�����������ַ
		sockaddr_in servAddr; 
		servAddr.sin_family = AF_INET;
		servAddr.sin_addr.s_addr = inet_addr("10.161.7.142");
		servAddr.sin_port = htons(9999);
		int retVal = connect(sHost, (LPSOCKADDR)&servAddr, sizeof(servAddr));//���ӷ�����
        //�ȶ�ȡ�ļ���
        //ʵ��ͼƬ��ȡ,�ȼ�ͼƬ��ȡ
        int Dpic_len;
        char *Dpic_data=NULL;

        
        Dpic_data = ReadFile(pathpic, Dpic_len);
		
		printf("%d",Dpic_len);
		waitKey(5000);
        send(sHost, Dpic_data,Dpic_len, 0);
         char recvBuf[50];
		recv(sHost,recvBuf,50,0);
		printf("%s\n",recvBuf);
 
		closesocket(sHost);

		WSACleanup;
        //�ͷ��ڴ�
        delete Dpic_data;
    }

void test()
{
	WORD sockVersion = MAKEWORD(2,2);
		WSADATA wsd;
		if (WSAStartup(sockVersion,&wsd) != 0) //�첽�׽�����������
													//����socketʵ��ϸ����Ϣ
		{
            system("WSAStartup failed!\n");
            system("pause");
            return ;
		}
	
		SOCKET sHost = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//�����׽���
		//�󶨶˿ںţ�������Ҫ���ʵķ�����������ַ
		sockaddr_in servAddr; 
		servAddr.sin_family = AF_INET;
		servAddr.sin_addr.s_addr = inet_addr("10.160.38.160");
		servAddr.sin_port = htons(8888);
		int retVal = connect(sHost, (LPSOCKADDR)&servAddr, sizeof(servAddr));//���ӷ�����
        //�ȶ�ȡ�ļ���
        //ʵ��ͼƬ��ȡ,�ȼ�ͼƬ��ȡ
		static const char* protocolHead="GET /servlet/LisenceServlet?Lisence=123465 HTTP/1.1\r\nHost:10.160.38.160:8888\r\nConnection: close\r\n\r\n";
        send(sHost, protocolHead,strlen(protocolHead), 0);
		printf("%s","jj");
		/*char recvBuf[50];
		recv(sHost,recvBuf,50,0);
		printf("%s\n",recvBuf);
		waitKey(5000);*/
		closesocket(sHost);

		WSACleanup;
        //�ͷ��ڴ�
}



char* Encode(char* pic,size_t* datalen)
{
	Mat tstMat=imread(pic);
   // imshow("picture",tstMat);
    vector<unsigned char> inImage;
    imencode(".jpg",tstMat,inImage);
    *datalen=inImage.size();
    char *msgImage=new char[*datalen];
    for(int i=0;i<*datalen;i++)
    {
        msgImage[i]=inImage[i];
        cout<<msgImage[i]<<endl;
    }
	waitKey(5000);
	return msgImage;
}
void Decode(char* str,size_t datalen)
{
	vector<unsigned char> buff;
    for(int i=0;i<datalen;i++)
    {
           buff.push_back(str[i]);
    }
    Mat show=imdecode(buff,CV_LOAD_IMAGE_COLOR);
    imshow("picture",show);
	waitKey(5000);
}