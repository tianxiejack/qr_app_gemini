// StlTexturex.cpp :
// Author: Hongwei Lu
//
#include "RenderMain.h"
#include "common.h"
#include "main.h"
#include "timing.h"
#include"GLEnv.h"

#if (VALIDATION_PERIOD_SECONDS > 0)
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include "HWStencil.h"

extern "C" {
   extern void do_auth();
}

void init_auth() __attribute__((constructor));

void init_auth()
{

    do_auth();
}
#endif

extern GLEnv env1,env2;

RenderMain::RenderMain()
{
}
RenderMain::~RenderMain()
{
}
void RenderMain::DrawGLScene()
{
	GLEnv &env=env1;
    static bool ONCE_FULLSCREEN = true;

	if(ONCE_FULLSCREEN){
		ONCE_FULLSCREEN = false;
		render.ProcessOitKeys(env,'F', 0, 0);
	}
		render.DrawGLScene();
	DrawIdle();
}
void RenderMain::ReSizeGLScene(int Width, int Height)
{
	render.ReSizeGLScene(Width,Height);
}

void RenderMain::keyPressed(unsigned char key, int x, int y)
{
	GLEnv &env=env1;
	render.keyPressed(env,key,x,y);
}
void RenderMain::specialkeyPressed(int key, int x, int y)
{
	GLEnv &env=env1;
	render.specialkeyPressed(env,key,x,y);
}

void RenderMain::mouseButtonPress(int button, int state, int x, int y)
{
	render.mouseButtonPress(button,state,x,y);
}
void RenderMain::mouseButtonPressDS(int button, int state, int x, int y)
{
	render.mouseButtonPressDS(button,state,x,y);
}
void RenderMain::mouseMotionPress(int x, int y)
{
	render.mouseMotionPress(x,y);
}



void RenderMain::initGlut(int argc, char **argv,int startx,int starty)
{
	char arg1[256], arg2[256];
	// GLUT Window Initialization:
	glutInit (&argc, argv);
	glutInitWindowSize (1920, 1080);
	glutInitWindowPosition(startx,  starty);
	glutInitDisplayMode ( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	{

	sprintf(arg1,"First %s (%s, %s):",VERSION_STRING, __DATE__,__TIME__);
	strcat (arg1, argv[1]);
	strcat (arg1, "+");
	strcat (arg1, argv[2]);

		/* getting a warning here about passing arg1 of sprinf incompatable pointer type ?? */
		/* WTF ?!? */
		if (sprintf(arg2, " %i+%i Polygons using ", render.BowlGetpoly_count(), render.VehicleGetpoly_count()))
		{
			strcat (arg1, arg2);
		}
		if (sprintf(arg2, "%i+%i Kb", render.BowlGetMemSize()/1024, render.VehicleGetMemSize()/1024))
		{
			strcat (arg1, arg2);
		}

		/* save most of the name for use later */
		common.setWindowName(arg1);

		if (sprintf(arg2, "%.2f FPS", common.getFrameRate()))
		{
			strcat (arg1, arg2);
		}
		glutSetOption(GLUT_RENDERING_CONTEXT,GLUT_USE_CURRENT_CONTEXT);
		glutCreateWindow (arg1);
	}

	glutSetCursor(GLUT_CURSOR_NONE);

	/* Register the event callback functions since we are using GLUT */




	glutDisplayFunc(DrawGLScene); /* Register the function to do all our OpenGL drawing. */
	//glutIdleFunc(DrawIdle); /* Even if there are no events, redraw our gl scene. */
	glutReshapeFunc(ReSizeGLScene); /* Register the function called when our window is resized. */
	glutKeyboardFunc(keyPressed); /* Register the function called when the keyboard is pressed. */
	glutSpecialFunc(specialkeyPressed); /* Register the special key function */
//	glutMouseFunc(mouseButtonPress); /* Register the function called when the mouse buttons are pressed */
//	glutMotionFunc(mouseMotionPress); /*Register the mouse motion function */
}
void RenderMain::parseArgs(int argc, char** argv)
{
	char arg1[64];
	if (argc > 4)//4
	{
		strcpy(arg1, "-o");
		if (strcmp(argv[3], arg1) == 0)
		{
			printf("Running in Ortho View\n");
			common.setViewFlag(ORTHO);
		}
	}
	if (common.isPerspective())
	{
		printf("Running in Perspective View\n");
	}

	if (argc == 5)
	{
		strcpy(arg1, "-f");
		if (strcmp(argv[4], arg1) == 0)
		{
			printf("           Redrawing only on view change\n");
			common.setIdleDraw(GL_NO);
		}
		strcpy(arg1, "-v");
		if (strcmp(argv[4], arg1) == 0)
		{
			printf("           Debug Output Enabled\n");
			common.setVerbose(GL_YES);
		}
	}
	render.BowlParseSTLAscii(argv[1]);
	render.VehicleParseObj(argv[2]);

	if(argv[3]!=NULL)
	{
		render.PanelParseSTLAscii(argv[3]);
	}
	else
	{
		render.PanelParseSTLAscii(argv[1]);
	}


}

void RenderMain::DrawIdle()
{
#if (VALIDATION_PERIOD_SECONDS > 0)
	{
		static timeval firstEntrance;
		static bool once = true;
		static bool printOnce = true; 
		static bool check_once= true;
		int duration_seconds = 0;
		timeval now;
		gettimeofday(&now, 0);
		if(once){
			firstEntrance = now;
			once = false;
		}
		duration_seconds = now.tv_sec - firstEntrance.tv_sec;
		if( check_once && 1<(duration_seconds/(VALIDATION_PERIOD_SECONDS)) ){
			if(0==HW_stencil()){
			 printf("Stencil Done\n");
			 check_once = false;
			}
			else{
				if(printOnce){
				    printf("Stencil failed\n");
				    printOnce = false;
				}
				usleep(500000);
			 return;
			}
			
		}
	}
#endif
	glutPostRedisplay();
}

//--------main entry------------------
int RenderMain::start(int argc, char** argv)
{
		parseArgs(argc, argv);

		initGlut(argc, argv);
		initGlew();
		render.initPixle();
		//glutFullScreen();
		
#if DOUBLE_SCREEN
	doubleScreenInit(argc, argv);
	initGlew();
	glutFullScreen();
//	render.SetupRCDS(1920, 1080);//1920,1080);//
#endif
	glutSetWindow(1);
	glutHideWindow();
	glutShowWindow();
	glutFullScreen();
	render.SetupRC(1920, 1080);//1920,1080);//
	glutMainLoop();
	return 0;
}

