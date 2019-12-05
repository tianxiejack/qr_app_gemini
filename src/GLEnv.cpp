#include"GLEnv.h"
#include"StlGlDefines.h"
#if 1
GLenum iniformat=GL_BGR;
GLenum internalFormat=GL_RGB8;
static int iniCC=3;
#else
GLenum iniformat=GL_BGRA;
GLenum internalFormat=GL_RGBA8;
static int iniCC=4;
#endif
GLEnv::GLEnv(CaptureGroup *p_pano,CaptureGroup *p_chosen,CaptureGroup *p_misc):
m_panoCaptureGroup(p_pano),
m_chosenCaptureGroup(p_chosen),
m_miscCaptureGroup(p_misc),
PBOMgr(PBOSender(3,PANO_TEXTURE_WIDTH,PANO_TEXTURE_HEIGHT,iniCC,iniformat,0.6,0.4)),
FBOmgr(FBOManager(CURRENT_SCREEN_WIDTH,CURRENT_SCREEN_HEIGHT,iniformat,internalFormat)),
PBORcr(PBOReceiver(PBO_ALTERNATE_NUM,CURRENT_SCREEN_WIDTH,CURRENT_SCREEN_HEIGHT,iniCC,iniformat)),
PBOExtMgr(PBOSender(MAGICAL_NUM+EXT_CAM_COUNT,PAL_WIDTH,PAL_HEIGHT,iniCC,iniformat)),//图片
PBOVGAMgr(PBOSender(MAGICAL_NUM+VGA_CAM_COUNT,VGA_WIDTH,VGA_HEIGHT,iniCC,iniformat)),
PBOSDIMgr(PBOSender(MAGICAL_NUM+SDI_CAM_COUNT,SDI_WIDTH,SDI_HEIGHT,iniCC,iniformat)),
PBOChosenMgr(PBOSender(MAGICAL_NUM+CHOSEN_CAM_COUNT,SDI_WIDTH,SDI_HEIGHT,iniCC,iniformat)),
PBOCenterMgr(PBOSender(MAGICAL_NUM+CHOSEN_CAM_COUNT,CENTER_WIDTH,CENTER_HEIGHT,iniCC,iniformat)),

PBOTargetMgr(PBOSender(MAGICAL_NUM+TARGET_CAM_COUNT,FPGA_SINGLE_PIC_W,FPGA_SINGLE_PIC_H,iniCC,iniformat))
{

}

GLMatrixStack *GLEnv::GetmodelViewMatrix()
{
	return &modelViewMatrix;
}
GLMatrixStack	*GLEnv::GetprojectionMatrix()
{
	return &projectionMatrix;
}
GLGeometryTransform *GLEnv::GettransformPipeline()
{
	return &transformPipeline;
}
GLFrustum *GLEnv::GetviewFrustum()
{
	return &viewFrustum;
}
GLBatch *GLEnv::GetPanel_Petal(int idx)
{
	return &Panel_Petal[idx];
}
GLBatch *GLEnv::Getp_Panel_Petal_OverLap(int idx)
{
	return Panel_Petal_OverLap[idx];
}
void GLEnv::Setp_Panel_Petal_OverLap(int idx,GLBatch *recv_Panel_OverLap)
{
	Panel_Petal_OverLap[idx]=recv_Panel_OverLap;
}

void GLEnv::Setp_Panel_OverLap(int idx,GLBatch *recv_batch)
{
	 Panel_OverLap[idx]=recv_batch;
}
GLBatch *GLEnv::Getp_Panel_OverLap(int idx)
{
	return Panel_OverLap[idx];
}
GLBatch *GLEnv::Getp_shadowBatch()
{
	return &shadowBatch;
}

#if ADD_FUCNTION_BY_JIMMY
GLBatch *GLEnv::Getp_CircleFanBatch()
{
	return &Circle_Fan_Batch;
}
GLBatch *GLEnv::Getp_CircleLineBatch()
{
	return &Circle_Batch;
}
GLBatch *GLEnv::Getp_DividedLineBatch()
{
	return &Divided_Line;
}
GLBatch *GLEnv::Getp_DirectTriangle_Batch()
{
	return &directTriangle_Batch;
}
GLBatch *GLEnv::Getp_surroundVertical_Batch()
{
	return &surroundVertical_Batch;
}
GLBatch *GLEnv::Getp_SurroundVertical_MoveLine_Batch()
{
	return &surroundVertical_MoveLine_Batch;
}
GLBatch *GLEnv::Getp_TurretVertical_MoveLine_Batch()
{
	return &turretVertical_MoveLine_Batch;
}

#endif


PBOReceiver *GLEnv::Getp_PBORcr()
{
	return 	&PBORcr;
}
PBOSender *GLEnv::Getp_PBOMgr()
{
	return 	&	 PBOMgr;
}
PBOSender *GLEnv::Getp_PBOExtMgr()
{
	return 	&	 PBOExtMgr;
}
PBOSender *GLEnv::Getp_PBOVGAMgr()
{
	return 	&	 PBOVGAMgr;
}
PBOSender *GLEnv::Getp_PBOTargetMgr()
{
	return 	&	 PBOTargetMgr;
}
PBOSender *GLEnv::Getp_PBOSDIMgr()
{
	return 	&	 PBOSDIMgr;
}
PBOSender *GLEnv::Getp_PBOChosenMgr()
{
	return 	&	 PBOChosenMgr;
}
PBOSender *GLEnv::Getp_PBOCenterMgr()
{
	return 	&	 PBOCenterMgr;
}
FBOManager *GLEnv::Getp_FBOmgr()
{
	return 	&	 FBOmgr;
}
pPBO_FBO_Facade GLEnv::Getp_FboPboFacade()
{
	return 	mp_FboPboFacade;
}
void GLEnv::Set_FboPboFacade(FBOManager  &FBOMgr,PBOReceiver  &PBORcr)
{
	mp_FboPboFacade=new PBO_FBO_Facade(FBOMgr,PBORcr);
}

void GLEnv::init(CaptureGroup *p_pano,CaptureGroup *p_chosen,CaptureGroup *p_mvdetect,CaptureGroup *p_mvdetect_add,CaptureGroup *p_misc)
{
	m_panoCaptureGroup=p_pano;
	m_chosenCaptureGroup=p_chosen;
	m_miscCaptureGroup=p_misc;
	m_mvdectCaptureGroup=p_mvdetect;
	m_mvdect_addCaptureGroup=p_mvdetect_add;
}

