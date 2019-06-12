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
		int dev_id=FPGA_FOUR_CN;
		if(pHDAsyncVCap[dev_id]==NULL)
			pHDAsyncVCap[dev_id] = new HDAsyncVCap4(auto_ptr<BaseVCap>(new HDv4l_cam(dev_id,SDI_WIDTH,SDI_HEIGHT)),dev_id);
		dev_id=FPGA_SIX_CN;
		if(pHDAsyncVCap[dev_id]==NULL)
			pHDAsyncVCap[dev_id] = new HDAsyncVCap4(auto_ptr<BaseVCap>(new HDv4l_cam(dev_id,SDI_WIDTH,SDI_HEIGHT)),dev_id);
	}
};
unsigned char *  PanoCaptureGroup::GetSrc(int idx)
{
	if(idx==FPGA_FOUR_CN)
	{
		return pHDAsyncVCap[FPGA_FOUR_CN]->GetSrc();
	}
	else if(idx==FPGA_SIX_CN)
	{
		return pHDAsyncVCap[FPGA_SIX_CN]->GetSrc();
	}
	else
		return NULL;
}

void  PanoCaptureGroup::OpenProducers()
{
	int dev_id=FPGA_FOUR_CN;
	 pHDAsyncVCap[dev_id]->Open();
	 dev_id=FPGA_SIX_CN;
	 pHDAsyncVCap[dev_id]->Open();
}

PanoCaptureGroup::~PanoCaptureGroup()
{
	for(int i=1 ;i<MAX_CC;i++)//0 is not used
	{
		if(pHDAsyncVCap[i]){
					delete pHDAsyncVCap[i];
					pHDAsyncVCap[i]= NULL;
		}
	}
}


PanoCaptureGroup * PanoCaptureGroup::GetMainInstance()
{
	int queueid[2]={MAIN_FPGA_SIX,MAIN_FPGA_FOUR};
	int count=2;
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
