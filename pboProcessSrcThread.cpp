
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include <pthread.h>
#include "pboProcessSrcThread.h"
#include<string.h>
#include <glew.h>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "GLRender.h"
#include "PBOManager.h"
#include <osa_sem.h>
#include "gst_capture.h"
#include"Thread_Priority.h"
using namespace cv;
extern Render render;
extern GLEnv env1,env2;

extern bool IsgstCap;
#if GSTREAM_CAP
extern RecordHandle * record_handle;
#endif
#if 1
static int iniCC=3;
#else
static int iniCC=4;
#endif

void *pbo_process_thread(void *arg)
{
	GLEnv &env=env1;
	static bool once4=true;
	setCurrentThreadHighPriority(THREAD_L_GST);
#if 1
    Mat testData(CURRENT_SCREEN_HEIGHT, CURRENT_SCREEN_WIDTH, CV_8UC3);
#else
    Mat testData(CURRENT_SCREEN_HEIGHT, CURRENT_SCREEN_WIDTH, CV_8UC4);
#endif
#if GSTREAM_CAP
 initGstCap();
#endif
	while(1)
	{
		OSA_semWait(render.GetPBORcr(env)->getSemPBO(),100000);
		int processId=render.GetPBORcr(env)->getCurrentPBOIdx();
#if GSTREAM_CAP
		if(IsgstCap)
			gstCapturePushData(record_handle, (char *)*render.GetPBORcr(env)->getPixelBuffer(processId) , CURRENT_SCREEN_WIDTH*CURRENT_SCREEN_HEIGHT*iniCC);
		#else
		static int a=0;
		a++;
					if(a==20)
					{
						memcpy(testData.data, (char *)*render.GetPBORcr(env)->getPixelBuffer(processId),CURRENT_SCREEN_HEIGHT*CURRENT_SCREEN_WIDTH*iniCC);
						imwrite("./data/50TEST_PBO.bmp",testData);
					}
#endif
	}
}


void start_pbo_process_thread(void)
{
	pthread_t tid;
	int ret;
	ret = pthread_create( &tid, NULL,pbo_process_thread, NULL );
}
