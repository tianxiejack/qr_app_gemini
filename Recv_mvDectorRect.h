#ifndef RECV_MVDECTOR_RECT_H
#define RECV_MVDECTOR_RECT_H


#include "GLRender.h"
class Recv_MvDector_Rect
{
public:
	Recv_MvDector_Rect();
	~Recv_MvDector_Rect(){};
	void SetRectNum(int num){recvNum=num;};
	int GetRecvNum(){return recvNum;};
	void SetSingleRect(int i,Rect recvRect,GLint w,GLint h);
	Rect *GetSingleRect(int i);
	void InitRect(){recvNum=-1;};
private:
	Rect  TargetRect[DECTOR_TARGET_MAX];
	int recvNum;
};




#endif
