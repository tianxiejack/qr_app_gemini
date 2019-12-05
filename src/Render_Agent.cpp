/*
 *  Render_Agent.cpp
 *
 *  Created on: 2018年8月11日
 *      Author: fsmdn121
 */
#include"Render_Agent.h"
#include "GLRender.h"

extern Render render;
extern char chosenCam[2];
extern int center_cam[2];
extern int Touch_Cam_Idx[CAM_COUNT];
extern bool isEnhanceOn;
Render_Agent::Render_Agent()
{

}
bool Render_Agent:: IsUsePart()
 {
	return render.Get_p_ics()->IsUsePart();
 }
Center_Rect *Render_Agent::GetCurrentRect()
{
	int current_chosenCam[2]={0};
	int Cam_num[CAM_COUNT]={0};
		for(int i=0;i<CAM_COUNT;i++)
		{
			Cam_num[i]=Touch_Cam_Idx[i];
		}
	current_chosenCam[MAIN]=Cam_num[center_cam[MAIN]];
	CenterRect *temprect=render.Get_p_ics()->GetCurrentRect(current_chosenCam[MAIN]);
	return temprect;
}

float Render_Agent::GetTouchForesightPosY()
{
return 	render.Get_p_cpg()->GetTouchForesightPosY();
}
 char Render_Agent::GetCurrentChosenCam()
 {
	 	 return (render.GetChosenCamidx()-1);
 }
	 bool Render_Agent::ISenhance()
	 {
		 return isEnhanceOn;
	 }
		 	bool Render_Agent::IsProducerRGB()
		{
		 		return render.IsProducerRGB();
		}
			 bool Render_Agent::GetReset(int idx)
			 {
				 return render.GetVideoReset(idx);
			 }
			 void Render_Agent::SetReset(bool tof,int idx)
			 {
				 render.SetVideoReset(tof,idx);
			 }
