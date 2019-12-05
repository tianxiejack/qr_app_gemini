#if USE_12
#ifndef FORESIGHT_H_
#define FORESIGHT_H_
#include"StlGlDefines.h"
#include"auto_ptr.h"

typedef class InterfaceForeSight{
public:
	virtual void DrawSeveralpairs(GLEnv &m_env,float posX,float posY,float readAngle,int mainorsub=MAIN)=0;
} *PInterfaceForeSight;

class BaseForeSight:public InterfaceForeSight
{
public :
	BaseForeSight(){};
	virtual ~BaseForeSight(){};
};

//core
class PseudoForeSight_core:public BaseForeSight
{
public:
	virtual void DrawSeveralpairs(GLEnv &m_env,float posX,float posY,float readAngle,int mainorsub=MAIN){};
};

//decorator
class ForeSight_decorator:public BaseForeSight
{
public:
	ForeSight_decorator(GLMatrixStack &modelViewMat,
			GLMatrixStack	&projectionMat,
			GLShaderManager* mgr,
			auto_ptr<BaseForeSight> core,
			int i,
			GLfloat p_vTrack[48][3],
			float recvlimitX,
			float  recvlimitY);
	virtual ~ForeSight_decorator(){};
	virtual void DrawSeveralpairs(GLEnv &m_env,float posX,float posY,float readAngle,int mainorsub=MAIN);
protected:
	virtual float GetPosX(float pos);
	virtual float GetPosY(float pos);
	void Drawpairs(GLEnv &m_env,int mainorsub=MAIN);
	void Draw(GLEnv &m_env);
private:
	float limitX;
	float limitY;
	GLBatch myBatch;
	GLShaderManager * 	m_pShaderManager;
	GLMatrixStack &		modelViewMatrix;
	GLMatrixStack &		projectionMatrix;
	auto_ptr<BaseForeSight> m_core;
};

class ForeSight_decorator_fixedpos:public ForeSight_decorator
{
public:
	ForeSight_decorator_fixedpos(GLMatrixStack &modelViewMat,
			GLMatrixStack	&projectionMat,
			GLShaderManager* mgr,
			auto_ptr<BaseForeSight> core,
			int i,
			GLfloat p_vTrack[48][3],
			float recvlimitX,
			float  recvlimitY);
	virtual ~ForeSight_decorator_fixedpos(){};
protected:
	virtual float GetPosX(float pos);
	virtual float GetPosY(float pos){return 0;};
};

class ForeSight_decorator_fixedposX:public ForeSight_decorator
{
public:
	ForeSight_decorator_fixedposX(GLMatrixStack &modelViewMat,
			GLMatrixStack	&projectionMat,
			GLShaderManager* mgr,
			auto_ptr<BaseForeSight> core,
			int i,
			GLfloat p_vTrack[48][3],
			float recvlimitX,
			float  recvlimitY);
	virtual ~ForeSight_decorator_fixedposX(){};
protected:
	virtual float GetPosX(float pos);
};

typedef class InterfaceCamonForeSight{
public:
	virtual void CamMoveUp(int mainorsub=MAIN)=0;
	virtual void CamMoveDown(int mainorsub=MAIN)=0;
	virtual void CamMoveLeft(int mainorsub=MAIN)=0;
	virtual void CamMoveRight(int mainorsub=MAIN)=0;
} *PInterfaceCamonForeSight;

class PseudoForeSight_cam:public InterfaceCamonForeSight
{
	 void CamMoveUp(int mainorsub=MAIN){};
	 void CamMoveDown(int mainorsub=MAIN){};
	 void CamMoveLeft(int mainorsub=MAIN){};
	 void CamMoveRight(int mainorsub=MAIN){};
};


class PanoCamOnForeSight:public InterfaceCamonForeSight
{
public:
	PanoCamOnForeSight();
	~PanoCamOnForeSight(){};
	bool GetFront(){return Front;};
	GLFrame  &getOneTimeCam(){return OnetimeViewCameraFrame;};
	GLFrame  &getTwoTimesCam(){return TwotimesViewCameraFrame;};
	GLFrame  &getOneTimeCam2(){return OnetimeViewCameraFrame2;};
	GLFrame  &getTwoTimesCam2(){return TwotimesViewCameraFrame2;};
	void CamMoveUp(int mainorsub=MAIN){pano_CamMoveUp(mainorsub);};
	 void CamMoveDown(int mainorsub=MAIN){pano_CamMoveDown(mainorsub);};
	 void CamMoveLeft(int mainorsub=MAIN){pano_CamMoveX(mainorsub);};
	 void CamMoveRight(int mainorsub=MAIN){ pano_CamMoveX(mainorsub);};

