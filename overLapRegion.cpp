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
	EnableSingleHightLight=false;
	Src4=(unsigned char *)malloc(FPGA_SINGLE_PIC_W*FPGA_SINGLE_PIC_H*4*3);
	Src6=(unsigned char *)malloc(FPGA_SINGLE_PIC_W*FPGA_SINGLE_PIC_H*6*3);
	temp4=(unsigned char *)malloc(FPGA_SINGLE_PIC_W*FPGA_SINGLE_PIC_H*4*3);
	temp6=(unsigned char *)malloc(FPGA_SINGLE_PIC_W*FPGA_SINGLE_PIC_H*6*3);

	for(int i=0;i<3;i++)
	{
		pSrc[i]=(unsigned char *)malloc(SDI_WIDTH*SDI_HEIGHT*3);
		ptemp[i]=(unsigned char *)malloc(SDI_WIDTH*SDI_HEIGHT*3);
	}
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
	vector<Point3d>	van_alpha(CAM_COUNT);
	vector<Mat> van_images(CAM_COUNT);
	vector<Mat> van_images_warped_f(CAM_COUNT);
	vector<Mat> Src_images(2);
	char buf[1024];
	int i;
#if USE_BMPCAP
	for(i=0; i<CAM_COUNT; i++){
		sprintf(buf,"./data/gain/%02d.bmp",i);
		van_images[(i+1)%CAM_COUNT] = imread(buf);
		if (van_images[(i+1)%CAM_COUNT].empty()){
			cout<<"Can't open gain/image " << buf<<"open other one"<<endl;
			sprintf(buf,"./data/%02d.bmp",i);
			van_images[(i+1)%CAM_COUNT] = imread(buf);
		}
		else{
			strcpy(buf,"'\0'");
			van_images[(i+1)%CAM_COUNT].convertTo(van_images_warped_f[(i+1)%CAM_COUNT], CV_8U);
		}
	}
