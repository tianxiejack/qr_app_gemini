/*
 * CenterPoint_caculate.h
 *
 *  Created on: 2018年9月25日
 *      Author: fsmdn121
 */

#ifndef CENTERPOINT_CACULATE_H_
#define CENTERPOINT_CACULATE_H_
#include "stdlib.h"
typedef struct CenterRect
{
	int x;
	int y;
	int width;
	int height;
}Center_Rect;
class Interface_CenterPoint_setter
{
public:
	virtual void SetTouchForesightPos(float *pos)=0;
	virtual Center_Rect * GetCurrentRect(int idx)=0;
	virtual bool IsUsePart()=0;
	virtual void SetUsePart(bool tof)=0;
};

class Interface_CenterPoint_getter
{
public:
	virtual float GetTouchForesightPosY()=0;
};

class Interface_CenterPoint_caculate:public Interface_CenterPoint_getter,
																			public Interface_CenterPoint_setter
{
};

class CenterPoint_caculater:public Interface_CenterPoint_caculate
{
	public:
	CenterPoint_caculater( Center_Rect *prect=NULL);
	 void SetUsePart(bool tof){
		 m_usepart=tof;
	 };
	Center_Rect	* GetCurrentRect(int idx)
	{
		return &CenterRect[idx];
	};
	bool IsUsePart(){return m_usepart;};
	 void SetTouchForesightPos(float *pos);
	 float GetTouchForesightPosY(){return tf_pos[1];};
	private:
	 float tf_pos[2];
	bool m_usepart;
	Center_Rect  CenterRect[CAM_COUNT];
};






#endif /* CENTERPOINT_CACULATE_H_ */
