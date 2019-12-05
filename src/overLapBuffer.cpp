/*
 * overLapBuffer.cpp
 *
 *  Created on: Feb 16, 2017
 *      Author: wang
 */

#include "overLapBuffer.h"
#include "main.h"

#define COPYSIZE (IMAGEWIDTH*IMAGEHEIGHT*3)
overLapBuffer overLap;
overLapBuffer::overLapBuffer()
{
	for(int i=0; i<CAM_COUNT; i++)
	{
		for(int j=0; j<ENUM_COUNT; j++)
		{
			for(int k=0; k<ENUM_COUNT; k++)
			{
				gap[i].direction[j].buf[k] = Mat(IMAGEHEIGHT,IMAGEWIDTH,CV_8UC3,cvScalar(0));
			}
		}
	}
}

overLapBuffer::~overLapBuffer()
{
	for(int i=0; i<CAM_COUNT; i++)
	{
		for(int j=0; j<ENUM_COUNT; j++)
		{
			for(int k=0; k<ENUM_COUNT; k++)
			{
				gap[i].direction[j].buf[k].release();
			}
		}
	}
}

void overLapBuffer::setPoints(Point2f P[][2][4])
{
	int i,j;
	for(i=0; i<CAM_COUNT; i++)
		for(j=0; j<2; j++)
		{
			zone[i][j] = calcRect(P[i][j]);
		}
}
Rect overLapBuffer::calcRect(Point2f pos[4])
{
	int x ,y;
	Point2i max(-1000,-1000),min(1000,1000);
	for(int i=0; i<4; i++)
	{
		x = i%2*IMAGEWIDTH;
		y = i%2*IMAGEHEIGHT;
		pos[i].x = (pos[i].x > 1e-6) ? (pos[i].x) : (x);
		pos[i].y = (pos[i].y > 1e-6) ? (pos[i].y) : (y);
		max.x = MAX(max.x,pos[i].x);
		min.x = MIN(min.x,pos[i].x);
		max.y = MAX(max.y,pos[i].y);
		min.y = MIN(min.y,pos[i].y);
	}
	return Rect(min,max);
}
void overLapBuffer::updateGap(int chId,Mat& buf)
{
	updateGapBuffer(chId, ENUM_RIGHT, buf);
	updateGapBuffer(chId, ENUM_LEFT, buf);

	calcDiffCh(chId);
}
void overLapBuffer::calcDiffCh(int chId)
{
	calcDiffSum(chId,ENUM_RIGHT);
	calcDiffSum(chId,ENUM_LEFT);
}
void overLapBuffer::calcDiffSum(int chId, int mark)
{
	Mat& cur = gap[chId].direction[mark].buf[ENUM_NOW];
	Mat& pre = gap[chId].direction[mark].buf[ENUM_OLD];

	status[chId][mark] = getFrameAbsDiffSum(cur, pre, zone[chId][mark]);
//	printf("status[%d][%d]:%f\n",chId,mark,status[chId][mark]);
	computeStatus(chId,mark);
}
#define IS_ABOVE(num) ((num) > (VALUE_THRESHOLD))
void overLapBuffer::computeStatus(int chId, int mark)
{
	double left,right;
	int GapLeft,GapRight;

	if(ENUM_RIGHT == mark)
	{
		GapLeft  = chId;
		GapRight = (chId+1)%CAM_COUNT;
	}else
	{
		GapLeft  = (chId-1+CAM_COUNT)%CAM_COUNT;
		GapRight = chId;
	}

	left = status[GapLeft][ENUM_RIGHT];
	right= status[GapRight][ENUM_LEFT];

	bool leftNormal = IS_ABOVE(left);
	bool rightNormal= IS_ABOVE(right);
//	printf("leftNormal:%d,rightNormal:%d,left:%f,right:%f,\n",leftNormal,rightNormal,left,right);

	compareStatus(GapLeft,leftNormal,rightNormal);
}
int overLapBuffer::compareStatus(int chId, bool left, bool right)
{
	static int alpha=1;
	if(left&&right)
	{
		alpha = 1;
//		printf(":%d ",alpha);
	}
	else if(left&&(!right))
	{
		alpha = 0;
//		printf(":%d ",alpha);
	}
	else if(right&&(!left))
	{
		alpha = 2;
//		printf(":%d ",alpha);
	}
	else if((!left)&&(!right))
	{
		return 0;
	}
//	printf("alpha:%d\n",alpha);
	updateOverlap(chId, alpha);

	return alpha;
}
void overLapBuffer::updateOverlap(int chId, int alpha)
{
	render.setOverlapPeta(chId,alpha);
}
void overLapBuffer::swap(int index, int mark)
{
	gap[index].direction[mark].buf[ENUM_NOW].copyTo(gap[index].direction[mark].buf[ENUM_OLD]);
}

void overLapBuffer::copyToGap(int index, int mark, Mat& buf)
{
	gap[index].direction[mark].buf[ENUM_NOW] = buf.clone();
}

void overLapBuffer::updateGapBuffer(int chId, int mark, Mat& buf)
{
	swap(chId,mark);
	copyToGap(chId,mark,buf);
}

void overLapBuffer::updateGapLeft(int chId, Mat& buf)
{
	updateGapBuffer(chId, ENUM_LEFT, buf);
}

void overLapBuffer::updateGapRight(int chId, Mat& buf)
{
	updateGapBuffer(chId, ENUM_RIGHT, buf);
}

//double getFrameAbsDiffSum(unsigned char *pCurImg, unsigned char *pPreImg, int width, int height, int leftX, int topY, int block_size)
//double overLapBuffer::getFrameAbsDiffSum(unsigned char *pCurImg, unsigned char *pPreImg, Zone zoneT)
double overLapBuffer::getFrameAbsDiffSum(Mat& pCurImg, Mat& pPreImg, Rect zoneT)
{
	int width = IMAGEWIDTH;
	int height= IMAGEHEIGHT;

	Mat difImg(zoneT.height,zoneT.width,CV_8UC2,cvScalar(0));

    Mat curROI = pCurImg(zoneT);
    Mat preROI = pPreImg(zoneT);

    absdiff(curROI, preROI, difImg);

    CvScalar sumDiff = mean(difImg);

//    printf("sumDiff val[0]:%f,val[1]:%f,sum:%f\n",sumDiff.val[0],sumDiff.val[1],sumDiff.val[0]+sumDiff.val[1]);
    return (sumDiff.val[0] + sumDiff.val[1]);
}
