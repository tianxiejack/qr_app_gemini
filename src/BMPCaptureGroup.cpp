#include"BMPCaptureGroup.h"
#include"Camera.h"
BMPPanoGroup BMPPanoGroup::bmpCaptureGroup(SDI_WIDTH,SDI_HEIGHT,3,CAM_COUNT);

vector<Consumer>  BMPPanoGroup::GetConsumers(int *queueid,int count)
{
	 Consumer cons;
	 char *str[]={"pic0","pic1","pic2"};
	 char filename[64];
	 for(int i=0;i<count;i++)
	 {
		 sprintf(filename,"./data/%s.bmp",str[i]);
		   cons.pcap = new BMPVcap(filename);
		   cons.idx = i;
		   v_cons.push_back(cons);
	 }
	   return v_cons;
}

BMPPanoGroup * BMPPanoGroup::GetInstance()
{
			static bool once =true;
			if(once){
				bmpCaptureGroup.init(NULL,3);
				once =false;
			}
			return &bmpCaptureGroup;
}


