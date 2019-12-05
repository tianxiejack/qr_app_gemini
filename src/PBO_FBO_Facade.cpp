#include "PBO_FBO_Facade.h"

 PBO_FBO_Facade:: PBO_FBO_Facade(FBOManager  &FBOMgr,PBOReceiver & PBORcr):
	m_FBOMgr(FBOMgr),
	m_PBORcr(PBORcr)
 {
}
void PBO_FBO_Facade::DrawAndGet(bool &Isenhdata,bool &IsTouchenhdata)
{
	m_FBOMgr.PboDraw(m_PBORcr,Isenhdata,IsTouchenhdata);
}

void PBO_FBO_Facade::Render2Front(int mainOrsub,int w,int h)
{
	m_FBOMgr.DrawTex2Front(mainOrsub,w,h);
}
