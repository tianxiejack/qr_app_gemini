/*
 * GLRender.h
 *
 *  Created on: Nov 3, 2016
 *      Author: hoover
 */

#ifndef GLRENDER_H_
#define GLRENDER_H_

#include "STLASCIILoader.h"
#include <GL/glew.h> // glUniformxxx()
#include <GL/gl.h>   // OpenGL itself.
#include <GL/glu.h>  // GLU support library.
#include <GL/glut.h> // GLUT support library.

#include<opencv2/opencv.hpp>
#include "cv_version.h"
#include <stdio.h>
#include <time.h>       /* For our FPS */
#include <string.h>    /* for strcpy and relatives */
#include <unistd.h>     /* needed to sleep*/
#include "GLRender.h"
#include "StlGlDefines.h"
#include "ShaderParamArrays.h"
#include <math.h>
#include <GLBatch.h>
#include <GLShaderManager.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>
#include <GLFrustum.h>

#include <iostream>
#include "PBOManager.h"
#include "OitVehicle.h"
#include "STLASCIILoader.h"
#include "ShaderParamArrays.h"
#include "glm.h"
#include "StopWatch.h"
#include "PresetCameraGroup.h"
#include "DynamicTrack.h"
#include "CornerMarker.h"
#include "LineofRuler.h"

#include "DataofAlarmarea.h"
#include"ForeSight.h"

#include"FBOManager.h"
#include"PBOManager.h"
#include "PBO_FBO_Facade.h"
#include "RenderDrawBehaviour.h"
#include"GLEnv.h"
#include "CenterPoint_caculate.h"
#include "EnhBhv.h"
#include "EnhStateMachine.h"
using namespace std;
class InterfaceTouch_Center_Cross;
class RenderMain;
static const int ALPHA_MASK_HEIGHT= 540;//DEFAULT_IMAGE_HEIGHT;
static const int ALPHA_MASK_WIDTH = (DEFAULT_IMAGE_WIDTH/16);
/* A general OpenGL initialization function. */
/* Called once from main() */
/*set up render scene*/
static const M3DVector3f DEFAULT_ORIGIN = {0.0f, 0.0f, 50.0f};
static const int GAIN_TEX_HEIGHT=4;
static const int GAIN_TEX_WIDTH=192;//3
#define HEAD_MID_TAIL 192 //3
#define RGBNUM 3
#define MAX_SLICES (GAIN_TEX_HEIGHT)

class Render:public InterFaceDrawBehaviour,public IFEnhBhv
{
	
public:	
	
		void InitRenderTGA(int x=0,int y=155,int width=300,int height=155);
		void InitTextures();
		void DoTextureBinding();
		void	LoadTGATextureRects();
		
		bool LoadTGATextureRect(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode);
private:
	GLuint      renderTGATextures[1];
	char          renderTGATextureFileName[1][64];
	GLBatch  renderTGABatch;
public:
	Render();
	~Render();
	void ChangeChosenByForesight();
	virtual void FBOdraw(bool &Isenhdata,bool &IsTouchenhdata);
	bool getPointsValue(int direction, int x, Point2f *Point);
	void SetWheelAngle(float a){m_DynamicWheelAngle=a;};
	void destroyPixList();
	void SetupRC(int windowWidth, int windowHeight);
	void SetupRCDS(int windowWidth, int windowHeight);
	void initCorners(void);
	void ReSizeGLScene(int Width, int Height);
	void ReSizeGLSceneDS(int Width, int Height);
	void DrawGLScene();
	void DrawGLSceneDS();
	void initClass(void);
	void initPixle(void);
	void readPixleFile(const char* file, int index);
	void InitScanAngle(void);
	void readScanAngle(const char * filename);
	void writeScanAngle(const char *filename,float angle,float angleofruler);
	void ProcessOitKeys(GLEnv &m_env,unsigned char key, int x, int y);
	void ProcessOitKeysDS(GLEnv &m_env,unsigned char key, int x, int y);
	void mouseButtonPress(int button, int state, int x, int y);
	void mouseButtonPressDS(int button, int state, int x, int y);
	void mouseMotionPress(int x, int y);
	inline GLShaderManager* getShaderManager(){return &shaderManager;}
	void SendData(int i,bool needSendData);
	void UseShaderManager(GLT_STOCK_SHADER nShaderID);
//	inline GLGeometryTransform* getTransformPipeline(){return &transformPipeline;}
	void keyPressed(GLEnv &m_env,unsigned char key, int x, int y);
	void keyPressedDS(GLEnv &m_env,unsigned char key, int x, int y);

	void specialkeyPressed (GLEnv &m_env,int key, int x, int y);
	void BowlParseSTLAscii(const char* filename);
	void PanelParseSTLAscii(const char* filename);
	void VehicleParseObj(const char* filename);
	inline int BowlGetMemSize(){return BowlLoader.GetMemSize();};
	int VehicleGetMemSize();
	inline int BowlGetpoly_count(){return BowlLoader.Getpoly_count();};
	int VehicleGetpoly_count();
	static void SwitchBlendMode(int blendmode);
	void updateTexture(int id, bool needSendData);
	void setOverlapPeta(int chId, float alpha);
	void SetShowDirection(int dir,bool show_mobile);

        void InitALPHA_ZOOM_SCALE(void );
        void readALPHA_ZOOM_SCALE(const char * filename);
        void writeALPHA_ZOOM_SCALE(char * filename,float ALPHA_ZOOM_SCALE);

    void ReadPanoHorVerScaleData(char * filename);
    void WritePanoHorVerScaleData(char * filename ,float * hor_data,float * ver_data);

    void showDeviceState();

    int SetWheelArcWidth(float arcWidth);
    void SetdisplayMode( );
    void ChangeTelMode();
    void sendBack();
    void sendTrackSpeed(int w,int h);

    void Debuging();
    int TransPosX(int srcX);
    int TransPosY(int srcY);
    void	RecvNetPosXY();
    void	RecvNetPosXYDS();
    float getbase_x_scale(){
    	return base_x_scale;
    }
    float getbase_y_scale()
    {
    	return base_y_scale;
    }
    int getscale_count()
    {
    	return scale_count;
    }
    int getthechannel_max_count()
    {
    	return thechannel_max_count;
    }
private:
	float base_x_scale,base_y_scale;
	int scale_count;
	int thechannel_max_count;
	#if TRACK_MODE
	int getTrkId(int displayMode,int nextMode);
	void clearTrackParams();
	#endif
 enum DISPLAYMODE {

		SPLIT_VIEW_MODE = 0, //birdview + rotating view
		CHECK_MYSELF,
		ALL_VIEW_MODE,
		CHOSEN_VIEW_MODE,
		TRIM_MODE,

		TELESCOPE_FRONT_MODE,
		TELESCOPE_RIGHT_MODE,
		TELESCOPE_BACK_MODE,
		TELESCOPE_LEFT_MODE,


		PREVIEW_MODE,
		FREE_VIEW_MODE,

