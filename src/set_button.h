#pragma once
#include <vector>
#define MENU_BUTTON_HOR_COUNT 10
#define MENU_BUTTON_VER_COUNT 8
#define MENU_BUTTON_COUNT (MENU_BUTTON_HOR_COUNT*MENU_BUTTON_VER_COUNT)
#define MENU_GROUP_COUNT 19
class MenuButton;
class GLShaderManager;
class ButtonGroup;
class GLMatrixStack;
class GLFrustum;

class InterfaceRenderBehavior{
public:
   virtual int GetWindowWidth() = 0;
   virtual int GetWindowHeight()= 0;
   virtual int GetTouchPosX()=0;
   virtual int GetTouchPosY()=0;
   virtual void SetTouchPosX(int x)=0;
   virtual void SetTouchPosY(int x)=0;
   virtual int getGroupMenuIndex() = 0;
   virtual void processKeycode(int keycode)=0;
};

class InterfaceButtonGroup{

public:
	virtual void Group_Draw()=0;
	virtual void Update_State()=0;
	virtual void init_button_group(GLShaderManager * shaderManager,GLMatrixStack * modelViewMatrix,
			GLMatrixStack * projectionMtrx,GLFrustum *     pViewFrustrm)=0;

};


class multiLayerButtonGroup:public InterfaceButtonGroup
{
public:
	multiLayerButtonGroup( InterfaceRenderBehavior *p,int groupCount = 1);
	~multiLayerButtonGroup();
	void Group_Draw();
	void Update_State();
	void init_button_group(GLShaderManager *shaderManager,GLMatrixStack * modelViewMatrix,
			GLMatrixStack * projectionMtrx, GLFrustum *     pViewFrustrm);
	void SetEnableDraw(bool enable);
	bool GetEnableDraw(){return enable_draw;};
	void SetcurrentActiveBGIndex(int idx){currentActiveBGIndex=idx;};
private:
	std::vector<ButtonGroup*> m_layeredButtonGroupsVect;
	int currentActiveBGIndex;
	ButtonGroup * pbuttongroups;
	bool enable_draw;
	GLMatrixStack * p_modelViewMatrix;
	GLMatrixStack * p_projectionMatrix;
	GLFrustum *     pViewFrustrm;
    InterfaceRenderBehavior* p_Host;
};
