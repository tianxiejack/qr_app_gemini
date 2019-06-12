/*
 * CaptureGroup.cpp
 *
 *  Created on: Dec 16, 2016
 *      Author: wang
 */

#include "CaptureGroup.h"
#include <stdbool.h>
#include <iostream>
#include <assert.h>
#include <string.h>
#include "buffer.h"
#include "updatebuffer.h"

#if USE_12
#include"HDV4lcap.h"
#endif
using namespace std;

PseudoCaptureGroup PseudoCaptureGroup::pseudoCaptureGroup(0,0,0,0);

CaptureGroup::CaptureGroup(unsigned int capCount):m_TotalCamCount(capCount),m_currentIdx(0),
		width(SDI_WIDTH),height(SDI_HEIGHT),depth(DEFAULT_IMAGE_DEPTH),Defaultimage(NULL)
{
	capCamera = new pInterface_VCap[m_TotalCamCount];
	for(int i = 0; i < capCount; i ++){
		capCamera[i] = new PseudoVCap();
	}
	//memset((void*)m_V4LImag,0,sizeof(m_V4LImag));
	Defaultimage = new unsigned char [capCount*width* height * depth];
	fillColorBar();
}
CaptureGroup::CaptureGroup(unsigned int wide,unsigned int height,int NCHAN,unsigned int capCount):
		m_TotalCamCount(capCount),m_currentIdx(0),width(wide),height(height),
		depth(NCHAN),Defaultimage(NULL)
{
	capCamera = new pInterface_VCap[m_TotalCamCount];
	for(int i = 0; i < capCount; i ++){
		capCamera[i] = new PseudoVCap();
	}
	//memset((void*)m_V4LImag,0,sizeof(m_V4LImag));
	Defaultimage = new unsigned char [capCount*width* height * depth];
	fillColorBar();
}

void CaptureGroup::init(int *queueid,int count)
{
	CreateProducers();
	SetConsumers(GetConsumers(queueid,count));
	OpenProducers();
	Open();
}

CaptureGroup::~CaptureGroup()
{
	Close();
	for(int i=0; i < m_TotalCamCount; i ++){
		BaseVCap *pCamera = dynamic_cast<BaseVCap*>(capCamera[i]);
		if(pCamera){
			delete pCamera;
		}
		//delete update[i];
	}

	delete [] capCamera;
	delete [] Defaultimage;
}

void CaptureGroup::SetConsumers(vector<Consumer>  h)
{
     int size = h.size();
     for(int i = 0; i < size; i++){
    //re-oder the cameras according to idx
    	 for(int j = 0; j<size;j++){
    			 if(h[j].idx == i){
    				 Append(h[j].pcap);
    			 }
    	 }
     }
}


void CaptureGroup::fillColorBar()
{
	float factor = 18.45f;
	for(int i=0; i<m_TotalCamCount; i++)
	{
		char *pBuff = (char*)(& Defaultimage[i* width * height* depth]);
		unsigned int offset = (i+1)*width*height;
		for(int j = 0; j < width*height; j++)
		{
			int real = (int)factor;
			*pBuff++ = (int)(((j +offset%255)*(real*real)))&0xFF;
			*(pBuff++) = (int)(((j+offset%255)*real))&0xFF;
			*(pBuff++) = (int)(((m_TotalCamCount-1-i)*(j+offset%255)))&0xFF;//init a bluer stripe texture in the buffers.
			factor *= 3.30f;
			offset = (int)(offset*real)&0xff;
		}
	}
}

bool CaptureGroup:: Append(pInterface_VCap cap)
{
	if(m_currentIdx < m_TotalCamCount ){
		char * pImg = (char*)&Defaultimage[m_currentIdx * width * height* depth];
		// delete the pseudoCamera before assigning new one.
		delete capCamera[m_currentIdx];
		capCamera[m_currentIdx] = cap;
		cap->SetDefaultImg(pImg);
		m_currentIdx++;
		return true;
	}
	return false;
}
void CaptureGroup::Open()
{
	for(int i = 0; i < m_currentIdx; i ++){
		if(!capCamera[i]->Open()){
			cout<<"Error: fail to open camera["<<i<<"]"<<endl;
		}
	}
}

void CaptureGroup::Close()
{
	for(int i=0; i<m_currentIdx; i++)
	{
		capCamera[i]->Close();
	}
}

bool  CaptureGroup::captureCamTouchSrc(GLubyte *ptr, int index,bool istouchsrc)
{
	bool Isenhdata=false;
	if(istouchsrc)
	{
		capCamera[index]->CaptureTouchSrc((char*)ptr,istouchsrc);
	}
	else
	{
		printf("  CaptureGroup::captureCamTouchSrc error!\n");
		assert(false);
	}
	if(ptr[0]==SRCUYVY)
	{
		Isenhdata=true;
	}
	return Isenhdata;
}