		VGA_WHITE_VIEW_MODE,
		VGA_HOT_BIG_VIEW_MODE,
		VGA_HOT_SMALL_VIEW_MODE,
		VGA_FUSE_WOOD_LAND_VIEW_MODE,
		VGA_FUSE_GRASS_LAND_VIEW_MODE,
		VGA_FUSE_SNOW_FIELD_VIEW_MODE,
		VGA_FUSE_DESERT_VIEW_MODE,
		VGA_FUSE_CITY_VIEW_MODE,
		ALL_VIEW_FRONT_BACK_ONE_DOUBLE_MODE,//全景

			SDI1_WHITE_BIG_VIEW_MODE,
			SDI1_WHITE_SMALL_VIEW_MODE,
			SDI2_HOT_BIG_VIEW_MODE,
			SDI2_HOT_SMALL_VIEW_MODE,

			PAL1_WHITE_BIG_VIEW_MODE,
			PAL1_WHITE_SMALL_VIEW_MODE,
			PAL2_HOT_BIG_VIEW_MODE,
			PAL2_HOT_SMALL_VIEW_MODE,
			EMPTY_MODE,
	//	PREVIEW_MODE,	//allow use to browse the whole scene using a mouse


		SINGLE_PORT_MODE ,	//view an individual camera
		// operate the preset cameras
		TRIPLE_VIEW_MODE,		// birdview + presetCamera + individual camera
		BACK_VIEW_MODE,
		EXTENSION_VIEW_MODE,	// view external cameras
		ALL_ADD_712_MODE,
		CENTER_VIEW_MODE,//view from center
		PANO_VIEW_MODE,//pano view
		TWO_HALF_PANO_VIEW_MODE,//two pano view :first -22.5~202.5 degree;second: 157.5~382.5 degree
		//two pano and monitor(big)

		INIT_VIEW_MODE,
		FRONT_BACK_PANO_ADD_SMALLMONITOR_VIEW_MODE,//two pano and monitor(small)
		FRONT_BACK_PANO_ADD_MONITOR_VIEW_MODE,


		TOTAL_MODE_COUNT
		} displayMode; 

		enum SECOND_DISPLAY{
			SECOND_ALL_VIEW_MODE,
			SECOND_CHOSEN_VIEW_MODE,
			SECOND_559_ALL_VIEW_MODE,
			SECOND_TELESCOPE_FRONT_MODE,
			SECOND_TELESCOPE_RIGHT_MODE,
			SECOND_TELESCOPE_BACK_MODE,
			SECOND_TELESCOPE_LEFT_MODE,
			SECOND_TOTAL_MODE_COUNT
		}SecondDisplayMode;

		 enum FBO_MODE  {
			 FBO_ALL_VIEW_MODE,
			 FBO_ALL_VIEW_SCAN_MODE,
			 FBO_ALL_VIEW_559_MODE,
			 FBO_MODE_COUNT
		 }fboMode;

		 enum HideLabelState { 
			 SHOW_ALL_LABEL,
			 HIDE_TEST_LABEL,
			 HIDE_TEST_COMPASS_LABEL,
			 HIDE_LABEL_STATE_COUNT
		 };

	class TimeBar{
		public:
			TimeBar():delta_year(0),delta_month(0),delta_day(0),delta_hour(0),delta_minute(0),delta_second(0),indicator(0){buf[0] = 0;}
			void CalibTime(int key);
			const char* GetFTime();
			void SetFTime(const char* time_str);
			inline void ResetDeltas(){delta_year = delta_month=delta_day=delta_hour=delta_minute=delta_second=0;};
			inline unsigned int GetIndicator(){return indicator;};
		private:
			int  SetSysDateAndTime(const char *time_str);    
			void SetHWClockFromSysClock(int utc);    
			char buf[64];
			unsigned int indicator;
			int delta_year, delta_month, delta_day, delta_hour, delta_minute, delta_second;
		}m_Timebar;

	class BaseBillBoard{
	
		public:
			BaseBillBoard(GLMatrixStack &modelViewMat,GLMatrixStack	&projectionMat,GLShaderManager* mgr=NULL,int bmodeIdx=6);
			virtual ~BaseBillBoard()=0;
			void Init(int x=500,int y=155,int width=300,int height=155);
			virtual void processKeyDirection(int key)= 0;
			inline void setBlendMode(int mode){blendmode = mode;};
		public:
			void DrawBillBoard(int w, int h,int bmode=6);
		protected:
			bool LoadTGATextureRect(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode);
			virtual void InitTextures() = 0;
			virtual void DoTextureBinding() = 0;
		private:
			BaseBillBoard():modelViewMatrix(modelViewMatrix),projectionMatrix(projectionMatrix),m_pShaderManager(NULL){};
			GLShaderManager * 	m_pShaderManager;
			GLMatrixStack &		modelViewMatrix;
			GLMatrixStack &		projectionMatrix;
			int 			  blendmode;
			GLBatch		  HZbatch;

		} ;

	class BillBoard : public BaseBillBoard {
		friend class Render;
		private:
			enum BBD_DIRECTION {
				BBD_FRONT,
				BBD_FRONT_RIGHT,
				BBD_REAR_RIGHT,
				BBD_REAR,
				BBD_REAR_LEFT,
				BBD_FRONT_LEFT,
				BBD_COUNT
				};
			unsigned int m_Direction;
			unsigned int m_lastDirection;
		public:
			BillBoard(GLMatrixStack &modelViewMat,GLMatrixStack	&projectionMat,GLShaderManager* mgr=NULL, BBD_DIRECTION dir = BBD_FRONT);
			~BillBoard();
			virtual void processKeyDirection(int key);
		protected:
			virtual void InitTextures();
			virtual void DoTextureBinding();
		private:
			void 		LoadTGATextureRects();
			GLuint      m_BBDTextures[BBD_COUNT];
			char          m_BBDTextureFileName[BBD_COUNT][64];
		}*p_BillBoard;

		class CompassBillBoard : public BaseBillBoard {
			friend class Render;
			private:
				enum COMPASS_BBD_DIRECTION {
					COMPASS__PIC,
					COMPASS_COUNT
					};
				unsigned int m_CompassDirection;
				unsigned int m_CompasslastDirection;
			public:
					CompassBillBoard(GLMatrixStack &modelViewMat,GLMatrixStack	&projectionMat,GLShaderManager* mgr=NULL, COMPASS_BBD_DIRECTION dir = COMPASS__PIC);
					~CompassBillBoard();
					virtual void processKeyDirection(int key);
			protected:
				virtual void InitTextures();
				virtual void DoTextureBinding();
			private:
				void CompassLoadTGATextureRects();
				GLuint              m_CompassBBDTextures[COMPASS_COUNT];
				char                m_CompassBBDTextureFileName[COMPASS_COUNT][64];
			}*p_CompassBillBoard;
	
	class ExtBillBoard : public BaseBillBoard{
		friend class Render;
		public:

