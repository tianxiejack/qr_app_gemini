/*
 * RSTCaptureGroup.h
 *
 *  Created on: Sep 2, 2019
 *      Author: ubuntu
 */
#include "CaptureGroup.h"
#ifndef RSTCAPTUREGROUP_H_
#define RSTCAPTUREGROUP_H_

class RTSCaptureGroup:public CaptureGroup
{
public:
	RTSCaptureGroup(unsigned int w,unsigned int h,int NCHAN,unsigned int capCount=1):
		CaptureGroup(w,h,NCHAN,capCount){};
	virtual ~RTSCaptureGroup(){};
	virtual void CreateProducers(){};
	virtual void OpenProducers(){};
	RTSCaptureGroup(){};
private:
};

class RTSPanoGroup:public RTSCaptureGroup
{
public:
	virtual vector<Consumer>  GetConsumers(int *queueid,int count);
	static RTSPanoGroup * GetInstance();
private:
	static RTSPanoGroup rtsCaptureGroup;
	RTSPanoGroup(unsigned int w,unsigned int h,int NCHAN,unsigned int capCount=1):
		RTSCaptureGroup(w,h,NCHAN,capCount){};
};



#endif /* RSTCAPTUREGROUP_H_ */
