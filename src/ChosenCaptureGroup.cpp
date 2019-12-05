#include "ChosenCaptureGroup.h"
#include"HDV4lcap.h"
ChosenCaptureGroup ChosenCaptureGroup::MainChosenGroup(SDI_WIDTH,SDI_HEIGHT,3,MAIN_AND_SUB_EXT_COUNT);
ChosenCaptureGroup ChosenCaptureGroup::SubChosenGroup(SDI_WIDTH,SDI_HEIGHT,3,SUB_EXT_COUNT);
ChosenCaptureGroup ChosenCaptureGroup::MvDetectGroup(SDI_WIDTH,SDI_HEIGHT,3,MVDECT_CAM_COUNT/2);
ChosenCaptureGroup ChosenCaptureGroup::MvDetect_add_Group(SDI_WIDTH,SDI_HEIGHT,3,MVDECT_CAM_COUNT/2);
static HDAsyncVCap4* pHDAsyncVCap[MAX_CC]={0};
ChosenCaptureGroup::ChosenCaptureGroup(unsigned int w,unsigned int h,int NCHAN,unsigned int capCount):
		HDCaptureGroup(w,h,NCHAN,capCount)
{ }

void  ChosenCaptureGroup::CreateProducers()
{
		int dev_id=MAIN_CN;
		if(pHDAsyncVCap[dev_id]==NULL)
			pHDAsyncVCap[dev_id] = new HDAsyncVCap4(auto_ptr<BaseVCap>( new HDv4l_cam(dev_id,SDI_WIDTH,SDI_HEIGHT)),dev_id);
		 dev_id=SUB_CN;
			if(pHDAsyncVCap[dev_id]==NULL)
				pHDAsyncVCap[dev_id] = new HDAsyncVCap4(auto_ptr<BaseVCap>( new HDv4l_cam(dev_id,SDI_WIDTH,SDI_HEIGHT)),dev_id);
		dev_id=MVDECT_CN;
			if(pHDAsyncVCap[dev_id]==NULL)
				pHDAsyncVCap[dev_id] = new HDAsyncVCap4(auto_ptr<BaseVCap>( new HDv4l_cam(dev_id,SDI_WIDTH,SDI_HEIGHT)),dev_id);

			dev_id=MVDECT_ADD_CN;
					if(pHDAsyncVCap[dev_id]==NULL)
						pHDAsyncVCap[dev_id] = new HDAsyncVCap4(auto_ptr<BaseVCap>( new HDv4l_cam(dev_id,SDI_WIDTH,SDI_HEIGHT)),dev_id);

};

void  ChosenCaptureGroup::OpenProducers()
{
	int dev_id=MAIN_CN;
	 pHDAsyncVCap[dev_id]->Open();
	 dev_id=SUB_CN;
	 pHDAsyncVCap[dev_id]->Open();
	 dev_id=MVDECT_CN;
	 	 pHDAsyncVCap[dev_id]->Open();
	 dev_id=MVDECT_ADD_CN;
			 pHDAsyncVCap[dev_id]->Open();
}

ChosenCaptureGroup::~ChosenCaptureGroup()
{
	for(int i=1 ;i<MAX_CC;i++)//0 is not used
	{
		if(pHDAsyncVCap[i]){
					delete pHDAsyncVCap[i];
					pHDAsyncVCap[i]= NULL;
		}
	}
}
ChosenCaptureGroup * ChosenCaptureGroup::GetMvDetect_add_Instance()
{
	int queueid[5]={MAIN_1,
			MAIN_2,
			MAIN_3,
			MAIN_4,
			MAIN_5
		};
	int count=5;
	static bool once =true;
	if(once){
		MvDetect_add_Group.init(queueid,count);
		once =false;
	}
	return &MvDetect_add_Group;
}
ChosenCaptureGroup * ChosenCaptureGroup::GetMvDetectInstance()
{
	int queueid[5]={
			MAIN_6,
			MAIN_7,
			MAIN_8,
			MAIN_9,
			MAIN_10};
	int count=5;
	static bool once =true;
	if(once){
		MvDetectGroup.init(queueid,count);
		once =false;
	}
	return &MvDetectGroup;
}
ChosenCaptureGroup * ChosenCaptureGroup::GetMainInstance()
{
	int queueid[2]={MAIN_ONE_OF_TEN,SUB_ONE_OF_TEN};
	int count=2;
	static bool once =true;
	if(once){
		MainChosenGroup.init(queueid,count);
		once =false;
	}
	return &MainChosenGroup;
}

ChosenCaptureGroup * ChosenCaptureGroup:: GetSubInstance()
{
	int queueid[1]={SUB_ONE_OF_TEN};
	int count=1;
	static bool once =true;
	if(once){
		SubChosenGroup.init(queueid,count);
		once =false;
	}
	return &SubChosenGroup;
}