			ExtBillBoard(GLMatrixStack &modelViewMat,GLMatrixStack	&projectionMat,GLShaderManager* mgr=NULL);
			~ExtBillBoard();
			virtual void processKeyDirection(int key){};
		protected:
			virtual void InitTextures();	
			virtual void DoTextureBinding();
		private:
			static const int EXT_COUNT = 1;
			void LoadTGATextureRectExt();
			GLuint              m_BBDTextures[EXT_COUNT];
			char                m_BBDTextureFileName[EXT_COUNT][64];			
		}*p_BillBoardExt;

		class ChineseCharacterBillBoard:public BaseBillBoard{
			friend class Render;
			public:
	ChineseCharacterBillBoard(GLMatrixStack &modelViewMat,GLMatrixStack	&projectionMat,GLShaderManager* mgr=NULL);
	~ChineseCharacterBillBoard(){glDeleteTextures(CCT_COUNT, ChineseC_Textures);};
	virtual void processKeyDirection(int key){};
			protected:
					virtual void InitTextures();
					virtual void DoTextureBinding();

private:
	void LoadChineseCTGATexture();
	unsigned int  	ChooseTga; //20
	GLuint              ChineseC_Textures[CCT_COUNT];
	char                ChineseC_TextureFileName[CCT_COUNT][64];
		}*p_ChineseCBillBoard,*p_ChineseCBillBoard_bottem_pos;


	class FreeCamera{
		public:
			FreeCamera(GLFrame &f ):m_CameraFrame(f){};
			FreeCamera(){m_CameraFrame.SetOrigin(DEFAULT_ORIGIN);};
			void SetFrame(const GLFrame &f){m_CameraFrame = f;};
			inline void GetCameraMatrix( M3DMatrix44f m, bool bRotationOnly = false){m_CameraFrame.GetCameraMatrix(m, bRotationOnly);};
			const GLFrame& GetFrame(){return m_CameraFrame;}
			inline void Translate(float x, float y, float z){m_CameraFrame.TranslateLocal(x,y,z);};
			inline void Rotate(float angle, float x, float y, float z){m_CameraFrame.RotateLocal(angle, x, y, z);};
			inline void MoveForward(float fDelta){m_CameraFrame.MoveForward(fDelta);};
			void Reset(){m_CameraFrame.SetOrigin(DEFAULT_ORIGIN);
							m_CameraFrame.SetForwardVector(0.0f,0.0f,-1.0f);
							m_CameraFrame.SetUpVector(0.0f, 1.0f, 0.0f);};
			const char* GetCords();
			const char* GetOriginCords();
			const char* GetUpCords();
			const char* GetFwdCords();
		private:
			GLFrame m_CameraFrame;
			std::string Cords;
		} m_freeCamera;

	class FixedBillBoard{
		public:
			FixedBillBoard(const char* fileName,GLMatrixStack &modelViewMat,GLMatrixStack	&projectionMat,GLShaderManager* mgr=NULL);
			~FixedBillBoard();
			typedef enum{
				LOCATION_BBD_BACK,
				LOCATION_BBD_HEAD,
				LOCATION_BBD_FISHEYE
			} LOCATION_BBD;
			void SetBackLocation(float location[3]);
			void SetFishEyeLocation(float location[3]);
			void SetHeadLocation(float location[3]);
			void SetSize(float size[2]);
			static void setBlendMode(int mode){blendmode = mode;};
			static void DrawGroup(GLEnv &m_env,M3DMatrix44f camera, FixedBillBoard *bbd[], unsigned int count);
			void DrawSingle(GLEnv &m_env,M3DMatrix44f camera, FixedBillBoard *bbd);
		private:
			void Draw(GLEnv &m_env,M3DMatrix44f camera, LOCATION_BBD loc=LOCATION_BBD_BACK);
			GLuint LoadDDS(const char* fileName);
			GLShaderManager * 	m_pShaderManager;
			GLMatrixStack &		modelViewMatrix;
			GLMatrixStack &		projectionMatrix;
			static int blendmode;			
			static GLuint billboard_vertex_buffer;
			static GLuint programID;;
			static GLuint TextureID;
			GLuint Texture;
	// Vertex shader
			GLuint CameraRight_worldspace_ID;
			GLuint CameraUp_worldspace_ID;
			GLuint ViewProjMatrixID;
			GLuint BillboardPosID;
			GLuint BillboardSizeID;
	// Get a handle for our buffers
			GLuint squareVerticesID;

			GLfloat position[3];
			GLfloat head_position[3];
			GLfloat bbdsize[2];
			GLfloat fisheye_position[3];

	}*p_FixedBBD_1M, *p_FixedBBD_2M, 	*p_FixedBBD_5M, *p_FixedBBD_8M;
	void DrawTrackFixBBDs(GLEnv &m_env,M3DMatrix44f camera);
	void DrawCords(GLEnv &m_env,int w, int h, const char* s);
	void DrawAngleCords(GLEnv &m_env,int w, int h, const char* s,float toScale);
	void RememberTime();
	void DrawFrontBackTracks(GLEnv &m_env);
	void Draw4CrossLines(GLEnv &m_env);
	void DrawTrackHead(GLEnv &m_env);
	void DrawTrackRear(GLEnv &m_env);
	void DrawShadow(GLEnv &m_env);
	void DrawIndividualVideo(GLEnv &m_env,bool needSendData);
	void DrawExtensionVideo(GLEnv &m_env,bool needSendData);
	void DrawCompassVideo(GLEnv &m_env,bool needSendData);
	void DrawRulerVideo(GLEnv &m_env,bool needSendData,int type);
	int  GetCurrentExtesionVideoId(){return m_ExtVideoId;};

	void InitShadow(GLEnv &m_env);
	void InitRuler(GLEnv &m_env);
	void InitWheelTracks();
	void SetCurrentExtesionVideoId(int curChid){m_ExtVideoId=curChid;};
	void DrawVGAVideo(GLEnv &m_env,bool needSendData);
	void DrawSDIVideo(GLEnv &m_env,bool needSendData);
	void DrawChosenVideo(GLEnv &m_env,bool needSendData,int mainorsub=MAIN);
	void DrawCenterVideo(bool &Isenhdata,GLEnv &m_env,bool needSendData,int mainorsub=MAIN);
	void DrawTargetVideo(GLEnv &m_env, int targetIdx,int camIdx,bool needSendData);
	int GetCurrentVGAVideoId(){return m_VGAVideoId;};
	int GetCurrentSDIVideoId(){return m_SDIVideoId;};
	int GetCurrentChosenVideoId(){return m_ChosenVideoId;};

