#if USE_GAIN
#include <stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include<iostream>
#include <string.h>
#include<vector>
#include<opencv2/opencv.hpp>
#include"StlGlDefines.h"
#include"overLapRegion.h"
#include"GLRender.h"
#include"PanoCaptureGroup.h"
using namespace std;
using namespace cv;
#define GAIN_SAVE_IMG 0

#define LEFT_BLACK_LEN   0
#define RIGHT_BLACK_LEN  0
extern cv::Point2f RotatePoint(cv::Point2f Point,cv::Point2f rotate_center,float rotate_angle,float max_panel_length,int cam_count);
extern Render render;
extern int ExchangeChannel(int direction);
GLShaderManager *my_shaderm;
overLapRegion *overLapRegion::overlapregion;

int leftExchange(int idx)
{
//	return idx;
	int after=-1;
	switch(idx)
	{
	case 0:
		after=5;
		break;
	case 1:
		after=2;
		break;
	case 2:
		after=3;
		break;
	case 3:
		after=4;
		break;
	case 4:
		after=9;
		break;
	case 5:
		after=1;
		break;
	case 6:
		after=0;
		break;
	case 7:
		after=6;
		break;
	case 8:
		after=7;
		break;
	case 9:
		after=8;
		break;
	default :
		break;
	}

	return after;
}
int rightExchange(int idx)
{
//	return idx;
	int after=-1;
	switch(idx)
	{
	case 0:
		after=6;
		break;
	case 1:
		after=5;
		break;
	case 2:
		after=1;
		break;
	case 3:
		after=2;
		break;
	case 4:
		after=3;
		break;
	case 5:
		after=0;
		break;
	case 6:
		after=7;
		break;
	case 7:
		after=8;
		break;
	case 8:
		after=9;
		break;
	case 9:
		after=4;
		break;
	default :
		break;
	}
	return after;
}

overLapRegion::overLapRegion()
{
	CHANGE_GAIN=false;
	HEIGHT_SLICES=2;
	IsDealingWithSLICES=false;
	Src4=(unsigned char *)malloc(FPGA_SINGLE_PIC_W*FPGA_SINGLE_PIC_H*4*3);
	Src6=(unsigned char *)malloc(FPGA_SINGLE_PIC_W*FPGA_SINGLE_PIC_H*6*3);
	temp4=(unsigned char *)malloc(FPGA_SINGLE_PIC_W*FPGA_SINGLE_PIC_H*4*3);
	temp6=(unsigned char *)malloc(FPGA_SINGLE_PIC_W*FPGA_SINGLE_PIC_H*6*3);
}



overLapRegion *  overLapRegion::GetoverLapRegion()
{
	static bool once=false;
	if(!once){
		once = true;
		overlapregion=new overLapRegion();
	}
	return  overlapregion;
}

void  overLapRegion::set_change_gain(bool TOF)
{
	this->CHANGE_GAIN=TOF;
}

bool  overLapRegion::get_change_gain()
{
	return this->CHANGE_GAIN;
}

float Modula(float src,float divider)
{
	int count=src/divider;
	return src-count*divider;
}
#define CLAP(min, max , val)	(val<min)?(min):((val>max)?(max):(val))
void UYV2RGB(unsigned char *src, int width, int height, unsigned char * dst)
{
	int i, j;
	unsigned char *lpSrc, *lpDst;
	int Y, U, V, R, G, B;

	for(j=0; j<height; j++)
	{
		for(i=0; i<width; i++)
		{
			lpSrc = src + j*width*3 + 3*i;
			lpDst = dst + j*width*3 + 3*i;
			U = *lpSrc;Y = *(lpSrc+1);V = *(lpSrc+2);
		   B = (Y + 1.402 * (U - 128.0));
		   G = (Y - 0.344 * (V - 128.0) - 0.714 * (U - 128.0));
		   R = (Y + 1.772 * (V - 128.0));
			*lpDst = CLAP(0,255,B); *(lpDst+1) = CLAP(0,255,G); *(lpDst+2) = CLAP(0,255,R);
		}
	}
}

