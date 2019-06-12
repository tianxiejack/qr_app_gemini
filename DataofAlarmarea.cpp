#include "DataofAlarmarea.h"

int AlarmGetMaxData(int * data,int count)
{
	int max=data[0];
	int i=0;
	for(i=0;i<count;i++)
	{
		if(max<data[i])
		{
			max=data[i];
		}
	}
	return max;
}

int AlarmGetMinData(int * data,int count)
{
	int min=data[0];
	int i=0;
	for(i=0;i<count;i++)
	{
		if(min>data[i])
		{
			min=data[i];
		}
	}
	return min;
}

void AlarmGetoutline(int * src,int * dst,int count)
{
	int x[count/2],y[count/2];
	int i=0;
	for(i=0;i<count/2;i++)
	{
		x[i]=src[2*i];
		y[i]=src[2*i+1];
	}
	dst[0]=AlarmGetMinData(x,count/2);
	dst[1]=AlarmGetMinData(y,count/2);
	dst[2]=AlarmGetMaxData(x,count/2);
	dst[3]=AlarmGetMaxData(y,count/2);

}

DataofAlarmarea::DataofAlarmarea():alarm_area_count(0),alarm_area_index(0)
{
		for(int i=0;i<4;i++)
		{
			for(int j=0;j<8;j++)
			{
				alarm_area_pos[i][j]=100;
			}
		}
}
BaseAlarmObject::BaseAlarmObject(int type):alarm_count(0),alarm_index(0)
{
		for(int i=0;i<(ALARM_MAX_COUNT+ALARM_LINE_MAX_COUNT);i++)
		{
			for(int j=0;j<8;j++)
			{
				alarm_pos[i][j]=100;
			}
		}
		alarm_type=type;
}

void BaseAlarmObject::TranslatetoScreenCoord(int channel,const Point * src,Point * dst)
	{
		int i=0;
		int count=0;
		if(alarm_type==TYPE_ALARM_AREA)
		{
			count=4;
		}
		else
		{
			count=2;
		}
		for(i=0;i<count;i++)
		{
			dst[i].x=src[i].x+alarm_offset[channel][0];
			dst[i].y=src[i].y+alarm_offset[channel][1];
		}
	};

void BaseAlarmObject::TranslatetoPatchCoord(int channel,const Point * src,Point * dst)
{
	int i=0;
	int count=0;
	if(alarm_type==TYPE_ALARM_AREA)
	{
		count=4;
	}
	else
	{
		count=2;
	}
	for(i=0;i<count;i++)
	{
		dst[i].x=src[i].x-alarm_offset[channel][0];
		dst[i].y=src[i].y-alarm_offset[channel][1];
	}
}

void BaseAlarmObject::CalculatePatch( int alarm_obj[8],int patch[4],int size,int expansion)
{
	int outline[4];
	AlarmGetoutline(alarm_obj,outline,size);
	patch[0]=outline[0]-expansion;
	patch[1]=outline[1]-expansion;
	patch[2]=outline[2]-expansion;
	patch[3]=outline[3]-expansion;
}