#else
	for(int i=0;i<3;i++)
	{
		unsigned char *pcam=PanoCaptureGroup::GetMainInstance()->GetSrc(i);
		if(pcam!=NULL)
		{
			memcpy(ptemp[i],pcam,
			SDI_WIDTH*SDI_HEIGHT*3);
			UYV2RGB(ptemp[i],SDI_WIDTH,SDI_HEIGHT,pSrc[i]);

#if GAIN_SAVE_IMG
		Mat s4(SDI_HEIGHT,SDI_WIDTH,CV_8UC3,pSrc[i]);
		imwrite("./data/save/p4.bmp",s4);
#endif
		}
		else
				return false;
		for(int i=0;i<3;i++)
		{
			van_images[i+1].create(SDI_HEIGHT,SDI_WIDTH,CV_8UC3);
			van_images[i+1].data=pSrc[i];
		}
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

	vector<cv::Point2f>::iterator it2;
	vector<int>::iterator it;
	Vec3b van_pix_1, van_pix_2;

	cv::Point2f vPoint1[3], vPoint2[3];
	int direction;
	char buf_1[1024];
	char buf_2[1024];
	van_alpha[0].x  =van_alpha[0].y = van_alpha[0].z = 1.0;
	float x_min[CAM_COUNT]={},y_min[CAM_COUNT]={},x_max[CAM_COUNT]={},y_max[CAM_COUNT]={};
	float x2_min[CAM_COUNT]={},y2_min[CAM_COUNT]={},x2_max[CAM_COUNT]={},y2_max[CAM_COUNT]={};
int a=0,b=0,c=0,d=0;

float max_panel_length=render.GetPanoLen();
	for(direction=1;direction<4;direction++)
	{
		float cmp_x_min=30000, cmp_y_min=30000,cmp_x_max=-1,cmp_y_max=-1;
		float cmp_x2_min=30000, cmp_y2_min=30000,cmp_x2_max=-1,cmp_y2_max=-1;
		for(it=vectors[direction].begin();it!= vectors[direction].end();it++)
		{
			for(int van_index=0; van_index<3; van_index++)
			{
				render.getPointsValue(direction, *it,vPoint1);     //van_get
				render.getPointsValue((direction+1)%(CAM_COUNT),*it, vPoint2);//van_get

				if(vPoint1[van_index].x>cmp_x_max)
					cmp_x_max=vPoint1[van_index].x;
				if(vPoint1[van_index].y>cmp_y_max)
					cmp_y_max=vPoint1[van_index].y;
				if(vPoint1[van_index].x<cmp_x_min)
					cmp_x_min=vPoint1[van_index].x;
				if(vPoint1[van_index].y<cmp_y_min)
					cmp_y_min=vPoint1[van_index].y;

				if(vPoint2[van_index].x>cmp_x2_max)
					cmp_x2_max=vPoint2[van_index].x;
				if(vPoint2[van_index].y>cmp_y2_max)
					cmp_y2_max=vPoint2[van_index].y;
				if(vPoint2[van_index].x<cmp_x2_min)
					cmp_x2_min=vPoint2[van_index].x;
				if(vPoint2[van_index].y<cmp_y2_min)
					cmp_y2_min=vPoint2[van_index].y;
			}
		}

		x_min[direction]= cmp_x_min;
		y_min[direction]= cmp_y_min;
		x_max[direction]= cmp_x_max;
		y_max[direction]= cmp_y_max;
		if(direction==CAM_COUNT-1)
		{
			x2_min[0]= cmp_x2_min;
			y2_min[0]= cmp_y2_min;
			x2_max[0]= cmp_x2_max;
			y2_max[0]= cmp_y2_max;
		}
		else if(direction==3)
		{
			x2_min[1]= cmp_x2_min;
			y2_min[1]= cmp_y2_min;
			x2_max[1]= cmp_x2_max;
			y2_max[1]= cmp_y2_max;
		}
		else{
		x2_min[direction+1]= cmp_x2_min;
		y2_min[direction+1]= cmp_y2_min;
		x2_max[direction+1]= cmp_x2_max;
		y2_max[direction+1]= cmp_y2_max;
		}
//	strcpy(buf_1,"'\0'");
//	strcpy(buf_2,"'\0'");
//	sprintf(buf_1,"./van_2/%d_left.bmp",direction);
		if(y_min[direction]<0)
			y_min[direction]=0;
		if(y_max[direction]>=SDI_HEIGHT)
			y_max[direction]=SDI_HEIGHT-1;

		if(x_min[direction]<0)
			x_min[direction]=0;
		if(x_max[direction]>=SDI_WIDTH)
			x_max[direction]=SDI_WIDTH-1;


		int tmpidx=-1;
		if(direction+1==4)
			tmpidx=1;
		else
			tmpidx=(direction+1)%CAM_COUNT;
		if(y2_min[tmpidx]<0)
			y2_min[tmpidx]=0;
		if(y2_max[tmpidx]>=SDI_HEIGHT)
			y2_max[tmpidx]=SDI_HEIGHT-1;

		if(x2_min[tmpidx]<0)
			x2_min[tmpidx]=0;
		if(x2_max[tmpidx]>=SDI_WIDTH)
			x2_max[tmpidx]=SDI_WIDTH-1;

	roi_image[direction][LEFT_ROI]=van_images[direction](Range(y_min[direction],y_max[direction]),Range(x_min[direction],	x_max[direction]));
//		imwrite(buf_1,roi_image[direction][LEFT_ROI]);
//		strcpy(buf_2,"./van_2/0_right.bmp");
	if(tmpidx==4)
		roi_image[(1)%CAM_COUNT][RIGHT_ROI]=van_images[1](Range(y2_min[1],y2_max[1]),Range(x2_min[1],	x2_max[1]));
	else
		roi_image[(tmpidx)%CAM_COUNT][RIGHT_ROI]=van_images[tmpidx](Range(y2_min[tmpidx],y2_max[tmpidx]),Range(x2_min[tmpidx],	x2_max[tmpidx]));
//		imwrite(buf_2,roi_image[0][RIGHT_ROI]);
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
	Point3d gain_c;
	Mat subimg1, subimg2;
	 std::vector<Point3d>	alpha;

	float gamma[CAM_COUNT]={0};
	for(int i=0;i<CAM_COUNT;i++)
		gamma[i]=0.25;
		gamma[2]=0.22;
		gamma[1]=0.6;
	alpha.resize(CAM_COUNT);
	alpha[1].x  = alpha[1].y = alpha[1].z = 1.0;
	for(int i=1;i<4;i++)
	{
		 if(i+1==4)
			 subimg2=roi_image[1][RIGHT_ROI];
		 else
		 subimg2=roi_image[(i+1)%CAM_COUNT][RIGHT_ROI];
		 subimg1=roi_image[i][LEFT_ROI];
		 int minw=subimg1.cols;
		 if(minw<subimg2.cols)
			 minw=subimg2.cols;
		 int minh=subimg1.rows;
		 if(minh<subimg2.rows)
			 minh=subimg2.rows;

		 resize(subimg1,subimg1,Size(minw,minh),0,0,INTER_LINEAR);
		 resize(subimg2,subimg2,Size(minw,minh),0,0,INTER_LINEAR);
		Point3d Isum1 = Point3d(0.0,0.0,0.0), Isum2 = Point3d(0.0,0.0,0.0);
		for(int y=0;y< subimg1.rows;y++){
			const Point3_<uchar>* r1 = subimg1.ptr<Point3_<uchar> >(y);
			const Point3_<uchar>* r2 = subimg2.ptr<Point3_<uchar> >(y);
			for(int x=0;x< subimg1.cols;x++){
				Isum1.x += std::pow(static_cast<double>(r1[x].x/255.0),gamma[i])*255.0;
				Isum1.y += std::pow(static_cast<double>(r1[x].y/255.0),gamma[i])*255.0;
				Isum1.z += std::pow(static_cast<double>(r1[x].z/255.0),gamma[i])*255.0;
		//		printf("x=%d  y=%d\n",x,y);

				Isum2.x += std::pow(static_cast<double>(r2[x].x/255.0),gamma[i+1])*255.0;
				Isum2.y += std::pow(static_cast<double>(r2[x].y/255.0),gamma[i+1])*255.0;
				Isum2.z += std::pow(static_cast<double>(r2[x].z/255.0),gamma[i+1])*255.0;
			}
		}
		 if(i==3)
		{
			alpha[1].x  = Isum1.x/Isum2.x;
			alpha[1].y  = Isum1.y/Isum2.y;
			alpha[1].z  = Isum1.z/Isum2.z;

			alpha[1].x  *= alpha[i].x;
			alpha[1].y  *= alpha[i].y;
			alpha[1].z  *= alpha[i].z;
		}
		else{
		alpha[(i+1)%CAM_COUNT].x  = Isum1.x/Isum2.x;
		alpha[(i+1)%CAM_COUNT].y  = Isum1.y/Isum2.y;
		alpha[(i+1)%CAM_COUNT].z  = Isum1.z/Isum2.z;

		alpha[(i+1)%CAM_COUNT].x  *= alpha[i].x;
		alpha[(i+1)%CAM_COUNT].y  *= alpha[i].y;
		alpha[(i+1)%CAM_COUNT].z  *= alpha[i].z;
		}
	}
	Point3d sum_alph = Point3d(0.0, 0.0, 0.0);
	Point3d sum_alph2 = Point3d(0.0, 0.0, 0.0);
		for (int img_idx= 1; img_idx < 4; ++img_idx)
		{
			sum_alph.x += alpha[img_idx].x;
			sum_alph.y += alpha[img_idx].y;
			sum_alph.z += alpha[img_idx].z;

			sum_alph2.x += alpha[img_idx].x*alpha[img_idx].x;
			sum_alph2.y += alpha[img_idx].y*alpha[img_idx].y;
			sum_alph2.z += alpha[img_idx].z*alpha[img_idx].z;
		}
		gain_c.x = sum_alph.x/sum_alph2.x;
		gain_c.y = sum_alph.y/sum_alph2.y;
		gain_c.z = sum_alph.z/sum_alph2.z;   //求出增益值

	//	printf("gain_c.x=%f  gain_c.y=%f gain_c.z=%f \n",gain_c.x,gain_c.y,gain_c.z);
		float x,y,z;

		my_shaderm=(GLShaderManager *)getDefaultShaderMgr();
		 Point3d gain_2[CAM_COUNT];
		if(EnableSingleHightLight==false)
		{
			for(int index=1;index<3;index++)
			{
				x=(std::pow(alpha[index].x*gain_c.x, 1/gamma[index]));
				y=(std::pow(alpha[index].y*gain_c.y, 1/gamma[index]));
				z=(std::pow(alpha[index].z*gain_c.z, 1/gamma[index]));
	//			printf("index=%d x=%f  y=%f  z=%f \n",index,x,y,z);
				my_shaderm=(GLShaderManager *)getDefaultShaderMgr();
				my_shaderm->set_gain_(index,x,y,z);
			}
		}
}
#endif
//将相邻两张图的Gain值取平均
void Render::BlanceNeighbours2InterPolate(){
	memcpy(InterPolatedMask,tempMask,sizeof(InterPolatedMask));
};
