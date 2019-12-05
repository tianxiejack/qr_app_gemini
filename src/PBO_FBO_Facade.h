#ifndef POB_FBO_FACADE_
#define POB_FBO_FACADE_
#include "FBOManager.h"
#include "PBOManager.h"

using namespace std;
typedef  class PBO_FBO_Facade
{
public:
	PBO_FBO_Facade(FBOManager  &FBOMgr,PBOReceiver  &PBORcr);
	~PBO_FBO_Facade(){};
	void DrawAndGet(bool &Isenhdata,bool &IsTouchenhdata);
	void Render2Front(int mainOrsub,int w,int h);
	bool IsFboUsed(){return m_FBOMgr.IsFboUsed();};
private:
	FBOManager  &m_FBOMgr;
	PBOReceiver  &m_PBORcr;
}* pPBO_FBO_Facade;
#endif