	void InitForesightGroupTrack(GLEnv &m_env);

#if ADD_FUCNTION_BY_JIMMY
		void InitCircleFanBatch(GLEnv& m_env);
		void InitCircleBatch( GLEnv &m_env );	
		void DrawCircleLines(GLEnv &m_env,const float *color_data );	
		void RenderCircleLineViewForSurroundSight(GLEnv &m_env,GLint x, GLint y, GLint w, GLint h,const float *color_data);
		void RenderForTestTriangleView(GLEnv &m_env,GLint x, GLint y, GLint w, GLint h,  unsigned char state);
		void DrawCircleFans(GLEnv &m_env,const float *color_data);	
		void  RenderDirectTriangleViewForSurroundSight(GLEnv &m_env,GLfloat direct_angle ,GLint x, GLint y, GLint w, GLint h,  const float *color_data);
		void  DrawDirectionTriangleView( GLEnv &m_env,const float *color_data);
		void  InitDirectionTriangleBatch( GLEnv &m_env );
		void RenderVerticalValueViewForSurroundSight(GLEnv &m_env,GLint x, GLint y, GLint w, GLint h,  const float *color_data);
		void DrawSurroundSightVerticalView( GLEnv &m_env,const float *color_data );
		void InitSurroundingSighttVerticalBatch( GLEnv &m_env);
		
		void InitVerticalMoveLineBatchForSurroundSight(GLEnv &m_env);
		void RenderVerticalMoveLineViewForSurroundSight(GLEnv &m_env,GLint x, GLint y, GLint w, GLint h,  const float *color_data);
		void DrawTurretVerticalMoveLineView( GLEnv &m_env,const float *color_data );
		void RenderCircleLineViewForTurret(GLEnv &m_env,GLint x, GLint y, GLint w, GLint h,  const float *color_data);
	
		void RenderVerticalValueViewForTurret(GLEnv &m_env,GLint x, GLint y, GLint w, GLint h,	const float *color_data);
		void RenderDirectTriangleViewForTurret(GLEnv &m_env,GLfloat direct_angle ,GLint x, GLint y, GLint w, GLint h,  const float *color_data);
		void DrawSurroundSightVerticalMoveLineView( GLEnv &m_env,const float *color_data );
		void RenderVerticalMoveLineViewForTurret(GLEnv &m_env,GLint x, GLint y, GLint w, GLint h,  const float *color_data);
		void InitVerticalMoveLineBatchForTurret(GLEnv &m_env);
	
#endif

	void InitFrontTracks();
	void InitCrossLines();
	void InitWealTrack();
	void InitDynamicTrack(GLEnv &m_env);
	void InitCornerMarkerGroup(GLEnv &m_env);
	void InitBillBoard(GLEnv &m_env);
	void DrawCordsView(GLEnv &m_env,Rect* rec, char* text);
	void DrawAngleCordsView(GLEnv &m_env,Rect* rec, char* text,float toScale);
	void DrawInitView(GLEnv &m_env,Rect* rec, bool needSendData);
	void DrawSigleScale(GLEnv &m_env,Rect* rec, GLint idx, bool needSendData);
	void DrawSigleVideo(GLEnv &m_env,GLint idx, bool needSendData);

	void DrawVehiclesEtc(GLEnv &m_env,M3DMatrix44f camera=NULL);
	void DrawVehiclesEtcWithFixedBBD(GLEnv &m_env,M3DMatrix44f camera);
	void DrawStrings(GLEnv &m_env,int w, int h, const char * s=NULL);
	void DrawStringsWithHighLight(GLEnv &m_env,int w, int h, const char* s=NULL, int idx_HLt = -1);
	void RenderTimeView(GLEnv &m_env,GLint x, GLint y,GLint w, GLint h);
	void RenderBirdView(bool &Isenhdata,GLEnv &m_env,GLint x, GLint y, GLint w, GLint h, bool needSendData);
	void RenderAnimationToBirdView(bool &Isenhdata,GLEnv &m_env,GLint x, GLint y, GLint w, GLint h, bool needSendData);
	void RenderPreSetView(bool &Isenhdata,GLEnv &m_env,GLint x, GLint y, GLint w, GLint h,bool needSendData, bool isRearTop = false);
	void RenderRearTopView(bool &Isenhdata,GLEnv &m_env,GLint x, GLint y, GLint w, GLint h,bool needSendData);
	void RenderPresetViewByRotating(bool &Isenhdata,GLEnv &m_env,GLint x, GLint y, GLint w, GLint h,bool needSendData);
	void RenderRotatingView(bool &Isenhdata,GLEnv &m_env,GLint x, GLint y, GLint w, GLint h,bool needSendData);
	void RenderSingleView(bool &Isenhdata,GLEnv &m_env,GLint x, GLint y, GLint w, GLint h,int mainOrsub=MAIN);
	void RenderCenterView(bool &Isenhdata,GLEnv &m_env,GLint x, GLint y, GLint w, GLint h);
	void RenderRegionPanelView(bool &Isenhdata,GLEnv &m_env,GLint x, GLint y, GLint w, GLint h,int mainOrsub=MAIN);
	void RenderCompassView(GLEnv &m_env,GLint x, GLint y, GLint w, GLint h);
	void RenderRulerView(GLEnv &m_env,GLint x, GLint y, GLint w, GLint h,int type);
	void RenderPanoView(bool &Isenhdata,GLEnv &m_env,GLint x, GLint y, GLint w, GLint h,int mainOrsub=MAIN);


	void RenderRightForeSightView(GLEnv &m_env,GLint x, GLint y, GLint w, GLint h,int mainOrsub=MAIN);
	void RenderLeftForeSightView(GLEnv &m_env,GLint x, GLint y, GLint w, GLint h,int mainOrsub=MAIN);

	void RenderLeftPanoView(bool &Isenhdata,GLEnv &m_env,GLint x, GLint y, GLint w, GLint h,int mianORsub=MAIN,bool needSendData=true);
	void RenderRightPanoView(bool &Isenhdata,GLEnv &m_env,GLint x, GLint y, GLint w, GLint h,int mainOrsub=MAIN,GLint scissor_x=0, GLint scissor_y=0, GLint scissor_w=0, GLint scissor_h=0,bool needSendData=true);
	void RenderMyLeftPanoView(bool &Isenhdata,GLEnv &m_env,GLint x, GLint y, GLint w, GLint h,int mainOrsub=MAIN,bool needSendData=true);

	void RenderIndividualView(GLEnv &m_env,GLint x, GLint y, GLint w, GLint h,bool needSendData);
	void RenderBillBoardAt(GLEnv &m_env,GLint x, GLint y,GLint w, GLint h);
	
	void RenderCompassBillBoardAt(GLEnv &m_env,GLint x, GLint y,GLint w, GLint h);

	void InitDataofAlarmarea();
	void PrepareAlarmAera(GLEnv &m_env,int x,int y,int w,int h);
	void InitAlarmArea(int positionofalarm[8],int type);
	 void InitAlarmAreaType(int type);
	 void CancelAlarmArea();
	void DrawAlarmArea(GLEnv &m_env,float get_pos[8],int x,int y,int w,int h,int color_type);
	void DrawAlarmAreaonScreen(GLEnv &m_env);
	void DrawAlarmLine(GLEnv &m_env,float pos[4]);

