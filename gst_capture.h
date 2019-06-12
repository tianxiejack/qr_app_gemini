#ifndef  _GST_CAPTURE_H
#define _GST_CAPTURE_H




typedef enum
{
	FLIP_METHOD_NONE,
	FLIP_METHOD_COUNTERCLOCKWISE,
	FLIP_METHOD_ROTATE_180,
	FLIP_METHOD_CLOCKWISE,
	FLIP_METHOD_HORIZONTAL_FLIP,
	FLIP_METHOD_UPPER_RIGHT_DIAGONAL,
	FLIP_METHOD_VERTICAL_FLIP,
	FLIP_METHOD_UPPER_LEFT_DIAGONAL,
	FLIP_METHOD_END
}FLIP_METHOD;

typedef enum
{
	XIMAGESRC,
	APPSRC
}CAPTURE_SRC;

typedef struct _recordHandle
{
	void* context;
	unsigned int width;
	unsigned int height;

	unsigned int framerate;
	char format[30];
	char ip_addr[30];
	unsigned int ip_port;
	unsigned short bEnable;
	FLIP_METHOD filp_method;
	CAPTURE_SRC capture_src;
}RecordHandle;

typedef struct _gstCapture_data
{
	int width;
	int height;
	int framerate;
	unsigned int ip_port;
	FLIP_METHOD filp_method;
	CAPTURE_SRC capture_src;
	char* format;
	char* ip_addr;
}GstCapture_data;




RecordHandle * gstCaptureInit( GstCapture_data gstCapture_data );

void initGstCap();
void UninitGstCap();
int gstCapturePushData(RecordHandle *handle, char *pbuffer , int datasize);

int gstCaptureUninit(RecordHandle *handle);

int gstCaptureEnable(RecordHandle *handle, unsigned short bEnable);





/*
#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif
*/


#endif
