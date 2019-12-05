/*
 * v4l2camera.h
 *
 *  Created on: Jan 14, 2017
 *      Author: wang
 */

#ifndef V4L2CAMERA_H_
#define V4L2CAMERA_H_

#include <sys/types.h>
//#include <opencv2/opencv.hpp>
#include "Camera.h"
#include "timing.h"

using namespace cv;
//#define SHOW_TIMES
#define CLEAR(x) memset(&(x), 0, sizeof(x))

#define MAX_CHAN     2
#define FRAME_WIDTH  1448
#define FRAME_HEIGHT 1156
#define IMAGE_WIDTH  1440
#define IMAGE_HEIGHT 576



#ifndef V4L2_PIX_FMT_H264
#define V4L2_PIX_FMT_H264     v4l2_fourcc('H', '2', '6', '4') /* H264 with start codes */
#endif

//#define EXIT_SUCCESS         0;
#define FAILURE_ALLOCBUFFER  -1;
#define FAILURE_DEVICEOPEN   -2;
#define FAILURE_DEVICEINIT   -3;

enum io_method {
	IO_METHOD_READ,
	IO_METHOD_MMAP,
	IO_METHOD_USERPTR,
};

struct buffer {
	void   *start;
	size_t  length;
};

//-------------decorator implementations-------------------------
class AsyncVCap4:public Interface_VCap{
	public:
	    AsyncVCap4(auto_ptr<BaseVCap> coreCap);
		virtual ~AsyncVCap4();
		virtual bool Open();
		virtual void Close();
		virtual void Capture(char* ptr);
		void CaptureFish(char* ptr){};
		virtual void SetDefaultImg( char *);
		virtual void SavePic(const char* name);
		void saveOverLap(){};
	private:
		enum{
			THREAD_READY,
			THREAD_RUNNING,
			THREAD_STOPPING,
			THREAD_IDLE
		} thread_state;
		AsyncVCap4(){};
		void destroyLock();
		void initLock();
		void lock_read(char *ptr);
		void lock_write(char *ptr);
		static void* capThread(void*);
		void Run();
		void Start();
		auto_ptr<BaseVCap> m_core;
		pthread_t tid;
		pthread_rwlock_t rwlock;
        pthread_rwlockattr_t rwlockattr;
		unsigned int sleepMs;
};

class v4l2_camera:public BaseVCap {
public:
	v4l2_camera(int devId,int childSumCam=4);
	virtual ~v4l2_camera();
	bool Open();  //alloc_split_buffer
	void Close(){};
	void Capture(char*p){};
	void CaptureFish(char*p){};
	void saveOverLap(){};
	void mainloop(void);  //xunhuan
	void Get4Ptrs(char* ptr[4]){ptr[0] = (char*)dstbuffer[0];
	                            ptr[1] = (char*)dstbuffer[1];
	                            ptr[2] = (char*)dstbuffer[2];
	                            ptr[3] = (char*)dstbuffer[3];};
private:
	int CorrespondCamNum;
	int  alloc_split_buffer();  //shou ji shuju
	int  open_device(void);
	void close_device(void);
	int  init_device(void);
	void uninit_device(void);  //free
	void init_userp(unsigned int buffer_size);
	void init_mmap(void);
	void init_read(unsigned int buffer_size);
	bool getEmpty(unsigned char** pYuvBuf, int chId);  //duilie

	void start_capturing(void);
	void stop_capturing(void);
	void Deinterlace(unsigned char *pTmpBuf, unsigned char *lpYFrame, int ImgWidth, int ImgHeight, int ImgStride);
	void DeinterlaceYUV(unsigned char *pTmpBuf, unsigned char *lpYUVFrame, int ImgWidth, int ImgHeight, int ImgStride);
	int  read_frame(void);


	void process_image(const void *p, int size);
	void parse_line_header(int header, int channels, unsigned char *p_src);
	void parse_line_header(int channels, unsigned char *p_src);
	void parse_line_header2(int channels, unsigned char *p_src);
	void yuv2bgr(unsigned char *pYuvBuf,int width,int height,int chid);
	bool Data2Queue(unsigned char *pYuvBuf,int width,int height,int chId);

	int  xioctl(int fh, int request, void *arg);
	void errno_exit(const char *s);

	char            dev_name[16];
	int fd;
	enum io_method  io;


	unsigned int     n_buffers;
	struct buffer   *buffers;
	unsigned char   *split_buffer;
	unsigned char   *split_buffer_ch[4];
	Mat rgbImg[4];
	char   *dstbuffer[4];
	unsigned char *pTmpBuf;
	int imgwidth;
	int imgheight;
	int imgformat;
	int force_format;
	int Id;
#ifdef SHOW_TIMES
	tick_t procInit, procFinal;
#endif
};

typedef unsigned char uchar;
typedef	struct Header_Line{
	uchar frameNo:4;   /* Byte 1 */
	uchar HD1:4;

	uchar fieldNo:1;   /* Byte 2 */
	char :3;
	uchar HD2:4;

	uchar LineNoH:4;   /* Byte 3 */
	uchar HD3:4;

	uchar LineNoM:4;   /* Byte 4 */
	uchar HD4:4;

	uchar LineNoL:4;   /* Byte 5 */
	uchar HD5:4;

	uchar ChId:2;      /* Byte 6 */
	char :2;
	uchar HD6:4;

	char VLock:1;    /* Byte 7 */
	char SLock:1;
	char HLock:1;
	char VDLoss:1;
	uchar HD7:4;

	char V_Stable:1; /* Byte 8 */
	char Derstus:1;
	char DET50:1;
	char MONO:1;
	uchar HD8:4;
}LineHeader;

#endif /* V4L2CAMERA_H_ */
