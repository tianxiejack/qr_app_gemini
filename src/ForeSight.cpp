#if USE_12
#include <iostream>
#include"GLRender.h"
#include"StlGlDefines.h"
#include "OitVehicle.h"
#include"ForeSight.h"
#include"StlGlDefines.h"

using namespace cv;

extern Render render;
ForeSightPos foresightPos[MS_COUNT];
ForeSightPos touch_foresightPos[MS_COUNT];
extern int chosenCam[2];
static float xmove[2]={0,0};
static float ymove[3]={0,0,0};
static float temp_move[2]={0.0,0.0};


float lastposY=0.0;
float camPosY=0.0;

float tel_two_lastposY=0.0;
float tel_two_camPosY=0.0;
float tel_lasttwoposX=0.0;
float tel_lastfourposX=0.0;


float tel_four_lastposY=0.0;
float tel_four_camPosY=0.0;
ForeSight_decorator_fixedpos::	ForeSight_decorator_fixedpos(
		GLMatrixStack &modelViewMat,
		GLMatrixStack	&projectionMat,
		GLShaderManager* mgr,
	//	InterfaceForeSight * interfaceForesight,
		auto_ptr<BaseForeSight> core,
		int i,
		GLfloat p_vTrack[48][3],
		float recvlimitX,
		float recvlimitY):
		ForeSight_decorator(modelViewMat,
				projectionMat,
				 mgr,
				 core,
				 i,
				 p_vTrack,
				 recvlimitX,
				 recvlimitY)
{

}

ForeSight_decorator_fixedposX::	ForeSight_decorator_fixedposX(
		GLMatrixStack &modelViewMat,
		GLMatrixStack	&projectionMat,
		GLShaderManager* mgr,
	//	InterfaceForeSight * interfaceForesight,
		auto_ptr<BaseForeSight> core,
		int i,
		GLfloat p_vTrack[48][3],
		float recvlimitX,
		float recvlimitY):
		ForeSight_decorator(modelViewMat,
				projectionMat,
				 mgr,
				 core,
				 i,
				 p_vTrack,
				 recvlimitX,
				 recvlimitY)
{

}



float ForeSight_decorator_fixedposX::GetPosX(float pos)
{
	char  fixIdx=chosenCam[MAIN];//-768;
	float fixpos=0;
	float panoLen=37.68577;
	float per=panoLen/CAM_COUNT;
	int array[CAM_COUNT]={2,1,8,7,6,5,4,3};
	if(CAM_COUNT%2==1)
	{
		printf("rebuild the code by yourself !\n");
		assert(false);
	}
	for(int i=0;i<CAM_COUNT;i++)
	{
		if(array[i]==fixIdx)
		{
			fixpos=i*per+per/2;
		}
	}
	return fixpos;
}


