#include<stdio.h>
#include<stdlib.h>
#include"Camera.h"
#include"StlGlDefines.h"
#include"CaptureGroup.h"
#include"CheckMyself.h"
#include <pthread.h>
#include"Zodiac_Message.h"
#include"Zodiac_GPIO_Message.h"

extern unsigned int last_gpio_sdi;
extern bool isinSDI;
SelfCheck selfcheck;

void   SelfCheck::CheckBrokenCam()
{
#if 1
#if 1
	BrokenCam[0]=get_gpioNum_Value(GPIO_FAR_3_NUM);
	BrokenCam[1]=get_gpioNum_Value(GPIO_FAR_2_NUM);
	BrokenCam[2]=get_gpioNum_Value(GPIO_FAR_1_NUM);
	BrokenCam[3]=get_gpioNum_Value(GPIO_FAR_0_NUM);
	BrokenCam[4]=get_gpioNum_Value(GPIO_FAR_4_NUM);
	BrokenCam[5]=get_gpioNum_Value(GPIO_FAR_5_NUM);
	BrokenCam[6]=get_gpioNum_Value(GPIO_FAR_6_NUM);
	BrokenCam[7]=get_gpioNum_Value(GPIO_FAR_7_NUM);
	BrokenCam[8]=get_gpioNum_Value(GPIO_FAR_8_NUM);
	BrokenCam[9]=get_gpioNum_Value(GPIO_FAR_9_NUM);
#else
	BrokenCam[0]=get_gpioNum_Value(GPIO_FAR_0_NUM);
	BrokenCam[1]=get_gpioNum_Value(GPIO_FAR_1_NUM);
	BrokenCam[2]=get_gpioNum_Value(GPIO_FAR_2_NUM);
	BrokenCam[3]=get_gpioNum_Value(GPIO_FAR_3_NUM);
	BrokenCam[4]=get_gpioNum_Value(GPIO_FAR_4_NUM);
	BrokenCam[5]=get_gpioNum_Value(GPIO_FAR_5_NUM);
	BrokenCam[6]=get_gpioNum_Value(GPIO_FAR_6_NUM);
	BrokenCam[7]=get_gpioNum_Value(GPIO_FAR_7_NUM);
	BrokenCam[8]=get_gpioNum_Value(GPIO_FAR_8_NUM);
	BrokenCam[9]=get_gpioNum_Value(GPIO_FAR_9_NUM);

#endif
#else
for(int i=0;i<CAM_COUNT;i++)
	BrokenCam[i]=1;
#endif
}

self_check_state  SelfCheck::IsIDLE()
{
	for(int i=0;i<CAM_COUNT;i++)
	{
		if(BrokenCam[i]==-1)
		{
			return SELFCHECK_IDLE;
		}
		else if(BrokenCam[i]==0)
		{
			return SELFCHECK_FAIL;
		}
	}
	return SELFCHECK_PASS;
}
void  SelfCheck::CaptureCheckAll()
{
	for(int i=0;i<CAM_COUNT+2+2;i++)
	{
		if(capture_state[i]==SELFCHECK_IDLE)  //first time :I will check all,include set GPIO
		{
			//ori is idle;
	//	SetCheckState(CHECK_ITEM_CAPTURE,IDLE);
		return;
		}
	}
	for(int j=0;j<CAM_COUNT+2+2;j++)        //get broken cam
	{
		if(capture_state[j]==SELFCHECK_FAIL)
		{
			BrokenCam[j]=j;
		}
	}
	for(int k=0;k<CAM_COUNT+2+2;k++)   //if one is broken,return
	{
		if(BrokenCam[k]!=-1)
			SetCheckState(CHECK_ITEM_CAPTURE,SELFCHECK_FAIL);
		return;
	}
	SetCheckState(CHECK_ITEM_CAPTURE,SELFCHECK_PASS);//VAN_TEST
	return;
}


SelfCheck::SelfCheck()
{
	m_CheckResult[CHECK_ITEM_CAPTURE]=SELFCHECK_IDLE;
	m_CheckResult[CHECK_ITEM_COMMUNICATION]=SELFCHECK_PASS;
	m_CheckResult[CHECK_ITEM_BOARD]=SELFCHECK_PASS;
	for(int i=0;i<CAM_COUNT;i++)
	{
		capture_state[i]=SELFCHECK_IDLE;
		BrokenCam[i]=-1;
	}
	nowtime=0;
	lasttime=0;
	Onesec=true;
}

void SelfCheck::SetCheckState(self_check_item now_item,self_check_state now_state)
{
	m_CheckResult[now_item]=now_state;
}

self_check_state SelfCheck::getCheckState(self_check_item now_item)
{
	return m_CheckResult[now_item];
}
void SelfCheck::initState()
{
	for(int i=0;i<CAM_COUNT;i++)
	{
		capture_state[i]=SELFCHECK_IDLE;
		BrokenCam[i]=-1;
	}
}