float overLapRegion::getPercentX(int camidx,int leftorright)
{
	return midX[camidx][leftorright]/640.0;
}
bool overLapRegion::van_save_coincidence()
{
	Point3d Isum1 = Point3d(0.0,0.0,0.0), Isum2 = Point3d(0.0,0.0,0.0);
	double gamma=0.25;
	vector<Point3d>	van_alpha(CAM_COUNT);
	vector<Mat> van_images(CAM_COUNT);
	vector<Mat> van_images_warped_f(CAM_COUNT);
	vector<Mat> Src_images(2);
	char buf[1024];
	int i;
#if USE_BMPCAP
	for(i=0; i<CAM_COUNT; i++){
		sprintf(buf,"./data/gain/%02d.bmp",i);
		van_images[i] = imread(buf);
		if (van_images[i].empty()){
			cout<<"Can't open gain/image " << buf<<"open other one"<<endl;
			sprintf(buf,"./data/%02d.bmp",i);
			van_images[i] = imread(buf);
		}
		else{
			strcpy(buf,"'\0'");
			van_images[i].convertTo(van_images_warped_f[i], CV_8U);
		}
	}
#else
	unsigned char *p6=PanoCaptureGroup::GetMainInstance()->GetSrc(FPGA_SIX_CN);
	if(p6)
	{
		memcpy(temp6,p6,
		FPGA_SINGLE_PIC_H*FPGA_SINGLE_PIC_W*6*3);
		UYV2RGB(temp6,FPGA_SINGLE_PIC_W,FPGA_SINGLE_PIC_H*6,Src6);
	}
	else
		return false;
	unsigned char *p4=PanoCaptureGroup::GetMainInstance()->GetSrc(FPGA_FOUR_CN);
	char buf_save[64]={0};
	if(p4)
	{
		memcpy(temp4,p4,
		FPGA_SINGLE_PIC_H*FPGA_SINGLE_PIC_W*4*3);
		UYV2RGB(temp4,FPGA_SINGLE_PIC_W,FPGA_SINGLE_PIC_H*4,Src4);
#if GAIN_SAVE_IMG
		Mat s4(FPGA_SINGLE_PIC_H*4,FPGA_SINGLE_PIC_W,CV_8UC3,Src4);
		imwrite("./data/save/p4.bmp",s4);
#endif
	}
	else
		return false;
	for(int i=0; i<6; i++){
	van_images[i].create(FPGA_SINGLE_PIC_H,FPGA_SINGLE_PIC_W,CV_8UC3);
	van_images[i].data=Src6+i*FPGA_SINGLE_PIC_H*FPGA_SINGLE_PIC_W*3;
	}
	for(int i=6; i<CAM_COUNT; i++){
	van_images[i].create(FPGA_SINGLE_PIC_H,FPGA_SINGLE_PIC_W,CV_8UC3);
	van_images[i].data=Src4+(i-6)*FPGA_SINGLE_PIC_H*FPGA_SINGLE_PIC_W*3;
#if GAIN_SAVE_IMG
	Mat s6(FPGA_SINGLE_PIC_H*6,FPGA_SINGLE_PIC_W,CV_8UC3,Src6);
	imwrite("./data/save/p6.bmp",s6);
#endif
	}
#endif
	static bool Once=true;
	if(0)
	{
		Once=false;
		for(int i=0;i<2;i++)
		{
			char buf[48]={0};
			sprintf(buf,"./data/pic%d.bmp",i);
				Src_images[i]= imread(buf);
		}
	}

	vector<cv::Point2f>::iterator it,it2;
	Vec3b van_pix_1, van_pix_2;

	cv::Point2f vPoint1, vPoint2;
	int direction;
	char buf_1[1024];
	char buf_2[1024];
	van_alpha[0].x  =van_alpha[0].y = van_alpha[0].z = 1.0;
	float x_min[CAM_COUNT]={},y_min[CAM_COUNT]={},x_max[CAM_COUNT]={},y_max[CAM_COUNT]={};
	float x2_min[CAM_COUNT]={},y2_min[CAM_COUNT]={},x2_max[CAM_COUNT]={},y2_max[CAM_COUNT]={};
int a=0,b=0,c=0,d=0;

float max_panel_length=render.GetPanoLen();
	for(direction=0;direction<CAM_COUNT;direction++)
	{
		float cmp_x_min=30000, cmp_y_min=30000,cmp_x_max=-1,cmp_y_max=-1;
		float cmp_x2_min=30000, cmp_y2_min=30000,cmp_x2_max=-1,cmp_y2_max=-1;
		for(it=m_pointLeft[direction].begin();it!= m_pointLeft[direction].end();it++)
		{
				if((*it).x>cmp_x_max)
					cmp_x_max=(*it).x;
				if((*it).y>cmp_y_max)
					cmp_y_max=(*it).y;
				if((*it).x<cmp_x_min)
					cmp_x_min=(*it).x;
				if((*it).y<cmp_y_min)
					cmp_y_min=(*it).y;
		}

		for(it2=m_pointRight[direction].begin();it2!= m_pointRight[direction].end();it2++)
		{
				if((*it2).x>cmp_x2_max)
					cmp_x2_max=(*it2).x;
				if((*it2).y>cmp_y2_max)
					cmp_y2_max=(*it2).y;
				if((*it2).x<cmp_x2_min)
					cmp_x2_min=(*it2).x;
				if((*it2).y<cmp_y2_min)
					cmp_y2_min=(*it2).y;
		}


		x_min[direction]= cmp_x_min;
		y_min[direction]= Modula(cmp_y_min,FPGA_SINGLE_PIC_H);
		x_max[direction]= cmp_x_max;
		y_max[direction]= Modula(cmp_y_max,FPGA_SINGLE_PIC_H);

		x2_min[direction]= cmp_x2_min;
		y2_min[direction]= Modula(cmp_y2_min,FPGA_SINGLE_PIC_H);
		x2_max[direction]= cmp_x2_max;
		y2_max[direction]= Modula(cmp_y2_max,FPGA_SINGLE_PIC_H);

	strcpy(buf_1,"'\0'");
	strcpy(buf_2,"'\0'");
	char buf_1[48];
	char buf_2[48];

	int cur_x_min=x_min[direction]+LEFT_BLACK_LEN,
			cur_y_min = y_min[direction],
			cur_x_max=x_max[direction],
			cur_y_max=y_max[direction]
			;
		cur_x_min=cur_x_min < 0? 0 : cur_x_min;
		cur_y_min = cur_y_min<0?0:cur_y_min;

		cur_x_max=cur_x_max>FPGA_SINGLE_PIC_W? FPGA_SINGLE_PIC_W:cur_x_max;
		cur_y_max=cur_y_max>FPGA_SINGLE_PIC_H? FPGA_SINGLE_PIC_H:cur_y_max;
	sprintf(buf_1,"./data/save/%d_left.bmp",direction);
	roi_image[direction][LEFT_ROI]=van_images[direction](
			Range(cur_y_min,cur_y_max),
			Range(cur_x_min,cur_x_max)////leftmid
			);
	midX[direction][LEFT_ROI]=(cur_x_max-cur_x_min)/2.0+cur_x_min;
#if GAIN_SAVE_IMG
	imwrite(buf_1,roi_image[direction][LEFT_ROI]);

	cv::Rect Roi_rect = cv::Rect(cur_x_min+50,cur_y_min,roi_image[direction][LEFT_ROI].cols,roi_image[direction][LEFT_ROI].rows);
//	roi_image[direction][LEFT_ROI].copyTo(van_images[direction](Roi_rect));
	cv::rectangle(van_images[direction],cvPoint(cur_x_min,cur_y_min),cvPoint(cur_x_max,cur_y_max),cvScalar(255,0,0),1);
#endif
	cur_x_min=x2_min[direction];
	cur_y_min = y2_min[direction];

	cur_x_max=x2_max[direction]-RIGHT_BLACK_LEN;
	cur_y_max=y2_max[direction];

	cur_x_min=cur_x_min < 0? 0 : cur_x_min;
	cur_y_min = cur_y_min<0?0:cur_y_min;

	cur_x_max=cur_x_max>FPGA_SINGLE_PIC_W?FPGA_SINGLE_PIC_W:cur_x_max;
	cur_y_max=cur_y_max>FPGA_SINGLE_PIC_H? FPGA_SINGLE_PIC_H:cur_y_max;
	sprintf(buf_2,"./data/save/%d_right.bmp",direction);
	roi_image[direction][RIGHT_ROI]=van_images[direction](
			Range(cur_y_min,cur_y_max),
			Range(cur_x_min,	cur_x_max)   //rightmid
			);
	midX[direction][RIGHT_ROI]=(cur_x_max-cur_x_min)/2.0+cur_x_min;
#if GAIN_SAVE_IMG
	imwrite(buf_2,roi_image[direction][RIGHT_ROI]);

		cv::Rect Roi_rect2 = cv::Rect(cur_x_min-50,cur_y_min,
			roi_image[direction][RIGHT_ROI].cols,
			roi_image[direction][RIGHT_ROI].rows);
//	roi_image[direction][RIGHT_ROI].copyTo(van_images[direction](Roi_rect2));
	cv::rectangle(van_images[direction],cvPoint(cur_x_min,cur_y_min),cvPoint(cur_x_max,cur_y_max),cvScalar(0,255,0),1);
#endif
	}
#if GAIN_SAVE_IMG
for(int i=0;i<CAM_COUNT;i++)
{
	char buf_draw[48];
	sprintf(buf_draw,"./data/draw/%d_draw.bmp",i);
	imwrite(buf_draw,van_images[i]);
}
#endif
	return true;
}