	void SendtoTrack();
	void RenderTriangleView(GLEnv &m_env,GLint x, GLint y, GLint w, GLint h);
	void RenderChineseCharacterBillBoardAt(GLEnv &m_env,GLint x, GLint y,GLint w, GLint h,int bmode=6,	bool isbottem=false);
	void RenderPanoTelView(bool &Isenhdata,GLEnv &m_env,GLint x, GLint y, GLint w, GLint h,int direction,int mainOrsub=MAIN);
	void RenderTrackForeSightView(GLEnv &m_env,GLint x, GLint y, GLint w, GLint h);

	void Show_first_mode(int read_mode);
	int readFirstMode();
	void writeFirstMode(int Modenum);

	void RenderFreeView(bool &Isenhdata,GLEnv &m_env,GLint x, GLint y, GLint w, GLint h, bool needSendData);
	void RenderOriginCords(GLEnv &m_env,GLint x, GLint y,GLint w, GLint h);
	void RenderUpCords(GLEnv &m_env,GLint x, GLint y,GLint w, GLint h);
	void RenderFwdCords(GLEnv &m_env,GLint x, GLint y,GLint w, GLint h);
	void RenderCordsView(GLEnv &m_env,GLint x, GLint y,GLint w, GLint h);
	void RenderExtensionBillBoardAt(GLEnv &m_env,GLint x, GLint y,GLint w, GLint h);

	void RenderVGAView(GLEnv &m_env,GLint x, GLint y, GLint w, GLint h, bool needSendData);
	void RenderSDIView(GLEnv &m_env,GLint x, GLint y, GLint w, GLint h, bool needSendData);
	void RenderRegionalView(bool &IsTouchenhdata,GLEnv &m_env,GLint x, GLint y, GLint w, GLint h, int mainorsub=MAIN,bool needSendData=true,bool drawDirection=true);
	void RenderChosenView(GLEnv &m_env,GLint x, GLint y, GLint w, GLint h, int mainorsub=MAIN,bool needSendData=true,bool drawDirection=true);
	void RenderOnetimeView(bool &Isenhdata,GLEnv &m_env,GLint x, GLint y, GLint w, GLint h,int mainOrsub=MAIN);
	void RenderTwotimesView(bool &Isenhdata,GLEnv &m_env,GLint x, GLint y, GLint w, GLint h,int mainOrsub=MAIN);
	void RenderOnetimeView2(bool &Isenhdata,GLEnv &m_env,GLint x, GLint y, GLint w, GLint h,int mainOrsub=MAIN);
	void RenderTwotimesView2(bool &Isenhdata,GLEnv &m_env,GLint x, GLint y, GLint w, GLint h,int mainOrsub=MAIN);
	void RenderFourtimesTelView(bool &Isenhdata,GLEnv &m_env,GLint x, GLint y, GLint w, GLint h,int mainOrsub=MAIN);
	void RenderPositionView(GLEnv &m_env,GLint x, GLint y, GLint w, GLint h);
	void RenderCheckMyselfView(GLEnv &m_env,GLint x, GLint y, GLint w, GLint h);
	void TargectTelView(GLEnv &m_env,GLint x, GLint y, GLint w, GLint h,int camidx,int targetIdx,int enlarge=0,int mainOrsub=MAIN);
	void RenderMilView(GLEnv &m_env,GLint x, GLint y, GLint w, GLint h);



	void GenerateCheckView();
	void GenerateOnetimeView(int mainOrsub=MAIN);
	void GenerateOnetimeView2(int mainOrsub=MAIN);
	void GenerateTwotimesView(int mainOrsub=MAIN);
	void GenerateTwotimesView2(int mainOrsub=MAIN);
	void GenerateTwotimesTelView(int mainOrsub=MAIN);
	void GenerateFourtimesTelView(int mainOrsub=MAIN);

	void GenerateBirdView();
	void GenerateExtentView();
	void GenerateRegionalView();
	void GenerateCenterView();
	void GenerateScanPanelView();
	void GeneratePanoView();
	void GenerateChosenView();

	void GenerateSDIView();
	void GenerateVGAView();
	void GenerateRender2FrontView();
	void GenerateTargetFrameView();
	void GeneratePanoTelView(int mainOrsub=MAIN);
	void GenerateTrack();
	void GenerateTouchCrossView();
	void GenerateLeftPanoView();
	void GenerateRightPanoView();
	void GenerateLeftSmallPanoView();
	void GenerateRightSmallPanoView();
	void GenerateCompassView();
	void GenerateFrontView();
	void GenerateRearTopView();
	void GetFPS();
	void GetFPSDS();
	void SetView(int Width, int Height);
	void ChangeSize(int w, int h);
	void ChangeSizeDS(int w, int h);
	void calcCommonZone();
	void InitBowl();
	void InitBowlDS();
	void InitFollowCross();
	void InitCalibrate();
	void InitPanel(GLEnv &m_env,int idx=0,bool reset=false);

	void GenerateTriangleView();
	void DrawBowl(bool &Isenhdata,GLEnv &m_env,bool needSendData,int mainOrsub=MAIN);
	void DrawPanel(bool &Isenhdata,GLEnv &m_env,bool needSendData,int *p_petalNum,int mainOrsub=MAIN);

	void initAlphaMask();
	void DrawVehicle();
	void DrawSlideonPanel(GLEnv &m_env);
	void DrawCrossonPanel(GLEnv &m_env);
	void DrawRuleronPanel(GLEnv &m_env);
	void DrawNeedleonCompass(GLEnv &m_env);
	void DrawNeedleGunonCompass(GLEnv &m_env);
	void DrawNeedleGunonDegree(GLEnv &m_env);
	void DrawVerGunAngle(GLEnv &m_env);
	void DrawGapLine(GLEnv &m_env);
	void DrawNeedleCanononCompass(GLEnv &m_env);
	void DrawNeedleCanononDegree(GLEnv &m_env);
	void DrawVerCanonAngle(GLEnv &m_env);
	void DrawStateLabel(GLEnv &m_env);

	void DrawTriangle(GLEnv &m_env);
	 void set_SightWide(int recvWide);

	void RenderScene(void);
	void RenderSceneDS(void);
	void DrawOitVehicle(GLEnv &m_env);
	void InitOitVehicle(GLEnv &m_env);
	void InitAlarmAeraonPano(GLEnv &m_env);
	void GenerateGLTextureIds();
	void generateAlphaList(Point2f AlphaList[], float alpha_index_x, float alpha_index_y, int i);
	bool IsOverlay(bool AppDirection[6], int *direction);
	void fillDataList(vector<cv::Point3f> *list,int x);
	void panel_fillDataList(vector<cv::Point3f> *list,int x,int idx);
	void markDirection(bool AppDirection[],int num);
	void checkDirection(bool AppDirection[],int x);
	int getOverlapIndex(int direction,int idx);

