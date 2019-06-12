#include"BMPCaptureGroup.h"
BMPMiscGroup BMPMiscGroup::miscCaptureGroup(720,576,3,CAM_COUNT);

vector<Consumer>  BMPMiscGroup::GetConsumers(int *queueid,int count)
{
	 Consumer cons;
	 char *str[]={"45","90","180" ,"pic3","pic2"
	 };
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

BMPMiscGroup * BMPMiscGroup::GetInstance()
{
	static bool once =true;
	if(once){
#if USE_BMPCAP&&FORCE_1920
		miscCaptureGroup.init(NULL,ICON_COUNT);
#else
		miscCaptureGroup.init(NULL,ICON_COUNT-2);
#endif
		once =false;
	}
	return &miscCaptureGroup;

}