float ForeSight_decorator_fixedpos::GetPosX(float pos)
{
	char  fixIdx=chosenCam[MAIN];//-768;
	float fixpos=0;
	float panoLen=37.68577;
	float per=panoLen/CAM_COUNT;
	int array[CAM_COUNT]={2,1,8,7,6,5,4,3};
	if(CAM_COUNT%2==1)
	{
		printf("rebuild the code by yourself !\n");
		assert(false);
	}
	for(int i=0;i<CAM_COUNT;i++)
	{
		if(array[i]==fixIdx)
		{
			fixpos=i*per+per/2;
		}
	}
	return fixpos;
}

	ForeSight_decorator::	ForeSight_decorator(GLMatrixStack &modelViewMat,
			GLMatrixStack	&projectionMat,
			GLShaderManager* mgr,
		//	InterfaceForeSight * interfaceForesight,
			auto_ptr<BaseForeSight> core,
			int i,
			GLfloat p_vTrack[48][3],
			float recvlimitX,
			float recvlimitY):
 	m_pShaderManager(mgr),
 	modelViewMatrix(modelViewMat),
 	projectionMatrix(projectionMat),
 //	p_core(interfaceForesight),
 	m_core(core),
 	limitX(recvlimitX),
 	limitY(recvlimitY)
 {
		myBatch.Begin(GL_LINES, i);
		myBatch.CopyVertexData3f(p_vTrack);
		myBatch.End();
 }

 float ForeSight_decorator::GetPosX(float posX)
 {
	 if(posX>=limitX)
	 {
		 posX=limitX;
	 }
	 else if(posX<=-limitX)
	 {
		 posX=-limitX;
	 }
	 return posX;
 }
 float ForeSight_decorator::GetPosY(float posY)
 {
	 if(posY>=limitY)
	 {
		 posY=limitY;
	 }
	 else if(posY<=-limitY)
	 {
		 posY=-limitY;
	 }
	 return posY;
 }

 void ForeSight_decorator::DrawSeveralpairs(GLEnv &m_env,float posX,float posY,float readAngle,int mainorsub)
 {
	 m_core->DrawSeveralpairs(m_env,posX,posY,readAngle,mainorsub);
//	 foresightPosxy.GetSpeedX()
	 if(readAngle <180.0 ||readAngle>270)
		 	{
			 	 foresightPos[mainorsub].SetAlignPos(2,1.25);
		 	}
		 	else
		 	{
		 		foresightPos[mainorsub].SetAlignPos(2,-1.75);
		 	}

			for(int i=0;i< foresightPos[mainorsub].GetAlignNum();i++)
			{
				modelViewMatrix.PushMatrix();
				foresightPos[mainorsub].AlignTo(foresightPos[mainorsub].GetAlignIndex()+i);     //1/2
				if(foresightPos[mainorsub].GetAlignIndex()>=TRACK_VGA)
				{
					modelViewMatrix.Translate(foresightPos[mainorsub].GetMatrixX()+GetPosX(posX), 0.0,GetPosY(posY));
					Draw(m_env);
				}
				else
				{
					modelViewMatrix.Translate(foresightPos[mainorsub].GetMatrixX()+GetPosX(posX)+foresightPos[mainorsub].GetxDelta(), 0.0,GetPosY(posY));
					Drawpairs(m_env);
				}
					//+foresightPosxy.getxDelta()+foresightPosxy.GetForeSightPosX(),0.0f,foresightPosxy.GetForeSightPosY()
				modelViewMatrix.PopMatrix();
			}
 }

 void ForeSight_decorator::Drawpairs(GLEnv &m_env,int mainorsub)
 {
	float flag=1.0;//to put  frames before  or behind the picture
	for(int i=0;i<2;i++)
	{
		modelViewMatrix.PushMatrix();
		modelViewMatrix.Translate(0.0f,(foresightPos[mainorsub].GetMatrixY())*flag,0.0f);
		Draw(m_env);
		modelViewMatrix.PopMatrix();
		flag*= -1.0;
	}
 }