	bool getOverLapPointsValue(int direction, int x, Point2f *Point1, Point2f *Point2);
	void getOffsetValue(int direction, int x, int* offset_L, int* offset_R);
	inline bool isDirectionMode();
	void UpdateWheelAngle();
	 void prepareTexture(int texture_id);
	 void drawDynamicTracks(GLEnv &m_env);
	 void InitLineofRuler(GLEnv &m_env);
	 void InitPanoScaleArrayData();
	 void ReadPanoScaleArrayData(char * filename);
	 void WritePanoScaleArrayData(char * filename,float * arraydata_left,float * arraydata_right,float * arraydata_level);

	void initLabelBatch();

	void ChangeSecondMode();
	void ChangeSecondMv();
	void ChangeSecondEnh();
	void MoveSecondForesight();
	void ChangeSecondSc();
	void NoSigInf();	
	
	void RenderGreenScreen(GLEnv &m_env, GLuint x, GLuint y,GLuint w,GLuint h);
	
	void DrawSelectBlock();
	void InitSelectBlock();
	void RenderSelectBlock(GLEnv &m_env, GLint x, GLint y, GLint w, GLint h);
public:

	 GLShaderManager		shaderManager2;			// Shader Manager
	 GLMatrixStack		modelViewMatrix2;		// Modelview Matrix
	 GLMatrixStack		projectionMatrix2;		// Projection Matrix
	 GLFrustum			viewFrustum2;			// View Frustum
	 GLGeometryTransform	transformPipeline2;		// Geometry Transform Pipeline
	 GLTriangleBatch		torusBatch2;
	 GLBatch				floorBatch2;
	 GLTriangleBatch     sphereBatch2;
	 GLFrame             cameraFrame2;
	 	 bool GainisNew;
		bool isUseNewGain;
		GLfloat tempMask[GAIN_TEX_HEIGHT*GAIN_TEX_WIDTH*CAM_COUNT*RGBNUM];
		GLfloat InterPolatedMask[GAIN_TEX_HEIGHT*GAIN_TEX_WIDTH*CAM_COUNT*RGBNUM];
		GLint interflowMask[GAIN_TEX_HEIGHT*GAIN_TEX_WIDTH*CAM_COUNT];
		GLint transTempMask[GAIN_TEX_HEIGHT*GAIN_TEX_WIDTH*CAM_COUNT];
		GLint GainMask[GAIN_TEX_HEIGHT*GAIN_TEX_WIDTH*CAM_COUNT];

		void SetCharacteristicGainMask(int index,int sliceIdx,
				float leftr,float leftg,float leftb,
				float rightr,float rightg,float rightb);
		void SetDeltaGainMask();
		void AddrowDeltaGain(GLfloat g0,GLfloat g1,int idx,int slices,int row,int cols,int deltaIdx);

		void Interflow(int idx,int rows,int cols,GLfloat*src,GLint*dst );
		void     BlanceNeighbours2InterPolate();
		bool isUsingNewGain()
		{
			return isUseNewGain;
		};
		void En_DisableUseNewGain(bool tof)
		{
			isUseNewGain=tof;
		};
		void SetGainisNew(bool tof){
			GainisNew=tof;
		};
		bool isStitchingMode(){return displayMode == INIT_VIEW_MODE; };
		 float * getTankDistance(){return distance_of_tank;};
		 void setTankDistance(float distance[4]){distance_of_tank[0]=distance[0];
		 distance_of_tank[1]=distance[1];
		 distance_of_tank[2]=distance[2];
		 distance_of_tank[3]=distance[3];};
		 float*  getCrossCenterPos(){return cross_center_pos;};
		 void setCrossCenterPos(float set_val[2]){cross_center_pos[0]=set_val[0];
		 cross_center_pos[1]=set_val[1];};
		 bool getFollowValue(){return follow_enable;};
		 void setFollowVaule(bool value){follow_enable=value;};
		 float getScanRegionAngle(){return set_scan_region_angle;};
		 void setScanRegionAngle(float angle){set_scan_region_angle=angle;};
		 bool getSendFollowAngleEnable(){return send_follow_angle_enable;};
		 void setSendFollowAngleEnable(bool enable){send_follow_angle_enable=enable;};
		 float * getFollowAngle(){return follow_angle;};
		 void setFollowAngle(float angle[2]){follow_angle[0]=angle[0];
		 follow_angle[1]=angle[1];};
		 float getlastregionforwarddistance(){return last_region_forward_distance;};
                 float getALPHA_ZOOM_SCALE(){return ALPHA_ZOOM_SCALE;};
                 void setALPHA_ZOOM_SCALE(float dis){ALPHA_ZOOM_SCALE=dis;};
		 void setlastregionforwarddistance(float dis){last_region_forward_distance=dis;};
		 float getrulerangle(){return ruler_angle;};
		 void setrulerangle(float set_angle){ruler_angle=set_angle;};
		 bool getenableshowruler(){return enable_show_ruler;};
		 void setenableshowruler(bool enable){enable_show_ruler=enable;};
		 float getpanodeltaangle(){return pano_delta_angle;};
		 void setpanodeltaangle(float set_angle){pano_delta_angle=set_angle;};
		 bool getenablerefrushruler(){return refresh_ruler;};
		 void setenablerefrushruler(bool enable){refresh_ruler=enable;};
		 void repositioncamera();
		 void setrulerreferenceangle(float angle){ruler_reference_angle=angle;};
		 float getrulerreferenceangle(){return ruler_reference_angle;};
		 void settrackcontrolparams(int * data){track_control_params[0]=data[0];
		 track_control_params[1]=data[1];
		 track_control_params[2]=data[2];
		 track_control_params[3]=data[3];
		 };
		 int * gettrackcontrolparams(){return track_control_params;};

		 STLASCIILoader get_PanelLoader(){return PanelLoader;};
			 void SetPanoFloatData(float data[CAM_COUNT]){
						 for(int i=0;i<CAM_COUNT;i++)
						 {
							 PanoFloatData[i]=data[i];
						 }
					 };
					 float * GetPanoFloatData(){return PanoFloatData;};
					 void ReadPanoFloatDataFromFile(char * filename);
					 void WritePanoFloatDataFromFile(char * filename,float * panofloatdata);
		 			void ReadRotateAngleDataFromFile(char * filename);
					void WriteRotateAngleDataToFile(char * filename,float * rotateangledata);

					 Calibrate * getRulerAngle(){return p_LineofRuler;};
						ForeSightFacade * GetpWholeFacade(int mainorsub){return p_ForeSightFacade[mainorsub];};
						ForeSightFacade * GetpWholeScanFacade(int mainorsub){return p_ForeSightFacadeScan[mainorsub];};
						ForeSightFacade * GetpTelFacade(int mainorsub){return p_ForeSightFacade2[mainorsub];};
						ForeSightFacade * GetpTrackFacade(){return p_ForeSightFacade_Track;};
						PBOReceiver *GetPBORcr(GLEnv &env){return env.Getp_PBORcr();};
						GLFrame	*getVGACameraFrame(){return &VGACameraFrame;};
						GLFrame	*getRender2FrontCameraFrame(){return &Render2FrontCameraFrame;};
						GLFrame	*getRenderTargetCameraFrame(int i){return &targetFrame[i];};
						void SetFboMode(int mode);
	bool GetPSYButtonF1(){return psy_button_f1;};
	void SetPSYButtonF1(bool enable){psy_button_f1=enable;};
	bool GetPSYButtonF2(){return psy_button_f2;};
	void SetPSYButtonF2(bool enable){psy_button_f2=enable;};
	bool GetPSYButtonF3(){return psy_button_f3;};
	void SetPSYButtonF3(bool enable){psy_button_f3=enable;};
	bool GetPSYButtonF8(){return psy_button_f8;};
	void SetPSYButtonF8(bool enable){psy_button_f8=enable;};

