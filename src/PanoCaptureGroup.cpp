#include "PanoCaptureGroup.h"
#include"HDV4lcap.h"
 PanoCaptureGroup PanoCaptureGroup::MainPanoGroup(SDI_WIDTH,SDI_HEIGHT,3,CAM_COUNT);
 PanoCaptureGroup PanoCaptureGroup::SubPanoGroup(SDI_WIDTH,SDI_HEIGHT,3,CAM_COUNT);

 static HDAsyncVCap4* pHDAsyncVCap[MAX_CC]={0};
	static bool ProduceOnce = true;
PanoCaptureGroup::PanoCaptureGroup(unsigned int w,unsigned int h,int NCHAN,unsigned int capCount):
		HDCaptureGroup(w,h,NCHAN,capCount)
{
}

void  PanoCaptureGroup::CreateProducers()
{
	if(ProduceOnce)
	{
		ProduceOnce=false;
		int dev_id=0;
		if(pHDAsyncVCap[dev_id]==NULL)
			pHDAsyncVCap[dev_id] = new HDAsyncVCap4(auto_ptr<BaseVCap>(new HDv4l_cam(dev_id,SDI_WIDTH,SDI_HEIGHT)),dev_id);
		dev_id=1;
		if(pHDAsyncVCap[dev_id]==NULL)
			pHDAsyncVCap[dev_id] = new HDAsyncVCap4(auto_ptr<BaseVCap>(new HDv4l_cam(dev_id,SDI_WIDTH,SDI_HEIGHT)),dev_id);
		dev_id=2;
		if(pHDAsyncVCap[dev_id]==NULL)
			pHDAsyncVCap[dev_id] = new HDAsyncVCap4(auto_ptr<BaseVCap>(new HDv4l_cam(dev_id,SDI_WIDTH,SDI_HEIGHT)),dev_id);
	}
};
unsigned char *  PanoCaptureGroup::GetSrc(int idx)
{
	if(idx==0)
	{
		return pHDAsyncVCap[0]->GetSrc();
	}
	else if(idx==1)
	{
		return pHDAsyncVCap[1]->GetSrc();
	}else if(idx == 2){
		return pHDAsyncVCap[2]->GetSrc();
	}
	else
		return NULL;
}

void  PanoCaptureGroup::OpenProducers()
{
	int dev_id=0;
	 pHDAsyncVCap[dev_id]->Open();
	 dev_id=1;
	 pHDAsyncVCap[dev_id]->Open();
	 dev_id=2;
	 pHDAsyncVCap[dev_id]->Open();
}

PanoCaptureGroup::~PanoCaptureGroup()
{
	for(int i=0 ;i<3;i++)//0 is not used
	{
		if(pHDAsyncVCap[i]){
					delete pHDAsyncVCap[i];
					pHDAsyncVCap[i]= NULL;
		}
	}
}


PanoCaptureGroup * PanoCaptureGroup::GetMainInstance()
{
	int queueid[3]={2,1,0};
	int count=3;
	static bool once =true;
	if(once){
		MainPanoGroup.init(queueid,count);
		once =false;
	}
	return &MainPanoGroup;
}

PanoCaptureGroup * PanoCaptureGroup::GetSubInstance()
{
	int queueid[2]={SUB_FPGA_SIX,SUB_FPGA_FOUR};
	int count=2;
	static bool once =true;
	if(once){
		SubPanoGroup.init(queueid,count);
		once =false;
	}
	return &SubPanoGroup;
}