void ForeSight_decorator::Draw( GLEnv &m_env)
{
	GLGeometryTransform * pTransformPipeline = (GLGeometryTransform *)getDefaultTransformPipeline(m_env);
	modelViewMatrix.PushMatrix();
	m_pShaderManager->UseStockShader(GLT_SHADER_FLAT,  pTransformPipeline->GetModelViewProjectionMatrix(), vBlue);
	myBatch.Draw();
	modelViewMatrix.PopMatrix();
 }

  ForeSightPos::ForeSightPos(): yOffset(-0.01),xDelta(0.0),foresightPosX(0.0),foresightPosY(0.0),
		  alignIndex(0),alignNum(1),waitIntheleft(true),waitIntheright(true),trackPosX(0),trackPosY(0),touch_delta_angle(13.0)
{
	  for(int i=0;i<SEND_TRACK_NUM;i++)
	  {
		  sendtotrack[i]=0;
	  }
	  for(int i=0;i<RECV_TRACK_NUM;i++)
	  {
		  recvfromtrack[i]=0;
	  }

	  speedXY[0]=1.0;
	  speedXY[1]=1.0;
	  ForeSight_Pos[0]=-0.75;//-0.75;
	  	ForeSight_Pos[1]=0.25;//0.25;
	  	ForeSight_Pos[2]=1.25;

	  	ForeSight_Pos[3]=0.25;
	  	ForeSight_Pos[4]=0.5;
	  	ForeSight_Pos[5]=-0.25;
	  	ForeSight_Pos[6]=0;

		ForeSight_Pos[7]=0;
//		ForeSight_Pos[8]=0;
//		ForeSight_Pos[9]=0;
}


  bool ForeSightPos::MoveRight(float Xlimit)
	{
		waitIntheleft=true;
		foresightPosX+=speedXY[0];
		if(foresightPosX>pano_length)
		{
			foresightPosX-=pano_length;
		}

		  if(foresightPosX>=Xlimit)
		 {
			//	 printf("foresightPosX=%f\n",foresightPosX/pano_length*360.0);
			 foresightPosX=Xlimit;
			  if(waitIntheright)
			  {
				  waitIntheright=false;
			  }
			  else
			  {
				  return true;
			  }
		  }
		//  printf("foresightPosX=%f\n",foresightPosX/pano_length*360.0);
		  return false;
	}

  bool ForeSightPos::MoveLeft(float Xlimit)
	{
		waitIntheright=true;
		foresightPosX-=speedXY[0];
		if(foresightPosX<-pano_length)
		{
			foresightPosX+=pano_length;
		}
		if(foresightPosX<=Xlimit)
		 {
			 foresightPosX=Xlimit;
	//		 printf("foresightPosX=%f\n",foresightPosX/pano_length*360.0);
			if(waitIntheleft)
			{
				waitIntheleft=false;
			 }
			else
			{
				return true; //change displayMode
			}
		}
//		 printf("foresightPosX=%f\n",foresightPosX/pano_length*360.0);
		  return false;
	}

	void ForeSightPos::SetPos(float Xangle,float Yangle,int mainOrsub)
	{
		int dd=2;
		foresightPos[mainOrsub].SetSpeedX(0);
		foresightPos[mainOrsub].SetSpeedY(0);
		if(Yangle>=288+touch_delta_angle-dd && Yangle<=347+touch_delta_angle)
		{
			if(Xangle>=180.0)
			{
				foresightPosX=pano_length/4.0/180.0*(Xangle-180.0);
			}
			else
			{
				if(Xangle==0)
				{
					Xangle=1;
				}
				foresightPosX=pano_length*3.0/4.0+(Xangle)/180.0*pano_length/4.0;
			}
		}
		else if(Yangle>=216+touch_delta_angle-dd && Yangle<=275+touch_delta_angle)//Down
		{
			if(Xangle>=180.0)
			{
				foresightPosX=pano_length/2.0-pano_length/4.0*(Xangle-180.0)/180.0;
			}
			else
			{
				if(Xangle==0)
				{
					Xangle=1;
				}
				foresightPosX=pano_length*3.0/4.0-(Xangle)/180.0*pano_length/4.0;
			}
		}

		if(Yangle>=288+touch_delta_angle-dd &&Yangle<=347+touch_delta_angle)
		{
			foresightPosY=(Yangle-288-touch_delta_angle+dd)/61.0*2.25-2.25/2;
		}

		else if (Yangle>=216+touch_delta_angle-dd &&Yangle<=275+touch_delta_angle )
		{
			foresightPosY=(Yangle-216-touch_delta_angle+dd)/61.0*2.25-2.25/2;
		}
		render.GetpWholeFacade(mainOrsub)->MoveLeft(-render.GetPanoLen()*100.0,mainOrsub);
		if((Yangle>=288+touch_delta_angle-dd &&Yangle<328+touch_delta_angle) ||(Yangle>=216+touch_delta_angle-dd &&Yangle<256+touch_delta_angle))
			render.GetpWholeFacade(mainOrsub)->MoveDown(-render.GetPanoHeight()/CORE_AND_POS_LIMIT,mainOrsub);
			else if((Yangle>=328+touch_delta_angle &&Yangle<=347+touch_delta_angle)||(Yangle>=256+touch_delta_angle &&Yangle<276+touch_delta_angle) )
			render.GetpWholeFacade(mainOrsub)->MoveUp(render.GetPanoHeight()/CORE_AND_POS_LIMIT,mainOrsub);
	}


	bool ForeSightPos::Reset(int nowmode)
	{
		static int lastMode =-1;
		if(nowmode!=lastMode)
		{
			lastMode=nowmode;
			foresightPosX=0.0;
			foresightPosY=0.0;
			waitIntheright=true;
			waitIntheleft=true;
			return true;
		}
		return false;
	}

	bool ForeSightPos::TrackMoveLeft(float Xlimit)
	{
		foresightPosX-=speedXY[0];
		 if(foresightPosX<Xlimit)
		 {
			 foresightPosX+=-Xlimit*2.0;
		 }
	}
	bool ForeSightPos::TrackMoveRight(float Xlimit)
	{
		foresightPosX+=speedXY[0];
		 if(foresightPosX>Xlimit)
		 {
			 foresightPosX-=Xlimit*2.0;
		 }
	}
		void ForeSightPos::TrackMoveUp(float Ylimit)
		{
			foresightPosY+=speedXY[1];
			 if(foresightPosY>Ylimit)
			 {
				 foresightPosY-=Ylimit*2.0;
			 }
		}
		void ForeSightPos::TrackMoveDown(float Ylimit)
		{
			foresightPosY-=speedXY[1];
			 if(foresightPosY<Ylimit)
			 {
				 foresightPosY+=-Ylimit*2.0;
			 }
		}

		int *  ForeSightPos::ChangeEnlarge2Ori(GLint orix, GLint oriy, GLint enlargedx, GLint enlargedy, float  enalrged_midx, float enalrged_midy,int trackframeX,int trackframeY,	float Xlimit, float Ylimit)
		{
			float Xratio=0, Yratio=0;
			Xratio=(float)orix/(float)enlargedx;
			Yratio=(float)oriy/(float)enlargedy;
		//	printf("\nenalrged_midx=%f  ******* enalrged_midy=%f\n",enalrged_midx,enalrged_midy);
		//	printf("Xratio=%f\t,Yratio=%f\n",Xratio,Yratio);
			sendtotrack[SEND_TRACK_FRAME_X]=trackframeX;
			sendtotrack[SEND_TRACK_FRAME_Y]=trackframeY;
			enalrged_midx=enalrged_midx/Xlimit/2.0*(float)enlargedx;
			enalrged_midy=enalrged_midy/Ylimit/2.0*(float)enlargedy;
			sendtotrack[SEND_TRACK_START_X]=(int)(enalrged_midx*Xratio-trackframeX/2.0);
			sendtotrack[SEND_TRACK_START_Y]=(int)(enalrged_midy*Yratio-trackframeY/2.0);
			if(sendtotrack[SEND_TRACK_START_X]<0)
			{
				sendtotrack[SEND_TRACK_START_X]=0;
			}
			if(sendtotrack[SEND_TRACK_START_Y]<0)
			{
				sendtotrack[SEND_TRACK_START_Y]=0;
			}
		//	printf("frameX=%d\tframeY=%d\n",trackframeX,trackframeY);
		//	printf("SENDTrackMidX=%d\tSENDTrackMidY=%d\n",sendtotrack[SEND_TRACK_START_X]-trackframeX,sendtotrack[SEND_TRACK_START_Y]-trackframeY);
		//	printf("SENDTrackStartX=%d\tSENDTrackStartY=%d\n",sendtotrack[SEND_TRACK_START_X],sendtotrack[SEND_TRACK_START_Y]);
			return sendtotrack;
		}

				int * ForeSightPos::ChangeOri2Enlarge(GLint orix, GLint oriy, GLint enlargedx, GLint enlargedy,int ori_midx,int ori_midy,int trackframeX,int trackframeY,	float Xlimit, float Ylimit)
				{
		/*			float Xratio=0, Yratio=0;
					Xratio=(float)enlargedx/(float)orix;
					Yratio=(float)enlargedy/(float)oriy;
					recvfromtrack[RECV_TRACK_FRAME_X]=trackframeX;
					recvfromtrack[RECV_TRACK_FRAME_Y]=trackframeY;
					recvfromtrack[RECV_TRACK_START_X]=(int)(ori_midx*Xratio+trackframeX);
					recvfromtrack[RECV_TRACK_START_Y]=(int)(ori_midy*Yratio+trackframeY);
					return recvfromtrack;*/
				}



				float ForeSightPos::Change2TrackPosX(float Xlimit)
				{
			//		printf("\nlimitX=%f\n",Xlimit);
		//			printf("foresightPosX=%f\n",foresightPosX);
					trackPosX=foresightPosX+Xlimit;
	//				printf("trackPosX=%f\n",trackPosX);
					return trackPosX;
				}
				float ForeSightPos::Change2TrackPosY(float Ylimit)
				{
		//			printf("\nlimitY=%f\n",Ylimit);
		//			printf("foresightPosY=%f\n",foresightPosY);
					trackPosY=foresightPosY+Ylimit;
					trackPosY=-trackPosY+2*Ylimit;
		//			printf("trackPosY=%f\n",trackPosY);
					return trackPosY;
				}

	void ForeSightPos::MoveUp(float Ylimit)
	{
		if(foresightPosY+speedXY[1]>Ylimit)
		{
			foresightPosY=Ylimit;
		}
		else
		{
			foresightPosY+=speedXY[1];
		}
	}

	void ForeSightPos::MoveDown(float Ylimit)
	{
		if(foresightPosY-speedXY[1]<Ylimit)
		{
			foresightPosY=Ylimit;
		}
		else
		{
			foresightPosY-=speedXY[1];
		}
	}

	float ForeSightPos::GetForeSightPosX( )
	{
		return foresightPosX;
	}
	float ForeSightPos::GetForeSightPosY( )
	{
		return foresightPosY;
	}

	float * ForeSightPos::GetAngle()
	{
		float PosX=foresightPosX;
		float PosY=foresightPosY;
		if(PosX<0 )
		{
			PosX+=pano_length;
		}
		else if(PosX>pano_length)
		{
			PosX-=pano_length;
		}
		angle[0]=PosX/pano_length*360.0;
	//	printf("angleX=%f\n",angle[0]);
		angle[1]=PosY/pano_height*5.7*360.0/2.0;
		if(angle[1]>180.0)
		{
			angle[1]=180.0;
		}
		else if (angle[1]<-180.0)
		{
			angle[1]=-180.0;
		}
	//	printf("angleY=%f\n",angle[1]);
//		ChangeA2M();
		return angle;
	}
	float * ForeSightPos::GetTelAngle(int idx)
	{
		float PosX=foresightPosX;
		float PosY=foresightPosY;
		angle[0]=PosX/pano_length*360.0+idx*90.0;
		if(angle[0]<0.0)
		{
			angle[0]+=360.0;
		}
	//		printf("angleX=%f\n",angle[0]);
		angle[1]=PosY/pano_height*5.7*360.0/2.0;
		//printf("angleY=%f\n",angle[1]);
	}
	int *ForeSightPos::ChangeTelA2M()
	{
		mil[0]=(int)angle[0]/360.0*6000.0;
		mil[1]=(int) (angle[1]+180.0)/360.0*1167.0-167.0;
		//printf("mil[0]=%d\n",mil[0]);
		//	printf("mil[1]=%d\n",mil[1]);
		return mil;
	}
	int * ForeSightPos::ChangeA2M()
	{
		mil[0]=(int)angle[0]/360.0*6000.0;
		mil[1]=(int) (angle[1]+180.0)/360.0*1167.0-167.0;
	//	printf("mil[0]=%d\n",mil[0]);
	//	printf("mil[1]=%d\n",mil[1]);
		return mil;
	}


	void ForeSightPos::SetSpeedX(float recvSpeedX)
	{
		speedXY[0]=recvSpeedX;
	}
	void ForeSightPos::SetSpeedY(float recvSpeedY)
	{
		speedXY[1]=recvSpeedY;
	}

	/*float * ForeSightPos::GetAngle(foresight_assembly assembly )
	{

	}
	int *ForeSightPos::ChangeA2M(foresight_assembly assembly)
	{

	}*/

	float ForeSightPos::GetMatrixX()
	{
		return this->xOffset;
	}
	void ForeSightPos::AlignTo(int num)
	{
		float delta=1.0/CAM_COUNT/2;
		this->xOffset=pano_length*(ForeSight_Pos[num]+delta);
	}
	void ForeSightFacade::MoveUp(float Ylimit,int mainorsub)
	{
		foreSightPos.MoveUp( Ylimit);
	//	printf("foresightPOSY=%f\n",foreSightPos.GetForeSightPosY());
		pcamonForeSight->CamMoveUp(mainorsub);
	//	camonForeSight.pano_CamMoveUp();
	//	camonForeSight.tel_CamTwoMoveUp();
	//	camonForeSight.tel_CamFourMoveUp();
	}
	void ForeSightFacade::MoveDown(float Ylimit,int mainorsub)
	{
		foreSightPos.MoveDown( Ylimit);
//		printf("foresightPOSY=%f\n",foreSightPos.GetForeSightPosY());
		pcamonForeSight->CamMoveDown(mainorsub);
//		camonForeSight.pano_CamMoveDown();
//		camonForeSight.tel_CamTwoMoveDown();
//		camonForeSight.tel_CamFourMoveDown();
	}
	bool  ForeSightFacade::MoveLeft(float Xlimit,int mainorsub)
	{
		bool istochangeTelMode=false;
		istochangeTelMode=foreSightPos.MoveLeft(Xlimit);
		pcamonForeSight->CamMoveLeft(mainorsub);
//		camonForeSight.pano_CamMoveX();
//		camonForeSight.tel_CamMoveTwoLeft();
//		camonForeSight.tel_CamMoveFourLeft();
		return istochangeTelMode;
	}
	bool  ForeSightFacade::MoveRight(float Xlimit,int mainorsub)
	{
		bool istochangeTelMode=false;
		istochangeTelMode=foreSightPos.MoveRight(Xlimit);
		pcamonForeSight->CamMoveRight(mainorsub);
//		camonForeSight.pano_CamMoveX();
//		camonForeSight.tel_CamMoveTwoRight();
	//	camonForeSight.tel_CamMoveFourRight();
		return istochangeTelMode;
	}
	void ForeSightFacade::Draw(GLEnv &m_env,float readAngle,int mainorsub)//在渲染时间时调用
	{
		pInterfaceForeSight->DrawSeveralpairs(m_env,foreSightPos.GetForeSightPosX(),foreSightPos.GetForeSightPosY(), readAngle,mainorsub);
	}


	PanoCamOnForeSight::PanoCamOnForeSight():pano_length(0),pano_height(0),
			pano_two_lastposX(0),pano_two_lastposY(0),pano_one_lastposX(0),pano_one_lastposY(0)
	{
		for(int i=0;i<2;i++)
		{
		OnetimeViewCameraFrame.SetOrigin(0.0,0.0,0.0);
		TwotimesViewCameraFrame2.SetOrigin(0.0,0.0,0.0);
		OnetimeViewCameraFrame.SetOrigin(0.0,0.0,0.0);
		TwotimesViewCameraFrame2.SetOrigin(0.0,0.0,0.0);
		}
		Front=true;
	}



	void  PanoCamOnForeSight::pano_CamMoveX(int mainorsub)
		{
			float PosX=foresightPos[mainorsub].GetForeSightPosX();
			if(PosX<-pano_length/4.0 && PosX>-pano_length*3.0/4.0)
			{
				Front=false;
			}
			else if(PosX>pano_length*1.0/4.0 &&PosX<pano_length*3.0/4.0)
			{
				Front=false;
			}
			else
			{
				Front=true;
			}
			if(PosX<-pano_length/4.0 )
			{
				PosX+=pano_length;
			}
			else if(PosX>pano_length*3.0/4.0)
			{
				PosX-=pano_length;
			}
			float moveX=0.0;
			static float camPosX=0.0;
			moveX=PosX-pano_two_lastposX;
			camPosX+=moveX;
	//		printf("camPosXANGLE=%f\n",PosX/pano_length*360.0);
	//		printf("camPosAngle=%f\n",camPosX/pano_length*360.0);
			pano_two_lastposX=PosX;
			OnetimeViewCameraFrame.MoveRight(-moveX);
			OnetimeViewCameraFrame2.MoveRight(moveX);
			TwotimesViewCameraFrame.MoveRight(-moveX);
			TwotimesViewCameraFrame2.MoveRight(moveX);
		}

	void PanoCamOnForeSight::pano_OnetimeCamMoveUp(int mainorsub)
	{
		float limit=pano_height/(OUTER_RECT_AND_PANO_TWO_TIMES_CAM_LIMIT);
					float PosY=foresightPos[mainorsub].GetForeSightPosY();
					if(PosY<-limit)
					{
						return;
					}
					 if(PosY>limit)
					{
						PosY=limit;
					}
					float moveY=0.0;
					moveY=PosY-pano_one_lastposY;
			//		camPosY+=moveY;
					pano_one_lastposY=PosY;
					getOneTimeCam().MoveUp(moveY);
					getOneTimeCam2().MoveUp(moveY);
	}
	void PanoCamOnForeSight::pano_OnetimeCamMoveDown(int mainorsub)
	{
		float limit=-pano_height/(OUTER_RECT_AND_PANO_TWO_TIMES_CAM_LIMIT);
		float PosY=foresightPos[mainorsub].GetForeSightPosY();
		if(PosY>-limit)
		{
			return;
		}
		if(PosY<limit)
		{
			PosY=limit;
		}
		float moveY=0.0;
		moveY=PosY-pano_one_lastposY;
//		camPosY+=moveY;
		pano_one_lastposY=PosY;
		getOneTimeCam().MoveUp(moveY);
		getOneTimeCam2().MoveUp(moveY);
	}
	void PanoCamOnForeSight::pano_TwotimesCamMoveUp(int mainorsub)
	{
		float limit=pano_height/(INNER_RECT_AND_PANO_ONE_TIME_CAM_LIMIT);
					float PosY=foresightPos[mainorsub].GetForeSightPosY();
					if(PosY<-limit)
					{
						return;
					}
					 if(PosY>limit)
					{
						PosY=limit;
					}
					float moveY=0.0;
					moveY=PosY-pano_two_lastposY;
			//		camPosY+=moveY;
					pano_two_lastposY=PosY;

					getTwoTimesCam().MoveUp(moveY);
					getTwoTimesCam2().MoveUp(moveY);
	}
	void PanoCamOnForeSight::pano_TwotimesCamMoveDown(int mainorsub)
	{
		float limit=-pano_height/(INNER_RECT_AND_PANO_ONE_TIME_CAM_LIMIT);
		float PosY=foresightPos[mainorsub].GetForeSightPosY();
		if(PosY>-limit)
		{
			return;
		}
		if(PosY<limit)
		{
			PosY=limit;
		}
		float moveY=0.0;
		moveY=PosY-pano_two_lastposY;
//		camPosY+=moveY;
		pano_two_lastposY=PosY;
		TwotimesViewCameraFrame.MoveUp(moveY);
		TwotimesViewCameraFrame2.MoveUp(moveY);
	}
	void PanoCamOnForeSight::pano_CamMoveUp(int mainorsub)
	{
		pano_OnetimeCamMoveUp(mainorsub);
	//	pano_TwotimesCamMoveUp(mainorsub);
	}

	void PanoCamOnForeSight::pano_CamMoveDown(int mainorsub)
	{
		pano_OnetimeCamMoveDown(mainorsub);
	//	pano_TwotimesCamMoveDown(mainorsub);
	}




