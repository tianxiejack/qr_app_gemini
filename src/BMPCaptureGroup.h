#ifndef _BMP_CAP_GROUP_H_
#define _BMP_CAP_GROUP_H_
#include "CaptureGroup.h"

class BMPCaptureGroup:public CaptureGroup
{
public:
	BMPCaptureGroup(unsigned int w,unsigned int h,int NCHAN,unsigned int capCount=1):
		CaptureGroup(w,h,NCHAN,capCount){};
	virtual ~BMPCaptureGroup(){};
	virtual void CreateProducers(){};
	virtual void OpenProducers(){};
	BMPCaptureGroup(){};
private:
};

class BMPPanoGroup:public BMPCaptureGroup
{
public:
	virtual vector<Consumer>  GetConsumers(int *queueid,int count);
	static BMPPanoGroup * GetInstance();
private:
	static BMPPanoGroup bmpCaptureGroup;
	BMPPanoGroup(unsigned int w,unsigned int h,int NCHAN,unsigned int capCount=1):
		BMPCaptureGroup(w,h,NCHAN,capCount){};
};

class BMPMiscGroup:public BMPCaptureGroup
{
public:
	virtual vector<Consumer>  GetConsumers(int *queueid,int count);
	static BMPMiscGroup * GetInstance();
private:
	static BMPMiscGroup miscCaptureGroup;
	BMPMiscGroup(unsigned int w,unsigned int h,int NCHAN,unsigned int capCount=1):
		BMPCaptureGroup(w,h,NCHAN,capCount){};
};


#endif
