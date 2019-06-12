#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include <pthread.h>
#include"GLRender.h"
#include"overLapRegion.h"
#include"CaptureGroup.h"
#include"ExposureCompensationThread.h"
#include"GLEnv.h"
extern GLEnv env1,env2;
extern Render render;
//extern overLapRegion  overlapregion;


#if USE_GAIN


void *exposure_thread(void *arg)
{
	GLEnv &env=env1;
	sleep(1);

	int t[10]={0};
 timeval startT[20]={0};


	while(1)
	{
	//	gettimeofday(&startT[4],0);

		if(render.isUsingNewGain())
		{
		//if(!overLapRegion::GetoverLapRegion()->GetSingleHightLightState())
		{
		//	 sleep(2);
			 if(overLapRegion::GetoverLapRegion()->van_save_coincidence())
			 {  //此处我如果不能打开文件就返回，GetSingleHightLightState() 是否会继续使用亮度均衡的值？
				 overLapRegion::GetoverLapRegion()->brightness_blance();
			 }
		}
		render.SetGainisNew(true);
		 sleep(10);
		}
		else
			sleep(3);
	//	gettimeofday(&startT[5],0);
//		t[2]=((startT[5].tv_sec-startT[4].tv_sec)*1000000+(startT[5].tv_usec-startT[4].tv_usec))/1000.0;
//		printf("deltatimet[5]-t[4] =%d ms    \n",t[2]);
	}
}



void start_exposure_thread(void)
{
	pthread_t tid;
	int ret;
	ret = pthread_create( &tid, NULL,exposure_thread, NULL );
}
#endif