	void setPanolen(float len){pano_length=len;};
	void setPanoheight(float height){pano_height=height;};
private:
	void pano_CamMoveX(int mainorsub=MAIN);
	void pano_CamMoveUp(int mainorsub=MAIN);
	void pano_CamMoveDown(int mainorsub=MAIN);
	void pano_OnetimeCamMoveUp(int mainorsub=MAIN);
	void pano_OnetimeCamMoveDown(int mainorsub=MAIN);
	void pano_TwotimesCamMoveUp(int mainorsub=MAIN);
	void pano_TwotimesCamMoveDown(int mainorsub=MAIN);
	float pano_one_lastposX;
	float pano_one_lastposY;
	float pano_two_lastposX;
	float pano_two_lastposY;
	float pano_length;
	float pano_height;
	bool Front;
	GLFrame OnetimeViewCameraFrame;
	GLFrame OnetimeViewCameraFrame2;
	GLFrame TwotimesViewCameraFrame;
	GLFrame TwotimesViewCameraFrame2;
};

class TelCamOnForeSight:public InterfaceCamonForeSight
{
public:
	TelCamOnForeSight();
	~TelCamOnForeSight(){};
	GLFrame  &getTwoTimesCamTelF(){return TwotimesTelViewCameraFrame[0];};
	GLFrame  &getFourTimesCamTelF(){return FourtimesTelViewCameraFrame[0];};
	GLFrame  &getTwoTimesCamTelR(){return TwotimesTelViewCameraFrame[1];};
	GLFrame  &getFourTimesCamTelR(){return FourtimesTelViewCameraFrame[1];};
	GLFrame  &getTwoTimesCamTelB(){return TwotimesTelViewCameraFrame[2];};
	GLFrame  &getFourTimesCamTelB(){return FourtimesTelViewCameraFrame[2];};
	GLFrame  &getTwoTimesCamTelL(){return TwotimesTelViewCameraFrame[3];};
	GLFrame  &getFourTimesCamTelL(){return FourtimesTelViewCameraFrame[3];};

	 void CamMoveUp(int mainorsub=MAIN){
		 tel_CamTwoMoveUp(mainorsub);
		 tel_CamFourMoveUp(mainorsub);
	 };
	 void CamMoveDown(int mainorsub=MAIN){
		 tel_CamTwoMoveDown(mainorsub);
		 tel_CamFourMoveDown(mainorsub);
	 };
	 void CamMoveLeft(int mainorsub=MAIN){
		 tel_CamMoveTwoLeft(mainorsub);
		 tel_CamMoveFourLeft(mainorsub);
	 };
	 void CamMoveRight(int mainorsub=MAIN){
		 tel_CamMoveTwoRight(mainorsub);
		 tel_CamMoveFourRight(mainorsub);
	 };
	void setPanolen(float len){pano_length=len;};
	void setPanoheight(float height){pano_height=height;};
private:
	void tel_CamMoveTwoLeft(int mainorsub=MAIN);
	void tel_CamMoveTwoRight(int mainorsub=MAIN);
	void tel_CamMoveFourLeft(int mainorsub=MAIN);
	void tel_CamMoveFourRight(int mainorsub=MAIN);
	void tel_CamTwoMoveUp(int mainorsub=MAIN);
	void tel_CamTwoMoveDown(int mainorsub=MAIN);
	void tel_CamFourMoveUp(int mainorsub=MAIN);
	void tel_CamFourMoveDown(int mainorsub=MAIN);
	float tel_two_lastposX;
	float tel_four_lastposX;
	float tel_two_lastposY;
	float tel_four_lastposY;
	float pano_length;
	float pano_height;
	GLFrame TwotimesTelViewCameraFrame[4];
	GLFrame FourtimesTelViewCameraFrame[4];
};

