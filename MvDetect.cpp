#include<opencv2/opencv.hpp>
#include "mvdetectInterface.h"
#include<string.h>
#include<stdio.h>
#include "infoHead.h"
#include "osa_sem.h"
#include "Render_Agent.h"
using namespace cv;
extern bool IsMvDetect;
int  parm_accuracy=10;
int parm_inputArea=6/*6*/;
int parm_inputMaxArea=200;
int parm_threshold = 30;
int MV_CHOSE_IDX=0;
int MAX_RECT_COUNTS=50;
Mat m4(2160,640,CV_8UC3);
Mat m6(3240,640,CV_8UC3);
Mat m_chosen(1080,1920,CV_8UC3);

Mat m4_2cc(2160,640,CV_8UC2);
Mat m6_2cc(3240,640,CV_8UC2);
Mat m_chosen_2cc(1080,1920,CV_8UC2);
unsigned char * p_newestMvSrc[CAM_COUNT]={NULL,NULL,NULL,NULL,NULL,NULL,NULL
,NULL};
extern MvDetect mv_detect;


void MvDetect::SetoutRect(bool isinchosen)
{
	int len=0;
#if 1
	if(isinchosen)
	{
		int currentcamidx=(int )Render_Agent::GetCurrentChosenCam();
		outRect_Single_Channel[currentcamidx].clear();
		if(!tempRect_Srcptr[currentcamidx].empty())
		{
			OSA_semWait(this->GetpSemMV(currentcamidx),100000);
			mvRect tempOut;
					len=tempRect_Srcptr[currentcamidx].size();
			//		printf("chosenlen  =%d !\n",len);
		//	if(len<=MAX_RECT_COUNTS)
			{
				for(int j=0;j<len;j++)
				{
					if(tempRect_Srcptr[currentcamidx][j].targetRect.x>0)
					{
						(tempOut.outRect.targetRect)=tempRect_Srcptr[currentcamidx][j].targetRect;
						tempOut.camIdx=currentcamidx;
						outRect_Single_Channel[currentcamidx].push_back(tempOut);
					}
				}
			}
				OSA_semSignal(this->GetpSemMV(currentcamidx));
		}
	}
	else
	{
	for(int idx=0;idx<CAM_COUNT;idx++)
	{
		outRect[idx].clear();
		if(!tempRect_Srcptr[idx].empty())
		{
			OSA_semWait(this->GetpSemMV(idx),100000);
			mvRect tempOut;
				{
					len=tempRect_Srcptr[idx].size();
				}
			//	printf("panolen  =%d !\n",len);
			//	if(len<=MAX_RECT_COUNTS)
				{
				for(int j=0;j<len;j++)
				{
					if(tempRect_Srcptr[idx][j].targetRect.x>0)
					{
					//	tempOut.outRect=tempRect_Srcptr[idx][j];
						(tempOut.outRect.targetRect)=tempRect_Srcptr[idx][j].targetRect;
						tempOut.camIdx=idx;
						outRect[idx].push_back(tempOut);
				//		printf("Cam:%d     x=%d   y=%d  w=%d  h=%d\n",idx,
				//				tempRect_Srcptr[idx][j].targetRect.x,
				//				tempRect_Srcptr[idx][j].targetRect.y,
			//					tempRect_Srcptr[idx][j].targetRect.width,
			//					tempRect_Srcptr[idx][j].targetRect.height);
					}
				}
				}
				OSA_semSignal(this->GetpSemMV(idx));
		}
	}
#else

	outRect[0].clear();
	if(!tempRect_Srcptr[8].empty())
	{
		OSA_semWait(this->GetpSemMV(8),100000);
		mvRect tempOut;
					{
						len=tempRect_Srcptr[8].size();
					}
					for(int j=0;j<len;j++)
					{
						if(tempRect_Srcptr[8][j].targetRect.x>0)
						{
						//	tempOut.outRect=tempRect_Srcptr[idx][j];
							(tempOut.outRect.targetRect)=tempRect_Srcptr[8][j].targetRect;
							tempOut.camIdx=8;
							outRect[0].push_back(tempOut);
						}
					}
		OSA_semSignal(this->GetpSemMV(8));
		}
#endif
	}
}

