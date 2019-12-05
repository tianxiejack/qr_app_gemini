#ifndef _GLENV_H_
#define  _GLENV_H_
#include <GLMatrixStack.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>
#include <GLFrustum.h>
#include"StlGlDefines.h"
#include"PBOManager.h"
#include"PBO_FBO_Facade.h"
#include"CaptureGroup.h"
class GLEnv
{
public:
	GLEnv(CaptureGroup *p_pano=NULL,CaptureGroup *p_chosen=NULL,CaptureGroup *p_misc=NULL);
	~GLEnv(){};
	void init(CaptureGroup *p_pano,CaptureGroup *p_chosen,CaptureGroup *p_mvdetect,CaptureGroup *p_mvdetect_add,CaptureGroup *p_misc);
	GLMatrixStack *GetmodelViewMatrix();
	GLMatrixStack	*GetprojectionMatrix();
	GLGeometryTransform *GettransformPipeline();
	GLFrustum *GetviewFrustum();
	GLBatch *GetPanel_Petal(int idx);
	GLBatch *Getp_Panel_Petal_OverLap(int idx);
	void Setp_Panel_Petal_OverLap(int idx,GLBatch *recv_Panel_OverLap);
	GLBatch *Getp_Panel_OverLap(int idx);
	void Setp_Panel_OverLap(int idx,GLBatch *recv_batch);
	GLBatch *Getp_shadowBatch();
	PBOReceiver *Getp_PBORcr();
	PBOSender *Getp_PBOMgr();
	
#if ADD_FUCNTION_BY_JIMMY
			GLBatch *Getp_CircleFanBatch();
			GLBatch *Getp_CircleLineBatch();
			GLBatch *Getp_DividedLineBatch();
			GLBatch *Getp_DirectTriangle_Batch();
			GLBatch *Getp_surroundVertical_Batch();
			GLBatch *Getp_SurroundVertical_MoveLine_Batch();
			GLBatch *Getp_TurretVertical_MoveLine_Batch();
			
#endif
	PBOSender *Getp_PBOExtMgr();
	PBOSender *Getp_PBOVGAMgr();
	PBOSender *Getp_PBOTargetMgr();
	PBOSender *Getp_PBOSDIMgr();
	PBOSender *Getp_PBOChosenMgr();
	PBOSender *Getp_PBOCenterMgr();
	FBOManager *Getp_FBOmgr();
	pPBO_FBO_Facade Getp_FboPboFacade();
	void Set_FboPboFacade(FBOManager  &FBOMgr,PBOReceiver  &PBORcr);
	CaptureGroup *GetMvdectCaptureGroup(){return m_mvdectCaptureGroup;};
	CaptureGroup *GetPanoCaptureGroup(){return m_panoCaptureGroup;};
	CaptureGroup *GetChosenCaptureGroup(){return m_chosenCaptureGroup;};
	CaptureGroup *GetMiscCaptureGroup(){return m_miscCaptureGroup;};
	GLBatch *Getdegreescale45Batch(){return &degreescale45Batch;};
	GLBatch *Getdegreescale90Batch(){return &degreescale90Batch;};
	GLBatch *Getdegreescale180Batch(){return &degreescale180Batch;};
private:
	CaptureGroup * m_panoCaptureGroup;
	CaptureGroup * m_chosenCaptureGroup;
	CaptureGroup * m_miscCaptureGroup;
	CaptureGroup *m_mvdectCaptureGroup;
	CaptureGroup *m_mvdect_addCaptureGroup;
	GLMatrixStack	modelViewMatrix;
	GLMatrixStack	projectionMatrix;
	GLGeometryTransform transformPipeline;
	GLFrustum viewFrustum;
	GLBatch Panel_Petal[CAM_COUNT];
	GLBatch *Panel_Petal_OverLap[CAM_COUNT];
	GLBatch *Panel_OverLap[CAM_COUNT];
	GLBatch shadowBatch; //the shadow rect under vehicle
	GLBatch degreescale45Batch; //the degree scale (45-0-45)
	GLBatch degreescale90Batch; //the degree scale (90-0-90)
	GLBatch degreescale180Batch; //the degree scale (180-0-180)

	
#if ADD_FUCNTION_BY_JIMMY
			GLBatch Circle_Fan_Batch;	
			GLBatch directTriangle_Batch;
			GLBatch surroundVertical_Batch;
			GLBatch surroundVertical_MoveLine_Batch;
			GLBatch turretVertical_MoveLine_Batch;
			GLBatch Circle_Batch;
			GLBatch Divided_Line;
#endif	
	PBOReceiver PBORcr;
	PBOSender PBOMgr;
	PBOSender PBOExtMgr;
	PBOSender PBOVGAMgr;
	PBOSender PBOTargetMgr;
	PBOSender PBOSDIMgr;
	PBOSender PBOChosenMgr;
	PBOSender PBOCenterMgr;
	FBOManager FBOmgr;
	pPBO_FBO_Facade mp_FboPboFacade;
};
#endif
