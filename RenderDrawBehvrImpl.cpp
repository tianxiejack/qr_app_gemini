/*FBOdraw() will be expanded,
 * so I write it in a single file to support GLRender,
 * by this means I can add various of cases.
 *author: Fwy */
#include "GLRender.h"
#include"GLEnv.h"
extern GLEnv env1,env2;
float Rh=0;
float Lh=0;
void Render::FBOdraw(bool &Isenhdata,bool &IsTouchenhdata)
{
	GLEnv &env=env1;
	bool needSendData=true;
	switch(fboMode)
	{
		case  FBO_ALL_VIEW_MODE:
			//RenderLeftPanoView(Isenhdata,env,0,1080.0*540.0/1080.0,1920, 1080.0*540.0/1080.0,MAIN,false);
			RenderLeftPanoView(Isenhdata,env,0,0,1920, 1080,MAIN,false);
			break;
		default:
			break;
	}
}

