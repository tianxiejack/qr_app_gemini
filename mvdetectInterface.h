/*
 * mvdetectInterface.h
 *
 *  Created on: 2017年9月4日
 *      Author: cr
 */

#ifndef MVDECTINTERFACE_H_
#define MVDECTINTERFACE_H_
#include "StlGlDefines.h"
#include "infoHead.h"
//typedef	struct	 _trk_rect_t TRK_RECT_INFO;
typedef struct _OSA_SemHndl OSA_SemHndl,* pOSA_SemHndl;

typedef	enum{
	WARN_MOVEDETECT_MODE		=	0x01,
	WARN_BOUNDARY_MODE			=	0x02,
	WARN_INVADE_MODE					=	0x04,
	WARN_LOST_MODE						=	0x08,
	WARN_INVAD_LOST_MODE		=	0x10,
}WARN_MODE;
typedef void ( *LPNOTIFYFUNC)(void *context, int chId);

class	CMvDectInterface
{
public:
	CMvDectInterface(){};
	virtual ~CMvDectInterface(){};

public:
	virtual	int	init(LPNOTIFYFUNC	notifyFunc, void *context){return 1;};
	virtual	int destroy(){return 1;};

	virtual	void	setFrame(cv::Mat	src ,int srcwidth , int srcheight ,int chId,int accuracy=10,/*0~4  4*/int inputMinArea=8/*6*/,int inputMaxArea=200,int threshold = 30){};//输入视频帧
	virtual	void	setWarningRoi(std::vector<cv::Point2i>	warnRoi,	int chId	= 0){};//设置缩放前的警戒区域
	virtual	void	setWarnMode(WARN_MODE	warnMode,	int chId	= 0){};//设置警戒模式
	virtual	void	getMoveTarget(std::vector<TRK_RECT_INFO>	&resTarget,	int chId	= 0){};//移动目标
	virtual void mvOpen(){};
	virtual void mvClose(){};
	virtual bool  isNotStopped(){};
	virtual bool isRunning(){};
};

CMvDectInterface *MvDetector_Create();

#define  MAX_RECT_COUNT 10
#define MAX_TARGET_NUM 4
#define AREAH	1
struct mvRect
{
	//cv::Rect outRect;
	TRK_RECT_INFO		outRect;
	float color[3];
	int camIdx;
	float x_angle();//const {return camIdx*SDI_WIDTH+outRect->targetRect.x;};
	float y_angle();//const{return outRect->targetRect.y;};
};

class MvDetect
{
public:
	MvDetect(CMvDectInterface *pmvIf);
	~MvDetect();
	 void mvOpen();
	 void mvClose();
	 bool  isNotStopped(){return m_pMovDetector->isNotStopped();};
	 bool  isRunning();

	void DrawRectOnpic(unsigned char *src,int capidx,int CC_enh_mvd);
	void DrawRectOnChosenPic(unsigned char *src,int CC_enh_mvd);
	void uyvy2gray(unsigned char* src,unsigned char* dst,int idx,int width=MAX_SCREEN_WIDTH,int height=MAX_SCREEN_HEIGHT);
	void init(int w=MAX_SCREEN_WIDTH,int h=MAX_SCREEN_HEIGHT);
	void m_mvDetect(int idx,unsigned char* inframe,int w=MAX_SCREEN_WIDTH,int h=MAX_SCREEN_HEIGHT,bool isopen=true);
	void SetoutRect(bool isinchosen);//将检测到的每个通道里6个rect放入对应的6个容器里
	static  void NotifyFunc(void *context, int chId);
	void ClearAllVector(bool IsOpen);
	void ClearLocalRectPano(int idx=-1);
	void ClearLocalRectSingleChannel(int idx=-1);
	void ClearRecvRect(int idx=-1);
	pOSA_SemHndl GetpSemMV(int idx){return pSemMV[idx];};
	std::vector<mvRect> *Getm_WholeRect(int mainOrsub)
	{
		m_WholeRect[mainOrsub].clear();
		for(int i=0;i<CAM_COUNT;i++)
			m_WholeRect[mainOrsub].insert(m_WholeRect[mainOrsub].end(),outRect[i].begin(),outRect[i].end());
		return &m_WholeRect[mainOrsub];
	}
	std::vector<TRK_RECT_INFO> * GetptempRect_Srcptr(int idx)
		{
			return &tempRect_Srcptr[idx];
		};
	void SetLineY(int idx,int startY){/*lineY[idx]=startY;*/};
	void SetLinedalta(int idx,int delta){linedelta[idx]=delta;};
	void ReSetLineY();
	float GetRoiStartY_OffsetCoefficient(int idx);
private:
	CMvDectInterface *m_pMovDetector;
	bool enableMD[2];
	bool MDopen[2];
	std::vector<TRK_RECT_INFO>  tempRect_Srcptr[CAM_COUNT];
	unsigned char* grayFrame[CAM_COUNT];
	std::vector<mvRect>	outRect[CAM_COUNT];
	std::vector<mvRect>	outRect_Single_Channel[CAM_COUNT];
	std::vector<mvRect> m_WholeRect[2];
    pOSA_SemHndl pSemMV[CAM_COUNT];
    int lineY[CAM_COUNT];
    int linedelta[CAM_COUNT];
    float half_RoiAreah;
};









#endif /* MVDECTINTERFACE_H_ */
