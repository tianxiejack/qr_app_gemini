#include <GLBatch.h>
class GLShaderManager;

class GLMatrixStack;
class GLFrustum;
#define USE_ON_BOARD 0

class BaseBillBoard{

		public:
			BaseBillBoard(GLShaderManager* mgr=NULL);
			virtual ~BaseBillBoard()=0;
			void Init(int x, int y, int width, int height);
			inline void setBlendMode(int mode){blendmode = mode;};
		public:
			void DrawBillBoard(int w, int h);
		protected:
			bool LoadTGATextureRect(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode);
			virtual void InitTextures() = 0;
			virtual void DoTextureBinding() = 0;
		private:
			BaseBillBoard():m_pShaderManager(NULL){};
			void SwitchBlendMode(int blendmode);
			GLShaderManager * 	m_pShaderManager;
			int 			  blendmode;
			GLBatch		  HZbatch;

		} ;

	class MenuBillBoard : public BaseBillBoard {
		public:
			MenuBillBoard(const char* menuTgaFileName=NULL,GLShaderManager* mgr=NULL);
			~MenuBillBoard();
			void SetTgaFileName(const char * name);
		protected:
			virtual void InitTextures();
			virtual void DoTextureBinding();
		private:
			void LoadTGATextureRects();
			GLuint              m_BBDTextures[1];
			char                m_BBDTextureFileName[64];
		};
//---------------------------
class MenuButton
{
public:
	MenuButton(const char* menuFileName=NULL):buttonBBoard(MenuBillBoard(menuFileName))
	{

	};
	~MenuButton(){};

	MenuBillBoard buttonBBoard;
	GLfloat * button_color[2];
	int choose_state;
	//timeval compare_time;
	float start_x;
	float start_y;
	float button_width;
	float button_height;
	GLShaderManager * shaderMgr;
	int key_data;
	bool enable_draw;

	void init_button(GLfloat * color_normal,GLfloat * color_choose,int state,float x,float y,float width,float height,GLShaderManager * shaderManager,int key=20);

	void Draw(int width, int height,GLFrustum * viewFrustum,GLMatrixStack * modelViewMatrix,GLMatrixStack *projectionMtrx,int id=0);

	void SetShaderMgr(GLShaderManager * shaderManager){shaderMgr=shaderManager;};

	bool FindPointOnButton(float x,float y);
	void SetTgaFileName(const char * name);
	int getKeycode()
	{
		return key_data;
	};

};


struct buttonMask;
class ButtonGroup
{
public:
	ButtonGroup():pbuttons(0){};
	~ButtonGroup();
	void Group_Draw(int width,int height,GLMatrixStack * p_modelViewMatrix,GLMatrixStack * p_projectionMtrx,GLFrustum * viewFrustum);
	void Update_State(InterfaceRenderBehavior* p_Host);
	void acceptButtonMask(buttonMask* mask, int count);
	void init_button_group(int button_count,float * color_normal,float * color_choose,int * state,float * x,float * y,float * width,float * height,GLShaderManager * shaderManager);
	void HightlightButton(int id);
	int GetHighlightButtonId(){return currentHightLightButtonId;};
private:
	int FindButton(float x,float y,int window_width=1,int window_height=1);

	void Append_Group(MenuButton * button);
	MenuButton * pbuttons;
	std::vector< MenuButton *> m_buttonsVect;
	std::vector<buttonMask*> m_Maskbutton;
	GLShaderManager * shaderMgr_group;
	int currentHightLightButtonId;
	int pos_x,pos_y,window_width,window_height;

};