class ForeSightPos
	{
	public:
		ForeSightPos();  //全局只有一个位置，外框1外框2可通过中心点的位置计算
		~ForeSightPos(){};
		void SetPos(float Xangle,float Yangle,int mainOrsub);
		void SetPanoLen_Height(float len,float height){pano_length=len,pano_height=height;};
		void MoveUp(float Ylimit);
		void MoveDown(float Ylimit);
		bool MoveLeft(float Xlimit);
		bool MoveRight(float Xlimit);
			void TrackMoveUp(float Ylimit);
			void TrackMoveDown(float Ylimit);
			bool TrackMoveLeft(float Xlimit);
			bool TrackMoveRight(float Xlimit);
			int *ChangeEnlarge2Ori(GLint orix, GLint oriy,
					GLint enlargedx, GLint enlargedy,
					float enalrged_midx, float enalrged_midy,
					int trackframeX,int trackframeY,
					float Xlimit,float Ylimit);
			int *ChangeOri2Enlarge(GLint orix, GLint oriy,
					GLint enlargedx, GLint enlargedy,
					int ori_midx,int ori_midy,
					int trackframeX,int trackframeY,
					float Xlimit, float Ylimit);
			float Change2TrackPosX(float Xlimit);
			float Change2TrackPosY(float Ylimit);
			void ShowPosX(){
				printf("foresightPosX=%f\n",foresightPosX);
			};
			void ShowPosY(){
				printf("foresightPosY=%f\n",foresightPosY);
			}
		bool Reset(int nowmode);
		void SetSpeedX(float recvSpeedX);
		void SetSpeedY(float recvSpeedY);
		float GetForeSightPosX( );
		float GetForeSightPosY( );
		void SetxDelta(float xdelta){this->xDelta=xdelta;};
		float GetxDelta(){return xDelta;};
		float * GetAngle();
		int *ChangeA2M();
		float * GetTelAngle(int idx);
		int *ChangeTelA2M( );
		void AlignTo(int index);
		void SetAlignindex(int index){alignIndex=index;};
		int GetAlignIndex(){return alignIndex;};
		void SetAlignNum(int num){alignNum=num;};
		int GetAlignNum(){return alignNum;};
		void SetAlignPos(int index,float pos){ForeSight_Pos[index]=pos;};
		 float GetMatrixY(){return yOffset;};
		float GetMatrixX();
		int *Getsendtotrack(){return sendtotrack;};
		void initsendtotrack(){
			for(int i=0;i<SEND_COUNT;i++)
			{
				sendtotrack[i]=-1;
			}
		}
	private:
		int sendtotrack[SEND_TRACK_NUM];
		int recvfromtrack[RECV_TRACK_NUM];
		float trackPosX;
		float trackPosY;
		bool waitIntheleft;
		bool waitIntheright;
		int alignIndex;
		int alignNum;
		float xOffset;
		float yOffset;
		float xDelta;
		float ForeSight_Pos[FORESIGHT_POS_COUNT];
		float pano_length;
		float pano_height;
		float foresightPosX;
		float foresightPosY;
		float angle[2];
		int mil[2];
		float speedXY[2];
		float touch_delta_angle;
	};

/*******Facade Pattern********/
class	ForeSightFacade
{
public:
	ForeSightFacade(PInterfaceForeSight p_IF,ForeSightPos &pos ,PInterfaceCamonForeSight  p_cam):
		pInterfaceForeSight(p_IF),
		pcamonForeSight(p_cam),
		foreSightPos(pos)
	{
	};
	~ForeSightFacade(){
			BaseForeSight *pForeSight = dynamic_cast<BaseForeSight*>(pInterfaceForeSight);
			if(pForeSight){
				delete pForeSight;}
	};
	void MoveUp(float Ylimit,int mainorsub=MAIN); //在按键事件时调用
	void MoveDown(float Ylimit,int mainorsub=MAIN);
	bool MoveLeft(float Xlimit,int mainorsub=MAIN);
	bool MoveRight(float Xlimit,int mainorsub=MAIN);

	void TrackMoveUp(float Ylimit){foreSightPos.TrackMoveUp(Ylimit);}; //在按键事件时调用
	void TrackMoveDown(float Ylimit){foreSightPos.TrackMoveDown(Ylimit);};
	bool TrackMoveLeft(float Xlimit){foreSightPos.TrackMoveLeft(Xlimit);};
	bool TrackMoveRight(float Xlimit){foreSightPos.TrackMoveRight(Xlimit);};



	void Draw(GLEnv &m_env,float readAngle,int mainorsub=MAIN);//在渲染时间时调用
	void SetAlign(int num,int index){foreSightPos.SetAlignNum(num),foreSightPos.SetAlignindex(index);};
	float GetForeSightPosX( ){return foreSightPos.GetForeSightPosX();};
	float GetForeSightPosY( ){return foreSightPos.GetForeSightPosY();};
	int *GetMil(){
		foreSightPos.GetAngle();
		return foreSightPos.ChangeA2M();
	};
	int *GetTelMil(int idx)
	{
		foreSightPos.GetTelAngle(idx);
		return foreSightPos.ChangeTelA2M();
	}

	void Reset(int mode,int mainorsub=MAIN){
		if(foreSightPos.Reset(mode))
		{
			pcamonForeSight->CamMoveDown(mainorsub);
			pcamonForeSight->CamMoveLeft(mainorsub);
			pcamonForeSight->CamMoveRight(mainorsub);
			pcamonForeSight->CamMoveUp(mainorsub);
		}
	};
private:
	PInterfaceForeSight  pInterfaceForeSight;
	ForeSightPos & foreSightPos;
	PInterfaceCamonForeSight  pcamonForeSight;
	ForeSightFacade();
};

#endif
#endif