void SelfCheck::Check12CAM()//0~11
{
	GLubyte  *ptr=(GLubyte *)malloc(DEFAULT_IMAGE_WIDTH*DEFAULT_IMAGE_HEIGHT*4);
	int index=0;
	for(int index=0;index<CAM_COUNT;index++){
//	CaptureGroup::GetPanoCaptureGroup()->captureCam(ptr,index);
	CheckCaptureState(ptr,index,DEFAULT_IMAGE_WIDTH,DEFAULT_IMAGE_HEIGHT);
	}
	free(ptr);
}
void SelfCheck::CheckExtra2CAM()//12~13
{
/*	GLubyte  *Extraptr=(GLubyte *)malloc(DEFAULT_IMAGE_WIDTH*DEFAULT_IMAGE_HEIGHT*4);
	int index=0;
	for(int index=0;index<2;index++){
	CaptureGroup::GetExtCaptureGroup()->captureCam(Extraptr,index);
	CheckCaptureState(Extraptr,CAM_COUNT+index,DEFAULT_IMAGE_WIDTH,DEFAULT_IMAGE_HEIGHT);
	}
	free(Extraptr);*/
}
void SelfCheck::Check2HD()//14~15
{
	/*GLubyte  *HDptr=(GLubyte *)malloc(SDI_WIDTH*SDI_HEIGHT*4);
	int index=0;
#if USE_GPIO
	if(!isinSDI)
		{
			set_gpioNum_Value(GPIO_NUM152,ENABLE_SDI1);
			for(int index=0;index<1;index++){
			CaptureGroup::GetSDICaptureGroup()->captureCam(HDptr,index);
			CheckCaptureState(HDptr,CAM_COUNT+2+index,SDI_WIDTH,SDI_HEIGHT);
			}
			set_gpioNum_Value(GPIO_NUM152,ENABLE_SDI2);
			for(int index=0;index<1;index++){
			CaptureGroup::GetSDICaptureGroup()->captureCam(HDptr,index);
			CheckCaptureState(HDptr,CAM_COUNT+2+1+index,SDI_WIDTH,SDI_HEIGHT);
			}
		}
		else
		{
			for(int index=0;index<1;index++){
					CaptureGroup::GetSDICaptureGroup()->captureCam(HDptr,index);
					CheckCaptureState(HDptr,CAM_COUNT+2+last_gpio_sdi+index,SDI_WIDTH,SDI_HEIGHT);
			}
		}
#else
		for(int index=0;index<1;index++){
	//		CaptureGroup::GetSDICaptureGroup()->captureCam(HDptr,index);
//			CheckCaptureState(HDptr,CAM_COUNT+2+index,SDI_WIDTH,SDI_HEIGHT);
		}
#endif
	free(HDptr);*/
}


void SelfCheck::CheckCaptureState(GLubyte *ptr,int index,int imgWidth,int ingHeight)
{
	int CamFineis[50];
	for(int i=0;i<50;i++)
	{
		CamFineis[i]=1;
	}
	if(index>13)//HD
	{
		for(int i=0;i<50;i++)
		{
				if(*(ptr+imgWidth*i+i)==0x80
				&& *(ptr+imgWidth*i+i+1)==0x80
				&&  *(ptr+imgWidth*i+i+2)==0x80
				&&    *(ptr+imgWidth*i+i+3)==0x00)
				CamFineis[i]=0;
		}
	}
	else//PAL
	{
		for(int i=0;i<50;i++)
		{
			if(*(ptr+imgWidth*i+i)==0xc0
				&& *(ptr+imgWidth*i+i+1)==0x40
				&&  *(ptr+imgWidth*i+i+2)==0x70
				&&    *(ptr+imgWidth*i+i+3)==0x40)
				CamFineis[i]=0;
		}
	}
	capture_state[index]=SELFCHECK_PASS;
	for(int i=0;i<50;i++)
	{
		if(!CamFineis[i])
		{
			capture_state[index]=SELFCHECK_FAIL;
		//	printf("~~~~~~~~~~camera  %d is broken~~~~~~~~~\n",index);
			return ;
		}
	}
}

void SelfCheck::SendBrokenCAM()
{
	SendBackBrokenCam(BrokenCam);
	return;
}


bool SelfCheck::IsOnesec()
{
	return Onesec;
}

bool SelfCheck::IsAllReady()
{
	for(int i=0;i<SELFCHECK_ITEM_COUNT;i++)
	{
		if( m_CheckResult[i]	!=	SELFCHECK_PASS )
			return false;
	}
	return true;
}


void SelfCheck::CalculateTime(time_t sec)
{
		time(&nowtime);
		if((nowtime-lasttime)>=sec)
		{
			lasttime=nowtime;
			if(Onesec)
				Onesec=false;
			else if(!Onesec)
				Onesec=true;
		}
}

void SendBackBrokenCam(int *CamN)
{
	static float Once=true;
	unsigned char fine=0;
	unsigned char fine1=1;
#if USE_UART
	IPC_msg   ipc_msg;
	ipc_msg.msg_type=IPC_MSG_TYPE_CYCLE_SELF_CHECK_FEEDBACK;
	int n=0;
	for(int i=0;i<CAM_COUNT+2+2;i++)
	{
			if( CamN[i]!=-1)
			{
			//	printf("Sendback  i+1=%d     %d is broken~~~~~~~~~\n",i,CamN[i]+1);
				ipc_msg.payload.ipc_c_faultcode.circle_faultcode[n]=(unsigned char)(CamN[i]+1);
				n++;
			}
	}
	if(n!=0)
	{
		ipc_msg.payload.ipc_c_faultcode.fault_code_number=(unsigned char)n;
		WriteMessage(&ipc_msg);
	}
	else if(n==0&&Once==true)
	{
		ipc_msg.payload.ipc_c_faultcode.circle_faultcode[0]=fine;
		ipc_msg.payload.ipc_c_faultcode.fault_code_number=fine1;
		WriteMessage(&ipc_msg);
		Once=false;
	}
#endif
}