bool overLapRegion::beExist()
{
	for(int direction=0;direction<CAM_COUNT;direction++)
	{
		if(vectors[direction].empty())
			return false;
	}
	return true;
}

void overLapRegion::push_overLap_PointleftAndright(
		int direction_leftcam,
		cv::Point2f pointleft,
		cv::Point2f pointright
		)
{
	m_pointLeft[direction_leftcam].push_back(pointleft);
	 int right_dir=rightExchange(direction_leftcam);
	 m_pointRight[right_dir].push_back(pointright);
}

inline double CEILING(double src, double ceiling)
{ return ((src)>ceiling? ceiling : src);}

void overLapRegion::brightness_blance()
{
#define FACTOR (25.5)
	Point3d gain_c,gain_c2;
		Mat subimg1, subimg2;
		Mat subimg3, subimg4;
		IsDealingWithSLICES=true;
	//	render.En_DisableUseNewGain(true);
		std::vector<Point3d>	alpha[HEIGHT_SLICES],alpha2[HEIGHT_SLICES];

		float gamma=0.25;
		for(int i=0;i<HEIGHT_SLICES;i++)
		{
			alpha[i].resize(CAM_COUNT);
			alpha[i][0].x  = alpha[i][0].y = alpha[i][0].z = 1.0;

			alpha2[i].resize(CAM_COUNT);
			alpha2[i][CAM_COUNT-1].x  = alpha2[i][CAM_COUNT-1].y = alpha2[i][CAM_COUNT-1].z = 1.0;
		}

		for(int i=0;i<CAM_COUNT;i++)
		{
#if 0
			{
				 subimg2=roi_image[i][RIGHT_ROI];//HEAD i号相机//右边
			 }
			 subimg1=roi_image[(i+CAM_COUNT-1)%CAM_COUNT][LEFT_ROI];  //HEAD i-1号相机//左边

			 int j=CAM_COUNT-i-1;
			 {
				 subimg4=roi_image[j][LEFT_ROI];  //TAIL J号相机 //左边
			 }
			 subimg3=roi_image[(j+1)%CAM_COUNT][RIGHT_ROI]; //j+1号相机//右边
#else
					subimg2=roi_image[i][RIGHT_ROI];//HEAD i号相机//右边
					subimg1=roi_image[leftExchange(i)][LEFT_ROI];  //HEAD i-1号相机//左边
					 int j=CAM_COUNT-i-1;
					 {
						 subimg4=roi_image[j][LEFT_ROI];  //TAIL J号相机 //左边
					 }
					 subimg3=roi_image[rightExchange(j)][RIGHT_ROI]; //j+1号相机//右边
#endif
			 int minclos[2],minrows[2];
			 if(subimg1.cols<subimg2.cols)  //使重合区大小相等
			 {
				 minclos[0]=subimg1.cols;
			 }
			 else
			 {
				 minclos[0]=subimg2.cols;
			 }
			 if(subimg3.cols<subimg4.cols)
			 {
				 minclos[1]=subimg3.cols;
			 }
			 else
			 {
				 minclos[1]=subimg4.cols;
			 }

			 if(subimg1.rows<subimg2.rows)
				 {
				 	 minrows[0]=subimg1.rows;
				 }
				 else
				 {
					 minrows[0]=subimg2.rows;
				 }
				 if(subimg3.rows<subimg4.rows)
				 {
					 minrows[1]=subimg3.rows;
				 }
				 else
				 {
					 minrows[1]=subimg4.rows;
				 }

			 resize(subimg1,subimg1,Size( minclos[0], minrows[0]),0,0,INTER_LINEAR);
			 resize(subimg2,subimg2,Size( minclos[0], minrows[0]),0,0,INTER_LINEAR);
			 resize(subimg3,subimg3,Size( minclos[1], minrows[1]),0,0,INTER_LINEAR);
			 resize(subimg4,subimg4,Size( minclos[1], minrows[1]),0,0,INTER_LINEAR);
#if GAIN_SAVE_IMG
			 char buf[4][48];
			 static int a[4]={0};
			 sprintf(buf[0],"./subimg/subimg1_%d.bmp",a[0]);
			 imwrite(buf[0],subimg1);
			 sprintf(buf[1],"./subimg/subimg2_%d.bmp",a[1]);
			 imwrite(buf[1],subimg2);
			 sprintf(buf[2],"./subimg/subimg3_%d.bmp",a[2]);
			 imwrite(buf[2],subimg3);
			 sprintf(buf[3],"./subimg/subimg4_%d.bmp",a[3]);
			 imwrite(buf[3],subimg4);
			 for(int innx=0;innx<4;innx++)
			 {
				 a[innx]++;
			 }
#endif
			 Point3d Isum1[HEIGHT_SLICES], Isum2[HEIGHT_SLICES],
			 Isum3[HEIGHT_SLICES],Isum4[HEIGHT_SLICES];
			 for(int Isumidx=0;Isumidx<HEIGHT_SLICES;Isumidx++)
			 {
				 Isum1[Isumidx] = Point3d(0.0,0.0,0.0);
				 Isum2[Isumidx] = Point3d(0.0,0.0,0.0);
				 Isum3[Isumidx] = Point3d(0.0,0.0,0.0);
				 Isum4[Isumidx] = Point3d(0.0,0.0,0.0);
			 }
		 for(int idx=0;idx<HEIGHT_SLICES;idx++)
		 {
			 int countHead=0,countTail=0;
			 int rows_per_slice = subimg1.rows/HEIGHT_SLICES;
			//head重合区每个像素累加归一化
				for(int y=subimg1.rows/HEIGHT_SLICES*idx;
						y< subimg1.rows/HEIGHT_SLICES*(idx+1)  ;
						y++){
				const Point3_<uchar>* r1 = subimg1.ptr<Point3_<uchar> >(y);
				const Point3_<uchar>* r2 = subimg2.ptr<Point3_<uchar> >(y);
				for(int x=0;x< subimg1.cols;x++){
					countHead++;
					Isum1[idx].x += std::pow(static_cast<double>(r1[x].x/255.0),gamma)*255.0;
					Isum1[idx].y += std::pow(static_cast<double>(r1[x].y/255.0),gamma)*255.0;
					Isum1[idx].z += std::pow(static_cast<double>(r1[x].z/255.0),gamma)*255.0;
					Isum2[idx].x += std::pow(static_cast<double>(r2[x].x/255.0),gamma)*255.0;
					Isum2[idx].y += std::pow(static_cast<double>(r2[x].y/255.0),gamma)*255.0;
					Isum2[idx].z += std::pow(static_cast<double>(r2[x].z/255.0),gamma)*255.0;
				}
			}
			//tail重合区每个像素累加归一化
			for(int y=subimg3.rows/HEIGHT_SLICES*idx;y< subimg3.rows/HEIGHT_SLICES*(idx+1);y++){
				const Point3_<uchar>* r3 = subimg3.ptr<Point3_<uchar> >(y);
				const Point3_<uchar>* r4 = subimg4.ptr<Point3_<uchar> >(y);
				for(int x=0;x< subimg3.cols;x++){
					countTail++;
					Isum3[idx].x += std::pow(static_cast<double>(r3[x].x/255.0),gamma)*255.0;
					Isum3[idx].y += std::pow(static_cast<double>(r3[x].y/255.0),gamma)*255.0;
					Isum3[idx].z += std::pow(static_cast<double>(r3[x].z/255.0),gamma)*255.0;
					Isum4[idx].x += std::pow(static_cast<double>(r4[x].x/255.0),gamma)*255.0;
					Isum4[idx].y += std::pow(static_cast<double>(r4[x].y/255.0),gamma)*255.0;
					Isum4[idx].z += std::pow(static_cast<double>(r4[x].z/255.0),gamma)*255.0;
				}
			}
#define COMPARE_ASSIGN_ZERO(f)  if((f)<0.0001){(f)=0.0001;}
//head :拿图i-1 /图i
			{
				float denominator=(Isum2[idx].x+Isum1[idx].x)/2;
		//		denominator=Isum1[idx].x;

				COMPARE_ASSIGN_ZERO(Isum2[idx].x);
				COMPARE_ASSIGN_ZERO(Isum1[idx].x);
					alpha[idx][i].x  =denominator/ Isum2[idx].x;

					denominator=(Isum2[idx].y+Isum1[idx].y)/2;
			//		denominator=Isum1[idx].y;
				COMPARE_ASSIGN_ZERO(Isum2[idx].y);
				COMPARE_ASSIGN_ZERO(Isum1[idx].y);
					alpha[idx][i].y  = denominator/Isum2[idx].y;

					denominator=(Isum2[idx].z+Isum1[idx].z)/2;
			//		denominator=Isum1[idx].z;
				COMPARE_ASSIGN_ZERO(Isum2[idx].z);
				COMPARE_ASSIGN_ZERO(Isum1[idx].z);
					alpha[idx][i].z  = denominator/Isum2[idx].z;

			}
//tail 拿图j/j-1
			{
				float denominator=(Isum3[idx].x+Isum4[idx].x)/2.0f;
			//	denominator=Isum3[idx].x;
				COMPARE_ASSIGN_ZERO(Isum4[idx].x);
					alpha2[idx][j].x  =denominator/Isum4[idx].x;

					 denominator=(Isum3[idx].y+Isum4[idx].y)/2;
				//	 denominator=Isum3[idx].y;
				COMPARE_ASSIGN_ZERO(Isum4[idx].y);
					alpha2[idx][j].y  = denominator/Isum4[idx].y;

					 denominator=(Isum3[idx].z+Isum4[idx].z)/2;
				//	 denominator=Isum3[idx].z;
				COMPARE_ASSIGN_ZERO(Isum4[idx].z);
					alpha2[idx][j].z  =denominator/Isum4[idx].z;
			}
		}
	}

		for(int index=0;index<CAM_COUNT;index++)
		{
			for(int sliceIdx=0;sliceIdx<HEIGHT_SLICES;sliceIdx++)
			{
			 CV_Assert(roi_image[index][LEFT_ROI].type() == CV_8UC3);
			 CV_Assert(roi_image[index][RIGHT_ROI].type() == CV_8UC3);

			  Point3d gain_[HEIGHT_SLICES]  ,gain_2[HEIGHT_SLICES];
//应用到每张图上

			  gain_[sliceIdx].x = CEILING(std::pow(alpha[sliceIdx][index].x, 1/gamma), 10.0);
			  gain_[sliceIdx].y =CEILING( std::pow(alpha[sliceIdx][index].y, 1/gamma),10.0);//RIGHT
			  gain_[sliceIdx].z = CEILING(std::pow(alpha[sliceIdx][index].z, 1/gamma),10.0);

			  gain_2[sliceIdx].x = CEILING(std::pow(alpha2[sliceIdx][index].x, 1/gamma),10.0);
			  gain_2[sliceIdx].y = CEILING(std::pow(alpha2[sliceIdx][index].y, 1/gamma),10.0);//LEFT
			  gain_2[sliceIdx].z = CEILING(std::pow(alpha2[sliceIdx][index].z, 1/gamma),10.0);

	//		  printf("  alpha[sliceIdx][index].x=%f~~~~~~~~~\n",  alpha[sliceIdx][index].x);

		//	  if(index==3)
			  {
		  printf("CAM:%d gainR_%d:(%f, %f,%f)   gainL_%d:(%f,%f,%f)\n",index,sliceIdx,gain_[sliceIdx].x,gain_[sliceIdx].y,gain_[sliceIdx].z,
							  sliceIdx, gain_2[sliceIdx].x, gain_2[sliceIdx].y, gain_2[sliceIdx].z);
			  }
			  float r[2],g[2],b[2];
			  r[1]= gain_[sliceIdx].x;
			  g[1]= gain_[sliceIdx].y;
			  b[1]= gain_[sliceIdx].z;
			  r[0]= gain_2[sliceIdx].x;
			  g[0]= gain_2[sliceIdx].y;
			  b[0]= gain_2[sliceIdx].z;
#if TEST_GAIN
		render.SetCharacteristicGainMask(index,sliceIdx,(r[0]*FACTOR),(g[0]*FACTOR),(b[0]*FACTOR),
				(r[1]*FACTOR),(g[1]*FACTOR),(b[1]*FACTOR));
#endif
			}
		}
#if TEST_GAIN
		render.BlanceNeighbours2InterPolate();
		render.SetDeltaGainMask();
	//	render.En_DisableUseNewGain(true);
		//printf("using New Gain\n");
#endif
		IsDealingWithSLICES=false;
}
#endif
//将相邻两张图的Gain值取平均
void Render::BlanceNeighbours2InterPolate(){
	memcpy(InterPolatedMask,tempMask,sizeof(InterPolatedMask));
};
