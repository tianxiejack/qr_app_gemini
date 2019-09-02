#include "RSTCaptureGroup.h"
#include"Camera.h"
RTSPanoGroup RTSPanoGroup::rtsCaptureGroup(RTS_WIDTH,RTS_HEIGHT,3,CAM_COUNT);

vector<Consumer>  RTSPanoGroup::GetConsumers(int *queueid,int count)
{
	 Consumer cons;
	 for(int i=0;i<count;i++)
	 {
		   cons.pcap = new RTSVcap(i,RTS_WIDTH,RTS_HEIGHT);
		   cons.idx = i;
		   v_cons.push_back(cons);
	 }
	   return v_cons;
}

RTSPanoGroup * RTSPanoGroup::GetInstance()
{
			static bool once =true;
			if(once){
				rtsCaptureGroup.init(NULL,3);
				once =false;
			}
			return &rtsCaptureGroup;
}
