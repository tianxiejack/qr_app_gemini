#include"RenderMain.h"
#include "GLRender.h"
#include "common.h"
#include "GLEnv.h"
#include"Thread_Priority.h"
#include "thread_idle.h"
#include"mvdetectInterface.h"
#include"Xin_IPC_Yuan_Recv_Message.h"
#include"ClicktoMoveForesight.h"
extern bool IsMvDetect;
extern bool isEnhanceOn;
extern thread_idle tIdle;
extern Render render;
extern MvDetect mv_detect;
Common comSecondSC;
extern float forward_data;
extern GLEnv env2,env1;
extern ForeSightPos foresightPos[MS_COUNT];
extern char chosenCam[2];
extern float  menu_tpic[8];
void Render::InitBowlDS()
{

}

			void Render::ChangeSecondMv()
			{
				int net_open_mvdetect=getKey_TargetDetectionState(TRANSFER_TO_APP_DRIVER);
				if(net_open_mvdetect==1)	//todo useless
				{
#if MVDECT
					if(!mv_detect.isNotStopped())
					{
						tIdle.threadRun(MVDECT_CN);
						tIdle.threadRun(MVDECT_ADD_CN);
						mv_detect.mvOpen();
						mv_detect.ClearAllVector(true);
					}
					else
					{
						mv_detect.mvClose();
						mv_detect.ClearAllVector(false);
					}
#endif
				}
			}

			void Render::ChangeSecondEnh()
			{
				int net_open_enhance=getKey_ImageEnhancementState(TRANSFER_TO_APP_DRIVER);
						if(net_open_enhance==1)
						{
							isEnhanceOn=!isEnhanceOn;
						}
			}
			void Render::ChangeSecondSc()
			{
						if(SecondDisplayMode==SECOND_CHOSEN_VIEW_MODE)
						{
							int net_dirction=getKey_MoveDirection(TRANSFER_TO_APP_DRIVER);
							int camera_dir=chosenCam[SUB]-1;
							if((net_dirction==MOVE_TYPE_MOVELEFT)||(net_dirction==MOVE_TYPE_MOVEUP))
							{
								camera_dir=(camera_dir+9)%10;
							}
							else if((net_dirction==MOVE_TYPE_MOVERIGHT)||(net_dirction==MOVE_TYPE_MOVEDOWN))
							{
								camera_dir=(camera_dir+1)%10;
							}
							chosenCam[SUB]=camera_dir+1;
							ChangeSubChosenCamidx(chosenCam[SUB]);
						}
			}
			void Render::MoveSecondForesight()
			{
				GLEnv &env=env1;
			if(SecondDisplayMode==SECOND_ALL_VIEW_MODE)
			{
				int net_dirction=getKey_MoveDirection(TRANSFER_TO_APP_DRIVER);
								if(net_dirction==MOVE_TYPE_MOVELEFT)
								{
									ProcessOitKeysDS(env,'1',0,0);
								}
								if(net_dirction==MOVE_TYPE_MOVERIGHT)
								{
									ProcessOitKeysDS(env,'2',0,0);
								}
								if(net_dirction==MOVE_TYPE_MOVEUP)
								{
									ProcessOitKeysDS(env,'3',0,0);
								}
								if(net_dirction==MOVE_TYPE_MOVEDOWN)
								{
									ProcessOitKeysDS(env,'4',0,0);
								}

			}
			}


			void Render::mouseButtonPressDS(int button, int state, int x, int y)
			{

			}
