#ifndef DATAOFALARMAREA_H_
#define DATAOFALARMAREA_H_

#include "StlGlDefines.h"
#include "AlarmTarget.h"
#include <string.h>
#include <opencv2/opencv.hpp>
#define ALARM_MAX_COUNT 4
#define ALARM_LINE_MAX_COUNT 4
#define ALARM_MAX_WIDTH 1920
#define ALARM_MAX_HEIGHT 1080
#define TYPE_ALARM_AREA 0
#define TYPE_ALARM_LINE 1

using namespace cv;

class DataofAlarmarea{
public:
	DataofAlarmarea();
	~DataofAlarmarea(){};

	int * GetAlarmAreaPos(int channel){return alarm_area_pos[channel%ALARM_MAX_COUNT];};

	void AppendAlarmArea(int pos[8]){SetAlarmAreaPos((alarm_area_index+1)%ALARM_MAX_COUNT,pos);
	alarm_area_index=((alarm_area_index+1)%ALARM_MAX_COUNT);
	alarm_area_count++;
	if(alarm_area_count>ALARM_MAX_COUNT)
		{
			alarm_area_count=ALARM_MAX_COUNT;
		}
	};
	void Reset(){alarm_area_count=0;
	alarm_area_index=0;};
	int GetAlarmAreaCount(){return alarm_area_count;};

private:
	void SetAlarmAreaPos(int channel,int pos[8]){
		for(int i=0;i<8;i++)
		{
			alarm_area_pos[channel%ALARM_MAX_COUNT][i]=pos[i];
		}
	};
	int alarm_area_pos[ALARM_MAX_COUNT][8];//x1,y1~x4,y4
	int alarm_area_index;
	int alarm_area_count;//0~4

	int alarm_line_pos[ALARM_LINE_MAX_COUNT][4];//x1,y1,x2,y2
	int alarm_line_index;
};

class BaseAlarmObject{
public:
	BaseAlarmObject(int type);
	~BaseAlarmObject(){};

	int * GetAlarmPos(int channel){
		if(alarm_type==TYPE_ALARM_AREA)
			return alarm_pos[channel%ALARM_MAX_COUNT];
		else
			return alarm_pos[channel%ALARM_LINE_MAX_COUNT];};

	void AppendAlarmArea(int * pos){
		int max=0;
		int size=0;
		if(alarm_type==TYPE_ALARM_AREA)
		{
			max=ALARM_MAX_COUNT;
			size=8;
		}
		else
		{
			max=ALARM_LINE_MAX_COUNT;
			size=8;
		}
		SetAlarmAreaPos((alarm_index)%max,pos);
		CalculatePatch(alarm_pos[alarm_index%max],alarm_patch[alarm_index%max],size,5);
		alarm_index=((alarm_index+1)%max);
		alarm_count++;
		if(alarm_count>max)
		{
			alarm_count=max;
		}

	};
	void Reset(){alarm_count=0;
	alarm_index=0;};
	int GetAlarmAreaCount(){return alarm_count;};
	void SetOffsetData(int channel,int offset[2]){
		alarm_offset[channel][0]=offset[0];
		alarm_offset[channel][1]=offset[1];
	};
	int * GetOffsetData(int channel){return alarm_offset[channel];};
	void TranslatetoScreenCoord(int channel,const Point * src,Point * dst);
	//get object windows from mvdetector,and translate patch coords to screen coords

	void TranslatetoPatchCoord(int channel,const Point * src,Point * dst);
	//get object window from UART,translate screen coords to alarm patch coords

	int GetAlarmIndex(){return alarm_index;};

	void SetAlarmType(int type){alarm_type=type;};
	int GetAlarmType(){return alarm_type;};

private:
	void SetAlarmAreaPos(int channel,int * pos){
		int count=0,max_channel=0;
		if(alarm_type==TYPE_ALARM_AREA)
		{
			count=8;
			max_channel=ALARM_MAX_COUNT;
		}
		else
		{
			count=8;
			max_channel=ALARM_LINE_MAX_COUNT;
		}
		for(int i=0;i<count;i++)
		{
			alarm_pos[channel%max_channel][i]=pos[i];
		}
	};
	static void CalculatePatch(int alarm_obj[8],int patch[4],int size=8,int expansion=5);//calc outline of alarm area
	int alarm_pos[ALARM_MAX_COUNT+ALARM_LINE_MAX_COUNT][8];//x1,y1~
	int alarm_index;
	int alarm_count;//0~4
	int alarm_type;
	int alarm_offset[ALARM_MAX_COUNT+ALARM_LINE_MAX_COUNT][2];//x,y
	int alarm_patch[ALARM_MAX_COUNT+ALARM_LINE_MAX_COUNT][8];//x1,y1~
	int alarm_result[ALARM_MAX_COUNT+ALARM_LINE_MAX_COUNT][8];
};

#endif /* DATAOFALARMAREA_H_ */
