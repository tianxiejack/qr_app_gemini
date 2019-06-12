#include "StlGlDefines.h"
#include "GLRender.h"
#include "overLapBuffer.h"
using namespace std;
using namespace cv;
int cycle[CAM_COUNT][2];
Point3f bar[CAM_COUNT*2];
void Render::calcCommonZone()
{
#if 0
	int zone = PER_CIRCLE/CAM_COUNT;
	//int delta= zone/4*2;
	int delta[CAM_COUNT];
	delta[CAM_0]=zone/4*2;
	delta[CAM_1]=zone/4*2;
	delta[CAM_2]=zone/4*2-0.2f*zone;
	delta[CAM_3]=zone/4*2+0.25f*zone;
	delta[CAM_4]=zone/4*2;
	delta[CAM_5]=zone/4*2;
	delta[CAM_6]=zone/4*2;
	delta[CAM_7]=zone/4*2;

	Point2f p[CAM_COUNT][2][4];
	int i,j,k,index,x,y,num;

//	int data[CAM_COUNT] = {BLEND_HEAD/*-1*/, 0, BLEND_REAR-2/*-8*/,-BLEND_REAR/*-2*/, -2, -BLEND_HEAD-1/*-22*/};

	int data[CAM_COUNT] = {BLEND_HEAD/*-1*/, 0, BLEND_REAR-9/*-8*/,-BLEND_REAR+8/*-2*/, -2, -BLEND_HEAD-1/*-22*/,-2,-2};
	vector<cv::Point3f> *poly_list = BowlLoader.Getpoly_vector();

	for(i=0; i<CAM_COUNT; i++)
	{
		index = delta[i] + i*zone - data[i];
		bar[i] = poly_list->at(index*4+2);
		index += PER_CIRCLE*(MODLE_CIRCLE-1);
		bar[i+CAM_COUNT] = poly_list->at(index*4+2);
		printf("bar[%d]:(%f,%f,%f)\n",i,bar[i].x,bar[i].y,bar[i].z);
	}

	for(i=0; i<CAM_COUNT; i++)
	{
		index  = delta[i] + zone*i;
		for(k=0; k<2; k++)
			for(j=0; j<4; j++)
			{
				x = index + pow(-1,j%2+1)*BLEND_OFFSET;//OVERLAP_WIDTH;
				y = (j/2)*MODLE_CIRCLE/2;//OVERLAP_HEIGHT;//%(MODLE_CIRCLE-1);

				if(0 == j%2)
					cycle[i][0] = x;
				else
					cycle[i][1] = x;

				num = y*MODLE_CIRCLE + x;
				p[i][k][j] = pixleList[(i-k+CAM_COUNT)%CAM_COUNT][num*3];
			}
	}
	overLap.setPoints(p);
#endif
}
