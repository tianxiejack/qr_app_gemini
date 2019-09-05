
/**
 * Copyright 1993-2012 NVIDIA Corporation.  All rights reserved.
 *
 * Please refer to the NVIDIA end user license agreement (EULA) associated
 * with this source code for terms and conditions that govern your use of
 * this software. Any use, reproduction, disclosure, or distribution of
 * this software and related documentation outside the terms of the EULA
 * is strictly prohibited.
 */
#include <stdio.h>
#include <stdlib.h>

#include <cuda.h>
#include "CaptureRtsCam.h"
#include "buffer.h"
#include "StlGlDefines.h"
using namespace std;
using namespace cv;
static bool Once_buffer=true;
extern int m_bufId[QUE_CHID_COUNT];
extern RTSCam* m_rtscam;
unsigned char * rts_uyuv[3] = {NULL,NULL,NULL};
extern Alg_Obj * queue_main_sub;
void processFrame(const cv::Mat frame,const int chId)
{
	m_rtscam->SetMat(frame,chId);
}
RTSCam::RTSCam()
{
	for(int i=0;i<3;i++)
		Rstcam[i] = NULL;

	ReadOnvifConfigFile();
	if(Once_buffer)
	{
		init_buffer();
		Once_buffer=false;
	}
	start_queue();
}
int RTSCam::ReadOnvifConfigFile()
{
	char cfgAvtFile[64] = "onvif.yml";
	FILE *fp = fopen(cfgAvtFile, "rt");

	if(fp != NULL){
		for(int i=0;i<3;i++){
			fscanf(fp, "%s\n", devname);
			Rstcam[i]= RTSPCapture_Create();
			Rstcam[i]->init(devname,i,1920,1080,processFrame);
		}
	}
	else
	{
		printf("[get params] Can not find YML. Please put this file into the folder of execute file\n");
		exit (-1);
	}

	return 0;
}
void RTSCam::SetMat(cv::Mat frame,int chid)
{
						unsigned char **transformed_src_main=NULL;
			//uyvy2uyv
						transformed_src_main=&rts_uyuv[chid];
						uyvy2uyv(*transformed_src_main,frame.data,1920,1080);
				//		cv::Mat dst;
				//		cv::cvtColor(frame,dst,COLOR_YUV2BGR_UYVY);
				//		cv::imwrite("1.bmp",dst);
				//		*transformed_src_main = dst.data;
						if(Data2Queue(*transformed_src_main,1920,1080,chid))
						{
							if(getEmpty(&*transformed_src_main, chid))
							{
							}
						}
}
bool RTSCam::getEmpty(unsigned char** pYuvBuf, int chId)
{
	int status=0;
	bool ret = true;
	while(1)
	{
		status = OSA_bufGetEmpty(&queue_main_sub->bufHndl[chId],&m_bufId[chId],0);
		if(status == 0)
		{
			*pYuvBuf = (unsigned char*)queue_main_sub->bufHndl[chId].bufInfo[m_bufId[chId]].virtAddr;
			break;
		}else{
			if(!OSA_bufGetFull(&queue_main_sub->bufHndl[chId],&m_bufId[chId],OSA_TIMEOUT_FOREVER))
			{
				if(!OSA_bufPutEmpty(&queue_main_sub->bufHndl[chId],m_bufId[chId]))
				{
					;
				}
			}
		}
	}
	 return ret;
}
bool RTSCam::Data2Queue(unsigned char *pYuvBuf,int width,int height,int chId)
{
	int status;

//	chId = 0;
	if(chId>=QUE_CHID_COUNT)//if(chId >= CAM_COUNT+1)
		return false;
	queue_main_sub->bufHndl[chId].bufInfo[m_bufId[chId]].width=width;
	queue_main_sub->bufHndl[chId].bufInfo[m_bufId[chId]].height=height;
	queue_main_sub->bufHndl[chId].bufInfo[m_bufId[chId]].strid=width;
	OSA_bufPutFull(&queue_main_sub->bufHndl[chId],m_bufId[chId]);
	return true;
}
void RTSCam::uyvy2uyv(unsigned char *dst,unsigned char *src, int ImgWidth, int ImgHeight)
{
	for(int j =0;j<ImgHeight;j++)
	{
		for(int i=0;i<ImgWidth*2/4;i++)
		{
			*(dst+j*ImgWidth*3+i*6+0)=*(src+j*ImgWidth*2+i*4+0);
			*(dst+j*ImgWidth*3+i*6+1)=*(src+j*ImgWidth*2+i*4+1);
			*(dst+j*ImgWidth*3+i*6+2)=*(src+j*ImgWidth*2+i*4+2);

			*(dst+j*ImgWidth*3+i*6+3)=*(src+j*ImgWidth*2+i*4+0);
			*(dst+j*ImgWidth*3+i*6+4)=*(src+j*ImgWidth*2+i*4+3);
			*(dst+j*ImgWidth*3+i*6+5)=*(src+j*ImgWidth*2+i*4+2);

		}
	}
}
void  RTSCam::start_queue()
{
	for(int j = 0;j<3;j++)
		getEmpty(&(rts_uyuv[j]),j);
}

