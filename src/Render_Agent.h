/*
 * Render_Agent.h
 *
 *  Created on: 2018年8月11日
 *      Author: fsmdn121
 */

#ifndef RENDER_AGENT_H_
#define RENDER_AGENT_H_
#include "CenterPoint_caculate.h"
class Render_Agent
{
public:
	Render_Agent();
	static 		bool IsUsePart();
	static 	Center_Rect*GetCurrentRect();
	static float GetTouchForesightPosY();
	static char GetCurrentChosenCam();
	static bool ISenhance();
	static 	bool IsProducerRGB();
	static bool GetReset(int idx);
	static void SetReset(bool tof,int idx);
};



#endif /* RENDER_AGENT_H_ */
