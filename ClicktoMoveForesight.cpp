/*
 * ClicktoMoveForesight.cpp
 *
 *  Created on: 2018年7月16日
 *      Author: fsmdn121
 */

#include"ClicktoMoveForesight.h"
#include "stdio.h"
#include "GLRender.h"
#include "ForeSight.h"
extern float temp_math[2];
extern Render render;
extern ForeSightPos foresightPos[MS_COUNT];
extern char chosenCam[2];
extern int center_cam[2];
#define nowW	1024
#define nowH	768
int Touch_Cam_Idx[CAM_COUNT] = { 1, 0, 7, 6, 5, 4, 3, 2 };
void RegionalViewreflash() {
	int Cam_num[CAM_COUNT] = { 0 };
	for (int i = 0; i < CAM_COUNT; i++) {
		Cam_num[i] = Touch_Cam_Idx[i];
	}
	if (chosenCam[MAIN] != Cam_num[center_cam[MAIN]] + 1) {
		chosenCam[MAIN] = Cam_num[center_cam[MAIN]] + 1;

		render.ChangeMainChosenCamidx(chosenCam[MAIN]);
	}
}

void transAngle2CamIdx(float Angle, int mainOrsub) {
	if (Angle >= 45 && Angle < 90) {
		center_cam[mainOrsub] = 1;
	} else if (Angle >= 0 / CAM_COUNT * 2 && Angle < 45) {
		center_cam[mainOrsub] = 0;
	} else if (Angle >= 315 || Angle < 0) {
		center_cam[mainOrsub] = 7;
	} else if (Angle >= 270 && Angle < 315) {
		center_cam[mainOrsub] = 6;
	} else if (Angle >= 225 && Angle < 270) {
		center_cam[mainOrsub] = 5;
	} else if (Angle >= 180 && Angle < 225) {
		center_cam[mainOrsub] = 4;
	} else if (Angle >= 135 && Angle < 180) {
		center_cam[mainOrsub] = 3;
	} else if (Angle >= 90 && Angle < 135) {
		center_cam[mainOrsub] = 2;
	}
}

char transIdx(char idx) {
	int ret = -1;
	switch (idx) {
	case 0:
		ret = 3;
		break;
	case 1:
		ret = 2;
		break;
	case 2:
		ret = 1;
		break;
	case 3:
		ret = 0;
		break;
	case 4:
		ret = 4;
		break;
	case 5:
		ret = 5;
		break;
	case 6:
		ret = 6;
		break;
	case 7:
		ret = 7;
		break;
	default:
		break;
	}
	return ret;
}

void chosenCamMove(bool isleft, int chosenidx) {
#if USE_BMPCAP &&FORCE_1920
	float currentScreenW=1024;
	float currentScreenH=768;
	int y[2]= {704,550};
#else
	float currentScreenW = SDI_WIDTH;
	float currentScreenH = SDI_HEIGHT;
	int y[2] = { 990, 773 };  //704/768*1080   550/768*1080
#endif
	int nowx = 0;
	int nowy = 0;
	float x[CAM_COUNT / 2] = { 0 };

	for (int i = 0; i < CAM_COUNT / 2; i++) {
		x[i] = currentScreenW / (CAM_COUNT / 2) / 2
				+ currentScreenW / (CAM_COUNT / 2) * i;
	}
//  		1234
//			5067

	if (isleft) {
		switch (chosenidx) {
		case 0:
			nowx = x[1];
			nowy = y[0];
			break;
		case 1:
			nowx = x[2];
			nowy = y[0];
			break;
		case 2:
			nowx = x[3];
			nowy = y[0];
			break;
		case 3:
			nowx = x[3];
			nowy = y[1];
			break;
		case 4:
			nowx = x[2];
			nowy = y[1];
			break;
		case 5:
			nowx = x[1];
			nowy = y[1];
			break;
		case 6:
			nowx = x[0];
			nowy = y[1];
			break;
		case 7:
			nowx = x[0];
			nowy = y[0];
			break;
		default:
			break;
		}
	} else {
		switch (chosenidx) {
		case 0:
			nowx = x[3];
			nowy = y[0];
			break;
		case 1:
			nowx = x[3];
			nowy = y[1];
			break;
		case 2:
			nowx = x[2];
			nowy = y[1];
			break;
		case 3:
			nowx = x[1];
			nowy = y[1];
			break;
		case 4:
			nowx = x[0];
			nowy = y[1];
			break;
		case 5:
			nowx = x[0];
			nowy = y[0];
			break;
		case 6:
			nowx = x[1];
			nowy = y[0];
			break;
		case 7:
			nowx = x[2];
			nowy = y[0];
			break;
		default:
			break;
		}
	}
	clicktoMoveForesight(nowx, nowy, MAIN);
}

void Render::ChangeChosenByForesight() {
	float FXangle = foresightPos[MAIN].GetAngle()[0];
	float Angle = FXangle;
	while (Angle > 360) {
		Angle -= 360.0;
	}
	int Cam_num[CAM_COUNT] = { 0 };
	for (int i = 0; i < CAM_COUNT; i++) {
		Cam_num[i] = Touch_Cam_Idx[i];
	}
	transAngle2CamIdx(Angle, MAIN);
	chosenCam[MAIN] = Cam_num[center_cam[MAIN]] + 1;
	static char lastChosenCam = 2;
	if (lastChosenCam != chosenCam[MAIN]) {
		render.ChangeMainChosenCamidx(chosenCam[MAIN]);
		lastChosenCam = chosenCam[MAIN];
	}
}

void clicktoMoveForesight(int x, int y, int mainOrsub) {
#define Render_FBO_ALL_VIEW_MODE 0
#define	 Render_FBO_ALL_VIEW_SCAN_MODE 1
	int dd = 2;
	int touch_delta_angle = 13;
#if USE_BMPCAP &&FORCE_1920
	float Xangle=(float)(x*360.0/nowW);
	float Yangle=(float)(y*360.0/nowH);
#else
	float Xangle = (float) (x / 1920.0 * 1024.0 * 360.0 / nowW);
	float Yangle = (float) (y / 1080.0 * 768.0 * 360.0 / nowH);
#endif
	bool updateYangle = false;
	if (Yangle >= 215 && Yangle <= 360)
		render.SetFboMode(Render_FBO_ALL_VIEW_MODE);
	else if (Yangle >= 0 && Yangle < 215) {
		render.SetFboMode(Render_FBO_ALL_VIEW_SCAN_MODE);
	}
	if (Yangle >= 288 + touch_delta_angle - dd
			&& Yangle <= 347 + touch_delta_angle)  //Up
					{
		foresightPos[mainOrsub].SetPos(Xangle, Yangle, mainOrsub);
		Yangle -= (288 + touch_delta_angle - dd);
		updateYangle = true;
	} else if (Yangle >= 216 + touch_delta_angle - dd
			&& Yangle <= 275 + touch_delta_angle)  //Down
					{
		foresightPos[mainOrsub].SetPos(Xangle, Yangle, mainOrsub);
		Yangle -= (216 + touch_delta_angle - dd);
		updateYangle = true;
	}
	if (updateYangle) {
		Yangle /= 61.0;
		Yangle = 1 - Yangle;
		float pos[2] = { Xangle, Yangle };
		render.Get_p_ics()->SetTouchForesightPos(pos);
		render.ChangeChosenByForesight();
	}
}
