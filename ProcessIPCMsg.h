#ifndef PROCESS_UPCMSG_H
#define PROCESS_UPCMSG_H

#include "StlGlDefines.h"

typedef class Process_Message
{
public:
virtual unsigned char GetdispalyMode()=0;
virtual void save0pos()=0;
virtual void setXspeedandMove()=0;
virtual void setYspeedandMove()=0;
virtual self_check_state CheckFine()=0;
}*p_Process_Message;


class Process_Zodiac_Message:public Process_Message
{
public:
	Process_Zodiac_Message();
	~Process_Zodiac_Message();
	unsigned char GetdispalyMode();
 void save0pos();
  void setXspeedandMove();
  void setYspeedandMove();
  self_check_state CheckFine();
  void setPanoHeight_Length(float height,float length){
	  PanoHeight=height;
	  PanoLen=length;
  };
  bool IsToChangeTelMode(){return istochangeTelMode;};
  telBreak GetTelBreak(){return telbreak;};
  void DisableChangeTelMode(){istochangeTelMode=false;};
private:
  bool istochangeTelMode;
  telBreak telbreak;
  float PanoHeight;
  float PanoLen;
};
#endif
