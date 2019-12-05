#include "AlarmTarget.h"

AlarmTarget::AlarmTarget()
{
	for(int i=0;i<ALARM_TARGET_CHANNEL_MAX;i++)
	{
		TargetCount[i]=0;
		TargetType[i]=0;
	}
}