TelCamOnForeSight::TelCamOnForeSight():pano_length(0),pano_height(0),
		 tel_two_lastposX(0),
		 tel_four_lastposX(0),
		 tel_two_lastposY(0),
		 tel_four_lastposY(0)
{
	for(int i=0;i<4;i++){
	TwotimesTelViewCameraFrame[i].SetOrigin(0.0,0.0,0.0);
	FourtimesTelViewCameraFrame[i].SetOrigin(0.0,0.0,0.0);
	}
}

void  TelCamOnForeSight::tel_CamMoveTwoLeft(int mainorsub)
	{
		float limit=(pano_length/TELXLIMIT-(1/14.0-1/25.0)*pano_length);
		float PosX=foresightPos[mainorsub].GetForeSightPosX();
		float moveX=0.0;
		if(PosX>limit)
		{
			return;
		}
		else if(PosX<-limit)
		{
			PosX=-limit;
		}
		moveX=PosX-tel_two_lastposX;
		tel_two_lastposX=PosX;
		getTwoTimesCamTelF().MoveRight(-moveX);
		getTwoTimesCamTelR().MoveRight(-moveX);
		getTwoTimesCamTelB().MoveRight(-moveX);
		getTwoTimesCamTelL().MoveRight(-moveX);
	}

void  TelCamOnForeSight::tel_CamMoveTwoRight(int mainorsub)
	{
		float limit=(pano_length/TELXLIMIT-(1/14.0-1/25.0)*pano_length);
		float PosX=foresightPos[mainorsub].GetForeSightPosX();
		float moveX=0.0;
		if(PosX<-limit)
			{
				return;
			}
			else if(PosX>limit)
			{
				PosX=limit;
			}
		moveX=PosX-tel_two_lastposX;
		tel_two_lastposX=PosX;
		getTwoTimesCamTelF().MoveRight(-moveX);
		getTwoTimesCamTelR().MoveRight(-moveX);
		getTwoTimesCamTelB().MoveRight(-moveX);
		getTwoTimesCamTelL().MoveRight(-moveX);
	}




