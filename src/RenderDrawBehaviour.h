#ifndef  _RENDER_DRAW_BEHAVIOUR_
#define _RENDER_DRAW_BEHAVIOUR_
using namespace std;
typedef class InterFaceDrawBehaviour
{
public:
		virtual void FBOdraw(bool &Isenhdata ,bool &IsTouchenhdata)=0;
}*p_InterFaceDrawBehaviour;
#endif
