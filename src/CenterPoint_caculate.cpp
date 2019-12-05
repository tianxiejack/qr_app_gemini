/*
 * CenterPoint_caculate.cpp
 *
 *  Created on: 2018年9月25日
 *      Author: fsmdn121
 */
#include "CenterPoint_caculate.h"
CenterPoint_caculater::CenterPoint_caculater(Center_Rect *prect):m_usepart(true)
{
	tf_pos[0]=0;
	tf_pos[1]=0;
	if(!prect)
	{
		for(int i=0;i<CAM_COUNT;i++)
		{
			CenterRect[i].x=0;
			CenterRect[i].y=215;
			CenterRect[i].width=1920;
			CenterRect[i].height=650;
		}
	}
	else
	{
		for(int i=0;i<CAM_COUNT;i++)
			{
			CenterRect[i]=*prect;
			}
	}
}
#include"stdio.h"
void CenterPoint_caculater::SetTouchForesightPos(float *pos){
	pos[1]-=0.5;//0.5~-0.5   //-215～+215
	tf_pos[0]=pos[0];
	tf_pos[1]=pos[1]*430.0/1080.0;
};