void  TelCamOnForeSight::tel_CamMoveFourLeft(int mainorsub)
	{
			float limit=pano_length/TELXLIMIT-((1.0/14.0-1.0/15.75)*pano_length);
			float PosX=foresightPos[mainorsub].GetForeSightPosX();
			float moveX=0.0;
			if(PosX>limit)
			{
				return;
			}
			else if(PosX<-limit)
			{
				PosX=-limit;
			}
			moveX=PosX-tel_four_lastposX;
			tel_four_lastposX=PosX;
			getFourTimesCamTelF().MoveRight(-moveX);
			getFourTimesCamTelR().MoveRight(-moveX);
			getFourTimesCamTelB().MoveRight(-moveX);
			getFourTimesCamTelL().MoveRight(-moveX);
	}

void  TelCamOnForeSight::tel_CamMoveFourRight(int mainorsub)
	{
			float limit=pano_length/TELXLIMIT-((1.0/14.0-1.0/15.75)*pano_length);
			float PosX=foresightPos[mainorsub].GetForeSightPosX();
			float moveX=0.0;
			if(PosX<-limit)
			{
				return;
			}
			else if(PosX>limit)
			{
				PosX=limit;
			}
			moveX=PosX-tel_four_lastposX;
			tel_four_lastposX=PosX;
			getFourTimesCamTelF().MoveRight(-moveX);
			getFourTimesCamTelR().MoveRight(-moveX);
			getFourTimesCamTelB().MoveRight(-moveX);
			getFourTimesCamTelL().MoveRight(-moveX);
	}