	float GetCanonHorAngle(){return canon_hor_angle;};
	void SetCanonHorAngle(float data){canon_hor_angle=data;};
	float GetCanonVerAngle(){return canon_ver_angle;};
	void SetCanonVerangle(float data){canon_ver_angle=data;};

	float GetGunHorAngle(){return gun_hor_angle;};
	void SetGunHorAngle(float data){gun_hor_angle=data;};
	float GetGunVerAngle(){return gun_ver_angle;};
	void SetGunVerAngle(float data){gun_ver_angle=data;};

	float GetCalcHorData(){return calc_hor_data;};
	void SetCalcHorData(float data){calc_hor_data=data;};
	float GetCalcVerData(){return calc_ver_data;};
	void SetCalcVerData(float data){calc_ver_data=data;};

	int GetTouchPosX(){return touch_pos_x;};
	void SetTouchPosX(int data){touch_pos_x=data;};
	int GetTouchPosY(){return touch_pos_y;};
	void SetTouchPosY(int data){touch_pos_y=data;};
	float GetPanoLen(){
		return PanoLen;
	};
	float GetPanoHeight()
	{
		return PanoHeight;
	};
	void ChangeMainChosenCamidx(char idx);
	char GetChosenCamidx(){
		return chosenCamidx;
	};
	void SetChosenCamidx(char idx){chosenCamidx=idx;};
	void ChangeSubChosenCamidx(char idx);
	Interface_CenterPoint_setter *Get_p_ics(){return p_ics;};
	Interface_CenterPoint_getter *Get_p_cpg(){return p_cpg;};
	void SetTuneSteps(int level);

	float GetPanoFloatData(int idx){
		return PanoFloatData[idx];
	};

	float Getchannel_left_scale(int idx){
		return channel_left_scale[idx];
	};
	float Getchannel_right_scale(int idx){
		return channel_right_scale[idx];
	};
	float Getmove_hor(int idx){
		return move_hor[idx];
	};
	float Getmove_hor_scale(int idx){
		return move_hor_scale[idx];
	};

	float Getmove_ver_scale(int idx){
		return move_ver_scale[idx];
	};
	float Getrotate_angle(int idx){
		return rotate_angle[idx];
	};
	bool IsProducerRGB();
private:
	char chosenCamidx;
	float pano_hor_step;  //a d
	float pano_float_step;  //up down
	float pano_hor_scale;//sw:"
	float pano_rotate_angle;//qe

	GLBatch Petal[CAM_COUNT];
	GLBatch *Petal_OverLap[CAM_COUNT]; // overlap area bwtween petal[i] and [(i+1)%CAM_COUNT]
	GLBatch *OverLap[CAM_COUNT];
	GLBatch WheelTrackBatch; // tracks behind the vehicle
	GLBatch WheelTrackBatch2;
	GLBatch WheelTrackBatch5;
	GLBatch WheelTrackBatch1;

	GLBatch FrontTrackBatch;  // tracks before the vehicle
	GLBatch FrontTrackBatch1;
	GLBatch FrontTrackBatch2;
	GLBatch FrontTrackBatch3;
	GLBatch FrontTrackBatch5;
	GLBatch CrossLinesBatch;  // the 4 cross lines on petal overlap area
	GLBatch WheelTrackBatchHead;
	GLBatch WheelTrackBatchRear;
	GLBatch WheelTrackBatchRear1;
	GLBatch WheelTrackBatchRear2;
	GLBatch WheelTrackBatchRear5;
	
	GLBatch SlideFrameBatch;
	GLBatch CrossFrameBatch;
	GLBatch RulerFrameBatch;
	GLBatch NeedleFrameBatch;
	GLBatch GapLineBatch;
	GLBatch NeedleGunBatch;
	GLBatch DegreeGunBatch;
	GLBatch VerGunAngleBatch;
	GLBatch VerGunRulerBatch;

	GLBatch NeedleCanonBatch;
	GLBatch DegreeCanonBatch;
	GLBatch VerCanonAngleBatch;
	GLBatch VerCanonRulerBatch;

	float RulerAngle;
	GLBatch triangleBatch;
	GLBatch AlarmAreaBatch;
	GLBatch AlarmLineBatch;
	GLBatch select_BlockBatch;

	GLBatch array_round_point[36];

	ForeSightFacade * p_ForeSightFacade[2];
	ForeSightFacade * p_ForeSightFacadeScan[2];
	ForeSightFacade * p_ForeSightFacade2[2];

	ForeSightFacade * p_ForeSightFacade_Track;
	ForeSightFacade * p_ForeSightFacade_Dector;


	BaseAlarmObject * p_dataofalarmarea;
	BaseAlarmObject * p_dataofalarmline;

	float SightWide;

	GLShaderManager shaderManager;

//	GLMatrixStack	modelViewMatrix;
//	GLMatrixStack	projectionMatrix;
//	GLGeometryTransform transformPipeline;
//	GLFrustum viewFrustum;

	OitVehicle *pVehicle;
	GLFrame	birdViewCameraFrame;
	GLFrame frontCameraFrame;
	GLFrame rearTopCameraFrame;      //a dedicated camera to view from rear top looking down
	GLFrame CenterViewCameraFrame;//view from center
	GLFrame CompassCameraFrame;//compass view
	GLFrame ScanPanelViewCameraFrame;//panel and screen move
	GLFrame PanoViewCameraFrame;//pano view

	float TrackSpeed;
	GLFrame TriangleCameraFrame;
	GLFrame CheckViewCameraFrame;
	GLFrame PanoTelViewCameraFrame[2];//pano view_tel
	GLFrame  TrackCameraFrame;

	GLFrame LeftPanoViewCameraFrame;//left pano view
	GLFrame RightPanoViewCameraFrame;//right pano view
	GLFrame LeftSmallPanoViewCameraFrame;//small left pano view
	GLFrame TouchCrossCameraFrame;//small left pano view
	GLFrame RightSmallPanoViewCameraFrame;//small right pano view
	GLFrame ExtCameraFrame;//extent camera view
	PresetCameraGroup mPresetCamGroup;	// a set of 6 fixed position vitual camera positions that can be ajusted in freeView mode
#define EXTENSION_TEXTURE_COUNT (EXT_CAM_COUNT)

