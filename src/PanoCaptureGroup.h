#ifndef _PANO_CAP_GROUP_H_
#define  _PANO_CAP_GROUP_H_
#include "CaptureGroup.h"

class PanoCaptureGroup:public HDCaptureGroup
{
public:
	static PanoCaptureGroup * GetMainInstance();
	static PanoCaptureGroup * GetSubInstance();
	~PanoCaptureGroup();
	virtual void CreateProducers();
	virtual void OpenProducers();
	unsigned char * GetSrc(int idx);
	//bool saveExposureCompensationCapImg();
private:
	PanoCaptureGroup(unsigned int w,unsigned int h,int NCHAN,unsigned int capCount=1);
	PanoCaptureGroup(){};
	PanoCaptureGroup(const PanoCaptureGroup&){};
	static PanoCaptureGroup MainPanoGroup;
	static PanoCaptureGroup SubPanoGroup;
};


#endif