void TelCamOnForeSight::tel_CamTwoMoveUp(int mainorsub)
	{
		float two_limit=pano_height/12;
		float two_PosY=foresightPos[mainorsub].GetForeSightPosY();
		if(two_PosY<-two_limit)
		{
			return;
		}
		else if(two_PosY>two_limit)
		{
			 two_PosY=two_limit;
		}
		float two_moveY=0.0;
		two_moveY=two_PosY-tel_two_lastposY;
		tel_two_lastposY=two_PosY;
		getTwoTimesCamTelF().MoveUp(two_moveY);
		getTwoTimesCamTelR().MoveUp(two_moveY);
		getTwoTimesCamTelB().MoveUp(two_moveY);
		getTwoTimesCamTelL().MoveUp(two_moveY);
	}

void TelCamOnForeSight::tel_CamTwoMoveDown(int mainorsub)
	{
		float two_limit=-pano_height/12;
		float two_PosY=foresightPos[mainorsub].GetForeSightPosY();
		if(two_PosY>-two_limit)
		{
			return;
		}
		else if(two_PosY<two_limit)
		{
			two_PosY=two_limit;
		}
		float two_moveY=0.0;
		two_moveY=two_PosY-tel_two_lastposY;
		tel_two_lastposY=two_PosY;
		getTwoTimesCamTelF().MoveUp(two_moveY);
		getTwoTimesCamTelR().MoveUp(two_moveY);
		getTwoTimesCamTelB().MoveUp(two_moveY);
		getTwoTimesCamTelL().MoveUp(two_moveY);
	}