void Render::ChangeSecondMode()
{
	Mode_Type mt=getKey_SwitchMode(TRANSFER_TO_APP_DRIVER);
	switch(mt)
	{
	case 	Mode_Type_START:
		break;
	case 		Mode_Type_SINGLE_POPUP_WINDOWS:
		if(SecondDisplayMode==SECOND_CHOSEN_VIEW_MODE)
		{
			SecondDisplayMode=SECOND_ALL_VIEW_MODE;
		}
		else if(SecondDisplayMode==SECOND_ALL_VIEW_MODE)
		{
			SecondDisplayMode=SECOND_CHOSEN_VIEW_MODE;
		}
		break;
	case Mode_Type_DEBUG:
		break;
	case 		Mode_Type_RESERVE:
		break;
	}
}
void Render::RenderSceneDS()
{}
void Render::SetupRCDS(int windowWidth, int windowHeight)
{}
void Render::ProcessOitKeysDS(GLEnv &m_env,unsigned char key, int x, int y)
{
	switch(key)
		{
	case '(':
				foresightPos[SUB].SetSpeedX((render.get_PanelLoader().Getextent_pos_x()-render.get_PanelLoader().Getextent_neg_x())/1920.0*10.0);
					break;
			case ')':
				foresightPos[SUB].SetSpeedY((render.get_PanelLoader().Getextent_pos_z()-render.get_PanelLoader().Getextent_neg_z())/1920.0*20.0);
				break;
			case'N':
			{
				SECOND_DISPLAY nextMode=SECOND_DISPLAY(((int)SecondDisplayMode+1)%SECOND_TOTAL_MODE_COUNT);
				if(nextMode==SECOND_559_ALL_VIEW_MODE)
				{
					nextMode=SECOND_ALL_VIEW_MODE;
				}
				SecondDisplayMode = nextMode;
			}
				break;
			case	'n':
			{
				chosenCam[SUB]=chosenCam[SUB]+1;
				if(chosenCam[SUB]==11)
					chosenCam[SUB]=1;
				ChangeSubChosenCamidx(chosenCam[SUB]);
			}
			break;
			case '1':
					if(SecondDisplayMode==	SECOND_ALL_VIEW_MODE
							||SecondDisplayMode==SECOND_559_ALL_VIEW_MODE)
						p_ForeSightFacade[SUB]->MoveLeft(-PanoLen*100.0,SUB);
				//	else if(SecondDisplayMode==SECOND_TELESCOPE_FRONT_MODE)

						break;
				case '2':
					if(SecondDisplayMode==	SECOND_ALL_VIEW_MODE
							||SecondDisplayMode==SECOND_559_ALL_VIEW_MODE)
						p_ForeSightFacade[SUB]->MoveRight(PanoLen*100.0,SUB);
			//		else if(SecondDisplayMode==SECOND_TELESCOPE_RIGHT_MODE)
									break;
				case '3':
					if(SecondDisplayMode==	SECOND_ALL_VIEW_MODE
							||SecondDisplayMode==SECOND_559_ALL_VIEW_MODE)
						p_ForeSightFacade[SUB]->MoveUp(PanoHeight/(OUTER_RECT_AND_PANO_TWO_TIMES_CAM_LIMIT),SUB);
				//	else if(SecondDisplayMode==SECOND_TELESCOPE_BACK_MODE)
									break;
				case '4':
					if(SecondDisplayMode==	SECOND_ALL_VIEW_MODE
							||SecondDisplayMode==SECOND_559_ALL_VIEW_MODE)
						p_ForeSightFacade[SUB]->MoveDown(-PanoHeight/(OUTER_RECT_AND_PANO_TWO_TIMES_CAM_LIMIT),SUB);
			//		else if(SecondDisplayMode==SECOND_TELESCOPE_LEFT_MODE)
									break;
				case 'O':
					break;
				case 'o':
					break;
				case 'F':
							//full screen on/off
							if(DisFullscreen){
								DisFullscreen = false;
								glutReshapeWindow(g_nonFullwindowWidth, g_nonFullwindowHeight);
								glutPostRedisplay();
							}else{
								DisFullscreen = true;
								glutFullScreen();
							}
							break;
					break;

			default:
		break;
		}
}
void Render::GetFPSDS()
{
	/* Number of samples for frame rate */
#define FR_SAMPLES 10

	static struct timeval last={0,0};
	struct timeval now;
	float delta;
	if (comSecondSC.plusAndGetFrameCount() >= FR_SAMPLES) {
		gettimeofday(&now, NULL);
		delta= (now.tv_sec - last.tv_sec +(now.tv_usec - last.tv_usec)/1000000.0);
		last = now;
		comSecondSC.setFrameRate(FR_SAMPLES / delta);
		comSecondSC.setFrameCount(0);
	}
}
void Render::DrawGLSceneDS()
{
	char arg1[128],arg2[128];
		RenderSceneDS();
		glutSwapBuffers();
		glFinish();
		GetFPSDS();  /* Get frame rate stats */

		/* Copy saved window name into temp string arg1 so that we can add stats */
		strcpy (arg1, comSecondSC.getWindowName());

		if (sprintf(arg2, "%.2f FPS", comSecondSC.getFrameRate()))
		{
			strcat (arg1, arg2);
		}

		/* cut down on the number of redraws on window title.  Only draw once per sample*/
		if (comSecondSC.isCountUpdate())
		{
			glutSetWindowTitle(arg1);
		}
}

void Render::ReSizeGLSceneDS(int Width, int Height)
{
	if (Height==0)	/* Prevent A Divide By Zero If The Window Is Too Small*/
		Height=1;
	ChangeSizeDS(Width, Height);
	comSecondSC.setUpdate(GL_YES);
}
void Render::keyPressedDS(GLEnv &m_env,unsigned char key, int x, int y)
{
	usleep(100);
	ProcessOitKeysDS(m_env,key, x, y);
}



void RenderMain::ReSizeGLSceneDS(int Width, int Height)
{
	render.ReSizeGLSceneDS(Width,Height);
}
void RenderMain::DrawGLSceneDS()
{
		render.DrawGLSceneDS();
		glutPostRedisplay();
}
void RenderMain::keyPressedDS(unsigned char key, int x, int y)
{
	GLEnv &env=env2;
	render.keyPressedDS(env,key,x,y);
}
	void 	RenderMain::doubleScreenInit(int argc, char **argv)
	{
		char arg1[256], arg2[256];
	//	glutInit (&argc, argv);
		glutCreateSubWindow(2,0,0,1920,1080);
		glutInitWindowPosition(1921,0);
		glutInitWindowSize(1920, 1080);

		glutInitDisplayMode ( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

		sprintf(arg1,"Second %s (%s, %s):",VERSION_STRING, __DATE__,__TIME__);
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
			comSecondSC.setWindowName(arg1);

			if (sprintf(arg2, "%.2f FPS", comSecondSC.getFrameRate()))
			{
				strcat (arg1, arg2);
			}

			glutCreateWindow (arg1);


		glutSetCursor(GLUT_CURSOR_NONE);
	//	glewInit();

			glutDisplayFunc(DrawGLSceneDS); /* Register the function to do all our OpenGL drawing. */
			//glutIdleFunc(DrawIdleDS);
			glutReshapeFunc(ReSizeGLSceneDS); /* Register the function called when our window is resized. */
			glutKeyboardFunc(keyPressedDS); /* Register the function called when the keyboard is pressed. */
	//		glutSpecialFunc(specialkeyPressed); /* Register the special key function */
			glutMouseFunc(mouseButtonPressDS); /* Register the function called when the mouse buttons are pressed */
//			glutMotionFunc(mouseMotionPress); /*Register the mouse motion function */
	}

