#ifndef CHECKMYSELF_H
#define CHECKMYSELF_H


#include"StlGlDefines.h"
using namespace std;
/*
class CheckMyself
{
public:
	CheckMyself();
	~CheckMyself(){};
	bool CheckCommunication();
	void CheckVision(GLubyte *ptr, int index);
	bool CheckSolicePlate();
	bool VisionResult();
	bool getVisonResult();
	int getNeadToCheck(){return NeadToCheck;};
	void setNeadToCheck(){NeadToCheck++;};

	void setCommuCheck(){CommuCheck=true;};

private:
	bool c_Vision[CAM_COUNT];
	bool visionResult;
	int NeadToCheck;
	bool CommuCheck;
};
*/
class SelfCheck
{
public:
	SelfCheck();
	~SelfCheck(){};
	void CheckBrokenCam();
	self_check_state IsIDLE();
	int *GetBrokenCam()
	{
		return BrokenCam;
	};

	void CheckCaptureState(GLubyte *ptr,int index,int imgWidth,int ingHeight);
	void CaptureCheckAll();
	void initState();
	void Check12CAM();
	void CheckExtra2CAM();
	void Check2HD();

	void SendBrokenCAM();
	 int *getBrokenCam(){return BrokenCam;};
		void SetCheckState(self_check_item now_item,self_check_state now_state);
		self_check_state getCheckState(self_check_item now_item);
		void CalculateTime(time_t sec);
		bool IsOnesec();

		void TimeToLoop();
		bool IsCheck();
		bool IsAllReady();
private:
		self_check_state m_CheckResult[SELFCHECK_ITEM_COUNT];
		self_check_state capture_state[CAM_COUNT];
		time_t nowtime;
		time_t lasttime;
		bool Onesec;
		time_t NowCheckTime;
		time_t LastCheckTime;
		bool Ischeck;
		int BrokenCam[CAM_COUNT];
};
void SendBackBrokenCam(int *CamN);


#endif