void TelCamOnForeSight::tel_CamFourMoveUp(int mainorsub)
	{
		float four_limit=pano_height/8;
			float four_PosY=foresightPos[mainorsub].GetForeSightPosY();
			if(four_PosY<-four_limit)
			{
				return;
			}
			else if(four_PosY>four_limit)
			{
				 four_PosY=four_limit;
			}
			 float four_moveY=0.0;
			four_moveY=four_PosY-tel_four_lastposY;
			tel_four_lastposY=four_PosY;
			getFourTimesCamTelF().MoveUp(four_moveY);
			getFourTimesCamTelR().MoveUp(four_moveY);
			getFourTimesCamTelB().MoveUp(four_moveY);
			getFourTimesCamTelL().MoveUp(four_moveY);
	}
	void TelCamOnForeSight::tel_CamFourMoveDown(int mainorsub)
	{
		float four_limit=-pano_height/8;
			float four_PosY=foresightPos[mainorsub].GetForeSightPosY();
			if(four_PosY>-four_limit)
			{
				return;
			}
		else if(four_PosY<four_limit)
			{
				four_PosY=four_limit;
			}
			float four_moveY=0.0;
			four_moveY=four_PosY-tel_four_lastposY;
			tel_four_lastposY=four_PosY;
			getFourTimesCamTelF().MoveUp(four_moveY);
			getFourTimesCamTelR().MoveUp(four_moveY);
			getFourTimesCamTelB().MoveUp(four_moveY);
			getFourTimesCamTelL().MoveUp(four_moveY);
	}


#endif
