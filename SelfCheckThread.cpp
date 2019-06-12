#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include <pthread.h>
#include"GLRender.h"
#include"overLapRegion.h"
#include"CaptureGroup.h"
#include"SelfCheckThread.h"
#include"CheckMyself.h"
#include "Zodiac_GPIO_Message.h"
extern SelfCheck selfcheck;

void *SelfCheck_thread(void *arg)
{
	 gpio_init();
	//sleep(5);
	selfcheck.initState();
	while(1)
	{

		selfcheck.CheckBrokenCam();
		//selfcheck.Check12CAM();
		//selfcheck.CheckExtra2CAM();
	//	selfcheck.Check2HD();
	//	selfcheck.CaptureCheckAll();
	//	selfcheck.SendBrokenCAM();
	 sleep(10);
	}
	return 0;
}

void start_SelfCheck_thread(void)
{
	pthread_t tid;
	int ret;
	ret = pthread_create( &tid, NULL,SelfCheck_thread, NULL );
}
