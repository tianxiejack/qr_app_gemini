#include "gst_capture.h"
#include "StlGlDefines.h"
#if GSTREAM_CAP
#define TARGET_IP  "127.0.0.1"
GstCapture_data gstCapture_data;
RecordHandle * record_handle;

void initGstCap()
{
	gstCapture_data.width = CURRENT_SCREEN_WIDTH;
	gstCapture_data.height = CURRENT_SCREEN_HEIGHT;
	gstCapture_data.framerate = 25;
	gstCapture_data.ip_port = 17000;
	gstCapture_data.filp_method = FLIP_METHOD_VERTICAL_FLIP;
	gstCapture_data.capture_src = APPSRC;
	gstCapture_data.format = "BGR";
	gstCapture_data.ip_addr =TARGET_IP;

	record_handle = gstCaptureInit(gstCapture_data);
}
void UninitGstCap()
{
    gstCaptureUninit(record_handle);
}
#endif