	GLFrame	VGACameraFrame;
	GLFrame	SDICameraFrame;
	GLFrame Render2FrontCameraFrame;
	GLFrame ChosenCameraFrame;
	GLFrame targetFrame[2];
	GLFrame CenterCameraFrame;
#define VGA_TEXTURE_COUNT (VGA_CAM_COUNT)
#define SDI_TEXTURE_COUNT (SDI_CAM_COUNT)
#define CHOSEN_TEXTURE_COUNT (CHOSEN_CAM_COUNT)
#define PETAL_TEXTURE_COUNT (1+1+1)

#define ALPHA_TEXTURE_IDX	(1)
//#define ALPHA_TEXTURE_IDX1	(1+2)

	GLuint textures[PETAL_TEXTURE_COUNT];
	GLuint GL_TextureIDs[PETAL_TEXTURE_COUNT];

	DynamicTrack *p_DynamicTrack;
	CornerMarkerGroup *p_CornerMarkerGroup;
	float* pConerMarkerColors[CORNER_COUNT];
	float   m_DynamicWheelAngle;
	GLuint alphaMask[ALPHA_MASK_HEIGHT*ALPHA_MASK_WIDTH];//={0};
//	GLuint alphaMask0[ALPHA_MASK_HEIGHT*ALPHA_MASK_WIDTH];//={0};
//	GLuint alphaMask1[ALPHA_MASK_HEIGHT*ALPHA_MASK_WIDTH];//={0};

	STLASCIILoader BowlLoader;
	STLASCIILoader PanelLoader;
	GLMmodel *VehicleLoader;

	int g_subwindowWidth ;
	int  g_subwindowHeight ;

	int g_windowWidth ;
	int g_windowHeight ;
	int g_nonFullwindowWidth;
	int g_nonFullwindowHeight;
	bool isFullscreen;
	bool DisFullscreen;
	bool bRotTimerStart;
	bool bControlViewCamera;
	bool isCalibTimeOn;
	bool isDirectionOn;
	bool stopcenterviewrotate;
	int rotateangle_per_second;
	float PanoLen;
	float PanoHeight;
	int GetWindowWidth(){return g_windowWidth;};
	int GetWindowHeight(){return g_windowHeight;};
	GLuint GL_ChosenTextureIDs[CHOSEN_TEXTURE_COUNT];
	GLuint GL_CenterTextureIDs[CHOSEN_TEXTURE_COUNT];
	GLuint GL_VGATextureIDs[VGA_TEXTURE_COUNT];
	GLuint GL_TargetTextureIDs[TARGET_CAM_COUNT];
	GLuint GL_SDITextureIDs[SDI_TEXTURE_COUNT];
	GLuint GL_FBOTextureIDs[1];
	GLuint VGATextures[VGA_TEXTURE_COUNT];
	GLuint SDITextures[SDI_TEXTURE_COUNT];
	GLuint GL_ChosenTextures[CHOSEN_TEXTURE_COUNT];
	GLuint GL_CenterTextures[CHOSEN_TEXTURE_COUNT];
	GLuint GL_ExtensionTextureIDs[EXTENSION_TEXTURE_COUNT];
	GLuint extensionTextures[EXTENSION_TEXTURE_COUNT];
	GLuint GL_IconTextureIDs[1];
	GLuint iconTextures[1];
	GLuint GL_IconRuler45TextureIDs[1];
	GLuint iconRuler45Textures[1];
	GLuint GL_IconRuler90TextureIDs[1];
	GLuint iconRuler90Textures[1];
	GLuint GL_IconRuler180TextureIDs[1];
	GLuint iconRuler180Textures[1];

	inline void setMouseCor(int x, int y){MOUSEx = x;  MOUSEy = y;};
	inline void setMouseButton(int button){BUTTON=button;};
	inline void updateRotate(float x, float y){ROTx +=x; ROTy +=y;};
	inline void updateRotateZ(float z){ROTz += z;};
	inline void updatePano(float x, float y){PANx +=x;PANy +=y;};
	inline void updateScale(float s, float o){scale +=s; oScale +=o;};
	int MOUSEx , MOUSEy , BUTTON ;
	float ROTx , ROTy , ROTz;
	float PANx , PANy ;
	float scale ;
	float oScale ;
	vector<cv::Point2f> pixleList[CAM_COUNT];
	vector<cv::Point2f> Panel_pixleList[CAM_COUNT];
	CStopWatch rotTimer;

	int alpha_x_count ;
	int alpha_y_count ;
	float alpha_x_step;
	float alpha_y_step;

	unsigned int m_presetCameraRotateCounter;
	static const int PRESET_CAMERA_ROTATE_MAX = 15;

	int m_ExtVideoId;
	int m_VGAVideoId;
	int m_SDIVideoId;
	int m_ChosenVideoId;
	float distance_of_tank[4];
	float cross_center_pos[2];
	bool follow_enable;
	float set_scan_region_angle;
	float last_region_forward_distance;

	bool send_follow_angle_enable;
	float follow_angle[2];

        float ALPHA_ZOOM_SCALE;
	bool enable_show_ruler;
	float ruler_angle;
	float pano_delta_angle;

	Calibrate * p_LineofRuler;
	bool refresh_ruler;
	float ruler_reference_angle;

	int track_control_params[4];

	bool EnterSinglePictureSaveMode;
	int enterNumberofCam;
	bool picSaveState[MAX_PANO_CAMERA_COUNT];
	bool EnablePanoFloat;//enable channel up and down
	int testPanoNumber;

	float PanoFloatData[CAM_COUNT];

	bool PanoDirectionLeft;

	float channel_left_scale[CAM_COUNT];
	float channel_right_scale[CAM_COUNT];

	float move_hor[CAM_COUNT];

	float move_hor_scale[CAM_COUNT];
	float move_ver_scale[CAM_COUNT];

	float rotate_angle[CAM_COUNT];

	bool psy_button_f1;
	bool psy_button_f2;
	bool psy_button_f3;
	bool psy_button_f8;

	float canon_hor_angle;
	float canon_ver_angle;
	float gun_hor_angle;
	float gun_ver_angle;


	float calc_hor_data;
	float calc_ver_data;

	int touch_pos_x;
	int touch_pos_y;

	int state_label_data[12][3];

	float test_angle;

	int hide_label_state;

	OitVehicle *pPano;

	vector <vector <int> > overlappoint[CAM_COUNT];

	GLFrame NeedleCameraFrame;
	
#if ADD_FUCNTION_BY_JIMMY
	uint8 light_state[36];
	uint8 selftest_state[10];
	//uint8 last_light_state[36];
#endif
	Interface_CenterPoint_setter *p_ics;
	Interface_CenterPoint_getter *p_cpg;
	InterfaceTouch_Center_Cross *p_icc;
	CenterPoint_caculater *p_cpc;
	IFEnhStateMachineGroup * p_EnhStateMachineGroup;
	GLT_STOCK_SHADER GetShaderIDByState(bool ispano=true);

};

void* getDefaultShaderMgr();
void * getDefaultTransformPipeline(GLEnv &m_env);

void SendBackXY(int *Pos);


#endif /* GLRENDER_H_ */
