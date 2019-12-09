#include "RenderMain.h"
#include "GLRender.h"
#include "common.h"
#include "main.h"
#include "thread.h"
#include "recvwheeldata.h"
#include"SelfCheckThread.h"
#if USE_CAP_SPI
#include "Cap_Spi_Message.h"
#endif

#if MVDETECTOR_MODE
#include "mvDetector.hpp"
#endif
#include"mvdetectInterface.h"


#include "BMPCaptureGroup.h"
#include"PanoCaptureGroup.h"
#include"ChosenCaptureGroup.h"

#include"GLEnv.h"

//add by wsh
#include "CUartProc.hpp"



RenderMain mainWin;
Common common;
AlarmTarget mainAlarmTarget;
GLEnv env1;
GLEnv env2;


float track_pos[4];

#if MVDETECTOR_MODE
void mvDetectorDraw(std::vector<TRK_RECT_INFO> &resTarget,int chId)
{
	int i=0;
	Rect get_rect;
	for(i=0;i<resTarget.size();i++)
		{
			get_rect=resTarget[i].targetRect;
			mainAlarmTarget.SetSingleRectangle(chId,i,get_rect);
		}
		mainAlarmTarget.SetTargetCount(chId,i);
}
#endif
int main(int argc, char** argv)
{

#if USE_CAP_SPI
	SpiSet();
	//InitIPCModule();
#endif

	Parayml param;
	if(!param.readParams("./Param.yml"))
		printf("read param error\n");


#if USE_BMPCAP
	env1.init(BMPPanoGroup::GetInstance(),
			BMPMiscGroup::GetInstance(),
			ChosenCaptureGroup::GetMvDetectInstance(),
			NULL,
			BMPMiscGroup::GetInstance());
	env2.init(BMPPanoGroup::GetInstance(),
			BMPMiscGroup::GetInstance(),
			NULL,
			NULL,
			BMPMiscGroup::GetInstance());
#else

	env1.init(PanoCaptureGroup::GetMainInstance(),
			NULL,//ChosenCaptureGroup::GetSubInstance(),
					NULL,
					NULL,
			BMPMiscGroup::GetInstance()
			);
	env2.init(NULL,
			NULL,//ChosenCaptureGroup::GetSubInstance(),
			NULL,
			NULL,
			BMPMiscGroup::GetInstance()
			);
#endif


//	start_overLap();
//	startrecv( );


#if USE_GPIO
	InitIPCModule();
	init_GPIO_IPCMessage();
#endif
	start_stitch();

	 #if MVDETECTOR_MODE
        mvDetector* mvDetector=mvDetector::getInstance();
        mvDetector->creat();
        mvDetector->init();
        mvDetector->mvDetectorDrawCB(mvDetectorDraw);
        mvDetector->setWarnMode(WARN_MOVEDETECT_MODE,0);
        #endif
	
//	initcabinrecord();//初始化舱内视频记录
//	initscreenrecord();//初始化录屏记录

    	CUartProc* recv = new CUartProc("/dev/ttyTHS2", 115200, 0, 8, 'N', 1);
    	recv->copen();
    	OSA_thrCreate(&(recv->recv_thrID), recv->thrRecv, 0, 0, NULL);




	mainWin.start(argc, argv);

#if USE_GPIO
	IPC_Destroy();
	delete_GPIO_IPCMessage();
#endif
	//gpio_deinit();

	OSA_thrDelete(&(recv->recv_thrID));

	return 0;
}

