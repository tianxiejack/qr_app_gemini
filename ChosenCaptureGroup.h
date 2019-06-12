#ifndef _CHOSEN_CAP_GROUP_H
#define _CHOSEN_CAP_GROUP_H
#include "CaptureGroup.h"
class ChosenCaptureGroup:public HDCaptureGroup
{
public:
	static ChosenCaptureGroup * GetMainInstance();
	static ChosenCaptureGroup * GetSubInstance();
	static ChosenCaptureGroup * GetMvDetectInstance();
	static ChosenCaptureGroup * GetMvDetect_add_Instance();
		~ChosenCaptureGroup();
	virtual void CreateProducers();
	virtual void OpenProducers();
private:
	ChosenCaptureGroup(unsigned int w,unsigned int h,int NCHAN,unsigned int capCount=1);
	ChosenCaptureGroup(){};
	ChosenCaptureGroup(const ChosenCaptureGroup&){};
	static ChosenCaptureGroup MainChosenGroup;
	static ChosenCaptureGroup SubChosenGroup;
	static ChosenCaptureGroup MvDetectGroup;
	static ChosenCaptureGroup MvDetect_add_Group;
};
#endif


