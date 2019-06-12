#include <opencv/cv.hpp>
#include<opencv2/opencv.hpp>
#include <unistd.h>
#include <string.h>
#include "main.h"
#include "GLRender.h"
#include "scanner.h"
#include"GLEnv.h"
extern GLEnv env1,env2;
extern Render render;
const char * stitchStart  = "GPU: start";
const char * stitchRecover= "GPU: recover";

static int pano_stitch_state=0;

int initStitchConfig(bool needSavePic)
{
	GLEnv &env =env1;
	if(needSavePic==true)
	{
		env.GetPanoCaptureGroup()->saveCapImg();
	}
	char buf[128];
	int ret=0,state;
	for(int index=0; index<CAM_COUNT; index++)
	{
		printf("\n%s, %d, i = %d\n",__FUNCTION__,__LINE__, index);
		if(!common.getStateChannel(index))
		{
			printf("\n--%s, %d\n",__FUNCTION__,__LINE__);
			sprintf(buf,"./stitchX -p ./data/%02d.yml",index);
			state = system(buf);
			usleep(1500*1000);
			printf("\n----%s, %d\n",__FUNCTION__,__LINE__);
			if(state == 0)
			{
				common.setStateChannel(index);
				printf("\n------%s, %d\n",__FUNCTION__,__LINE__);
				glutPostRedisplay();
			}
			ret |= state;
		}
	}
	return ret;
}

bool iniGenerate(bool needSavePic)
{
	int ret = initStitchConfig(needSavePic);
	if(0 == ret)
	{
		render.initPixle();
		puts(__FUNCTION__);
		common.setStitchState(true);
		glutPostRedisplay();
		return true;
	}

	return false;
}

int removeStitchConfig()
{
	render.destroyPixList();
	return system("rm *.ini");
}

bool iniDestroy()
{
	//notify UI thread to show init pic
	common.setStitchState(false);
	//mark stateStitch=false
	glutPostRedisplay();

	puts(__FUNCTION__);

	int ret = 0;//removeStitchConfig();

	return (0 == ret);
}

bool codeParse(char* code)
{
	 GLEnv &env=env1;
	if(code == NULL)
		return false;

	if( (common.isStateRecover()) && (strlen(code)==strlen(stitchStart)) && (strncmp(code,stitchStart,strlen(stitchStart))==0) )
	{
		//start
		common.setScanned(true);
			render.ProcessOitKeys(env,'i', 0, 0);
		glutPostRedisplay();
		bool ret = iniGenerate(true);
		if(ret)	//no more scaner
			return true;
	}

	if( (common.isStateStitch()) && (strlen(code)==strlen(stitchRecover)) && (strncmp(code,stitchRecover,strlen(stitchRecover))==0) )
	{
		//recover
		bool ret = iniDestroy();
		if(ret)	//no more scaner
			return true;
	}

	return false;
}


bool startStitch(bool state)
{

		bool ret = iniGenerate(state);
		if(ret)	//no more scaner
			return true;

	return false;
}


void* thread_overlap(void*)
{
	GLEnv &env=env1;
	sleep(5);
	while(1)
	{
		usleep(300*1000);
		if(ISDYNAMIC==1)
			env.GetPanoCaptureGroup()->saveOverLap();
	}
	return NULL;
}

void *thread_stitch(void *)
{
	bool ret=false;

	while(1)
	{
		usleep(1000*1000);
		if(GetThreadStitchState()==STITCH_ONLY)
		{
			if(startStitch(false))
			{
				SetThreadStitchState(NO_STITCH);
				continue;
			}


		}
		else if(GetThreadStitchState()==SAVEPIC_STITCH)
		{
			if(startStitch(true))
			{
				SetThreadStitchState(NO_STITCH);
				continue;
			}
		}
	}

	return NULL;
}

int GetThreadStitchState()
{
	return pano_stitch_state;
}

void SetThreadStitchState(int state)
{
	pano_stitch_state=state;
}