void UYV2Ycrcb(unsigned char *src,unsigned char *dst,int w,int h)
{
	for(int i=0;i<w*h;i++)
	{
		unsigned char temp0=src[i*3];//U
		unsigned char temp1=src[i*3+1];//Y
		unsigned char temp2=src[i*3+2];//V
		dst[i*3]=temp1;
		dst[i*3+1]=temp0;
		dst[i*3+2]=temp2;
	}
}

bool CaptureGroup::captureCam(GLubyte *ptr, int index)
{
	bool Isenhdata=false;
	capCamera[index]->Capture((char*)ptr);
	if(ptr[0]==SRCUYVY)
	{
		 Isenhdata=true;
	}
	#if USE_BMPCAP
	Mat src6(FPGA_SINGLE_PIC_H*6,FPGA_SINGLE_PIC_W,CV_8UC3,ptr);
	int x=0,y=0,w=FPGA_SINGLE_PIC_W,h=FPGA_SINGLE_PIC_H;
	char buf[24]={0};
	static bool once[2]={true,true};
	if(index==0  && once[0])
	{
		once[0]=false;
		for(int i=0;i<6;i++)
		{
			y=i*h;
			Rect roi(x,y,w,h);
			Mat dst=src6(roi);
			UYV2Ycrcb(dst.data,dst.data,dst.cols,dst.rows);
			cvtColor(dst,dst,CV_YCrCb2RGB);
			sprintf(buf,"./data/gain/%.2d.bmp",i);
			imwrite(buf,dst);
		}
	}
	else 	if(index==1  && once[1])
	{
		once[1]=false;
		for(int i=0;i<4;i++)
		{
			y=i*h;
			Rect roi(x,y,w,h);
			Mat dst=src6(roi);
			UYV2Ycrcb(dst.data,dst.data,dst.cols,dst.rows);
			cvtColor(dst,dst,CV_YCrCb2RGB);
			sprintf(buf,"./data/gain/%.2d.bmp",i+6);
			imwrite(buf,dst);
		}
	}
	#endif
	return Isenhdata;
}
void CaptureGroup::captureCamFish(GLubyte *ptr, int index)
{
	capCamera[index]->CaptureFish((char*)ptr);
}
bool CaptureGroup::getScannerPic(IplImage *pImage)
{
	if(capCamera[0])
	{
		capCamera[0]->Capture(pImage->imageData);
		return true;
	}
	return false;
}

bool CaptureGroup::saveCapImg()
{
	char buf[32];
	for(int i=0; i<CAM_COUNT; i++)
	{
		sprintf(buf,"./data/%02d.bmp",i);
		if(capCamera[i])
			capCamera[i]->SavePic(buf);
		else
			printf("cap:%d error\n",i);
	}
	return true;
}

bool CaptureGroup::saveExposureCompensationCapImg()
{
	char buf[32];
	for(int i=0; i<CAM_COUNT; i++)
	{
		sprintf(buf,"./data/gain/%02d.bmp",i);
		if(capCamera[i])
			capCamera[i]->SavePic(buf);
		else
			printf("cap:%d error\n",i);
	}
	return true;
}



void CaptureGroup::saveOverLap()
{
	for(int i=0; i<CAM_COUNT; i++)
	{
		if(capCamera[i])
			capCamera[i]->saveOverLap();
		else
			printf("cap:%d error\n",i);
	}
}

bool CaptureGroup::saveSingleCapImg(int cam_num)
{
	char buf[32];
	int i=0;
	i=cam_num;
		sprintf(buf,"./data/%02d.bmp",i);
		if(capCamera[i])
			capCamera[i]->SavePic(buf);
		else
			printf("cap:%d error\n",i);

	return true;
}



vector<Consumer>  HDCaptureGroup::GetConsumers(int *queueid,int count)
{
	 Consumer cons;
	 for(int i=0;i<count;i++)
	 {
		 int qid=queueid[i];
		   cons.pcap = new HDVCap(qid,SDI_WIDTH,SDI_HEIGHT);
		   cons.idx = i;
		   v_cons.push_back(cons);
	 }
	   return v_cons;
}


PseudoCaptureGroup *PseudoCaptureGroup::GetInstance()
{
	static bool once =true;
	if(once){
		pseudoCaptureGroup.init(NULL,0);
		once =false;
	}
	return &pseudoCaptureGroup;
}

