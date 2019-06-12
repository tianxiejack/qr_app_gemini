#ifndef ALARMTARGET_H_
#define ALARMTARGET_H_

#include "StlGlDefines.h"
#include <string.h>
#include <opencv2/opencv.hpp>
#define ALARM_TARGET_CHANNEL_MAX 4
#define ALARM_TARGET_MAX 40
enum{
	TYPE_CROSS_BORDER=0,
	TYPE_MOVE,
	TYPE_INVADE,
	TYPE_LOST,
	ALARM_TYPE_MAX
};



using namespace cv;

typedef struct _RectTarget{
public:
	int valid_count;
	Rect rect[ALARM_TARGET_MAX];
}RectTarget;

class AlarmTarget{
public:
	AlarmTarget();
	~AlarmTarget(){};
	void Reset(){
		for(int i=0;i<ALARM_TARGET_CHANNEL_MAX;i++)
		{
			TargetCount[i]=0;
		}
	};

	void SetTargetCount(int chId,int target_count){
		TargetCount[chId%ALARM_TARGET_CHANNEL_MAX]=target_count;};
	int GetTargetCount(int chId){return TargetCount[chId%ALARM_TARGET_CHANNEL_MAX];};
	void SetTargetType(int chId,int type){TargetType[chId]=type%ALARM_TYPE_MAX;};
	int GetTargetType(int chId){return TargetType[chId]; };
	void SetSingleRectangle(int chId,int number,Rect set_rect){
		RectArray[chId].rect[number]=set_rect;
	};
	Rect GetSingleRectangle(int chId,int number){
		return RectArray[chId].rect[number];
	};

private:
	RectTarget RectArray[ALARM_TARGET_CHANNEL_MAX];
	int TargetCount[ALARM_TARGET_CHANNEL_MAX];
	int TargetType[ALARM_TARGET_CHANNEL_MAX];
};



#endif /* ALARMTARGET_H_ */
