// StlTexturex.cpp :
// Author: Hongwei Lu
//
#include "RenderMain.h"
#include "common.h"
#include "main.h"
#include "timing.h"
#include"GLEnv.h"
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
		glutPostRedisplay();
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
	glutMouseFunc(mouseButtonPress); /* Register the function called when the mouse buttons are pressed */
	glutMotionFunc(mouseMotionPress); /*Register the mouse motion function */
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
	/*
	float distance[4];
	distance[0]=atof(argv[3])*1.0;
	distance[1]=atof(argv[4])*1.0;
	distance[2]=atof(argv[5])*1.0;
	distance[3]=atof(argv[6])*1.0;

	if(distance[0]<0.0)
	{
		distance[0]=20.0;
	}
	if(distance[1]<0.0)
	{
		distance[1]=20.0;
	}
	if(distance[2]<0.0)
	{
		distance[2]=20.0;
	}
	if(distance[3]<0.0)
	{
		distance[3]=3.0;
	}

		render.setTankDistance(distance);
*/

	if(argv[3]!=NULL)
	{
		render.PanelParseSTLAscii(argv[3]);
	}
	else
	{
		render.PanelParseSTLAscii(argv[1]);
	}


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
