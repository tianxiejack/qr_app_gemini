/*
 * v4l2camera.cpp
 *
 *  Created on: Jan 14, 2017
 *      Author: wang
 */

#include "v4l2camera.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <getopt.h>             /* getopt_long() */
#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include<opencv2/opencv.hpp>
#include "StlGlDefines.h"
#include "thread.h"
#include <osa_buf.h>
#include "buffer.h"
#if TRACK_MODE
#include "VideoProcessTrack.hpp"
#endif
#ifdef CAPTURE_SCREEN
#include "cabinCapture.h"
#endif
#define MEMCPY memcpy
using namespace std;
static int once_buffer;
int bufId[16]={0};
extern void DeinterlaceYUV_Neon(unsigned char *lpYUVFrame, int ImgWidth, int ImgHeight, int ImgStride);
#ifdef CAPTURE_SCREEN
extern RecordHandle * cabin_handle;
#endif
v4l2_camera::v4l2_camera(int devId,int childSumCam):io(IO_METHOD_MMAP),imgwidth(FRAME_WIDTH/2),imgheight(FRAME_HEIGHT),
imgformat(V4L2_PIX_FMT_UYVY),buffers(NULL),split_buffer(NULL),force_format(1),pTmpBuf(NULL),
fd(-1),n_buffers(0),Id(devId),CorrespondCamNum(childSumCam)
{
	sprintf(dev_name, "/dev/video%d",devId);
	bzero(split_buffer_ch,sizeof(split_buffer_ch));
	pTmpBuf  = (unsigned char*)malloc(720*576*2);
	for(int i = 0; i < CorrespondCamNum; i++){
		dstbuffer[i]= new char[(DEFAULT_IMAGE_WIDTH*DEFAULT_IMAGE_HEIGHT*3 )];
	    rgbImg[i]= Mat(DEFAULT_IMAGE_HEIGHT,DEFAULT_IMAGE_WIDTH,CV_8UC3,dstbuffer[i]);
	}
	if(once_buffer == 0)
	{
		once_buffer = 1;
		init_buffer();
	}
}

v4l2_camera::~v4l2_camera()
{
	stop_capturing();
	uninit_device();
	close_device();
	for(int i=0; i<4; i++)
		delete dstbuffer[i];
	if(once_buffer == 1)
	{
		once_buffer = 0;
		destroy_buffer();
	}
	free(pTmpBuf);
}

void v4l2_camera::yuv2bgr(unsigned char *pYuvBuf,int width,int height,int chId)
{
}

bool v4l2_camera::getEmpty(unsigned char** pYuvBuf, int chId)
{
	int status=0;
	bool ret = true;
	chId = Id*4+chId;

	while(1)
	{
		status = OSA_bufGetEmpty(&queue_dis->bufHndl[chId],&bufId[chId],0);
		if(status == 0)
		{
			*pYuvBuf = (unsigned char*)queue_dis->bufHndl[chId].bufInfo[bufId[chId]].virtAddr;
			break;
		}else{
			if(!OSA_bufGetFull(&queue_dis->bufHndl[chId],&bufId[chId],OSA_TIMEOUT_FOREVER))
			{
				if(!OSA_bufPutEmpty(&queue_dis->bufHndl[chId],bufId[chId]))
				{
					;
				}
			}
		}
	}
	 return ret;
}

bool v4l2_camera::Data2Queue(unsigned char *pYuvBuf,int width,int height,int chId)
{
	int status;

	chId = Id*4+chId;
	if(chId>=(CAM_COUNT+EXT_CAM_COUNT))//if(chId >= CAM_COUNT+1)
		return false;
	queue_dis->bufHndl[chId].bufInfo[bufId[chId]].width=width;
	queue_dis->bufHndl[chId].bufInfo[bufId[chId]].height=height;
	queue_dis->bufHndl[chId].bufInfo[bufId[chId]].strid=width;

	OSA_bufPutFull(&queue_dis->bufHndl[chId],bufId[chId]);
	return true;
}

void v4l2_camera::errno_exit(const char *s)
{
	fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));
	exit(EXIT_FAILURE);
}

int v4l2_camera::xioctl(int fh, int request, void *arg)
{
	int ret;

	do {
		ret = ioctl(fh, request, arg);
	} while (-1 == ret && EINTR == errno);

	return ret;
}

int v4l2_camera::alloc_split_buffer()
{
	int alloc_size = 720*578*2;

	split_buffer = (unsigned char     *)malloc(1920*1080*2); /* No G2D_CACHABLE */
	if(!split_buffer) {
		printf("Fail to allocate physical memory for image buffer!\n");
		return FAILURE_ALLOCBUFFER;
	}
	for (int i = 0; i < CorrespondCamNum; i++) {
		getEmpty(&split_buffer_ch[i], i);
//		split_buffer_ch[i] = (unsigned char *) malloc(alloc_size);
//		printf("split_buffer_ch[%d]:0x%p\n",i,split_buffer_ch[i]);
		if (!split_buffer_ch[i]) {
			printf("fail g2d_alloc ch%d!\n",i);
			return FAILURE_ALLOCBUFFER;
		}
	}
	return EXIT_SUCCESS;
}

void v4l2_camera::parse_line_header2(int channels, unsigned char *p)
{
	#if TRACK_MODE
	   static int frame_count =0;
	#endif
	
	int line, lines;
	int max_line_per_ch = IMAGE_HEIGHT/2 + 1;
	lines = channels * max_line_per_ch;//total num;

//#pragma omp parallel for
	for(line=0; line<lines; line++)
	{
		uchar* p_src = p + line*(sizeof(LineHeader) + IMAGE_WIDTH + 24);
		int lineId,chId,fieldId;//frameId,
		LineHeader *phead=(LineHeader*)p_src;

		if((p_src[0] & p_src[1] & p_src[2] & p_src[3] & p_src[4] & p_src[5] & p_src[6] & p_src[7] & 0xF0) != 0xA0)
			continue;

		fieldId = phead->fieldNo & 0x01;
		lineId  = ((phead->LineNoH&0x0F) << 8)
				| ((phead->LineNoM&0x0F) << 4)
				| (phead->LineNoL & 0x0F);
		chId    = phead->ChId & 0x03;

		if ((chId>3) || (lineId>max_line_per_ch) || (lineId<=0)||((Id*4+chId)>(CAM_COUNT+EXT_CAM_COUNT)))
			continue;

		uchar* pDst =  split_buffer_ch[chId] + ((lineId-1)*2 + fieldId)*IMAGE_WIDTH;
		memcpy(pDst, p_src+sizeof(LineHeader), IMAGE_WIDTH);

		if ((lineId == IMAGE_HEIGHT/2) )//&& (fieldId==0)
		{
			unsigned char * lastBuf;
			DeinterlaceYUV_Neon(split_buffer_ch[chId], IMAGE_WIDTH/2, IMAGE_HEIGHT, IMAGE_WIDTH/2);
			lastBuf = split_buffer_ch[chId];
			#if TRACK_MODE
			if((Id*4+chId)==12)
			{
				if(frame_count++>=100)
				{
					CVideoProcess* trackMode=CVideoProcess::getInstance();
					Mat frame(576,720,CV_8UC2,lastBuf);
					trackMode->process_frame(pal_Track,frame);
				}
			}
			if((Id*4+chId)==13)
			{
				if(frame_count++>=100)
				{
					CVideoProcess* trackMode=CVideoProcess::getInstance();
					Mat frame(576,720,CV_8UC2,lastBuf);
					trackMode->process_frame(pal2_Track,frame);
				}
			}
			#endif
			if(Data2Queue(split_buffer_ch[chId],DEFAULT_IMAGE_WIDTH,DEFAULT_IMAGE_HEIGHT,chId)){
				if(getEmpty(&split_buffer_ch[chId], chId)){
				    memcpy(split_buffer_ch[chId], lastBuf, 2*DEFAULT_IMAGE_WIDTH*DEFAULT_IMAGE_HEIGHT);// ready for next field
				}
			}
		}
	}
}
void v4l2_camera::process_image(const void *p, int size)
{
	//   frame_number++;
	parse_line_header2(4,(unsigned char *)p);
}

int v4l2_camera::read_frame(void)
{
	struct v4l2_buffer buf;

	switch (io) {
//	case IO_METHOD_READ:
//		if (-1 == read(fd, buffers[0].start, buffers[0].length)) {
//			switch (errno) {
//			case EAGAIN:
//				return 0;
//			case EIO:
//				/* Could ignore EIO, see spec. */
//				/* fall through */
//			default:
//				errno_exit("read");
//			}
//		}
//		process_image(buffers[0].start, buffers[0].length);
//		break;
	case IO_METHOD_MMAP:
		CLEAR(buf);
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;

		if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
			switch (errno) {
			case EAGAIN:
				return -1;
			case EIO:
				/* Could ignore EIO, see spec. */
				/* fall through */
			default:
				errno_exit("VIDIOC_DQBUF");
			}
		}

		assert(buf.index < n_buffers);
		process_image(buffers[buf.index].start, buf.bytesused);

		if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
			errno_exit("VIDIOC_QBUF");
		break;
//	case IO_METHOD_USERPTR:
//		CLEAR(buf);
//
//		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//		buf.memory = V4L2_MEMORY_USERPTR;
//
//		if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
//			switch (errno) {
//			case EAGAIN:
//				return 0;
//			case EIO:
//				/* Could ignore EIO, see spec. */
//				/* fall through */
//			default:
//				errno_exit("VIDIOC_DQBUF");
//			}
//		}
//
//		for (int i=0; i<n_buffers; ++i)
//			if (buf.m.userptr == (unsigned long)buffers[i].start
//					&& buf.length == buffers[i].length)
//				break;
//
//		assert(i < n_buffers);
//
//		process_image((void *)buf.m.userptr, buf.bytesused);
//
//		if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
//			errno_exit("VIDIOC_QBUF");
//		break;
	default:
		break;
	}

	return 0;
}

void v4l2_camera::mainloop(void)
{
	fd_set fds;
	struct timeval tv;
	int ret;

		FD_ZERO(&fds);
		FD_SET(fd, &fds);

		/* Timeout. */
		tv.tv_sec = 2;
		tv.tv_usec = 0;

		ret = select(fd + 1, &fds, NULL, NULL, &tv);

		if (-1 == ret)
		{
			if (EINTR == errno)
				return;

			errno_exit("select");
		}else if (0 == ret)
		{
			fprintf(stderr, "select timeout\n");
			exit(EXIT_FAILURE);
		}

		if (-1 == read_frame())  /* EAGAIN - continue select loop. */
			return;

}

void v4l2_camera::stop_capturing(void)
{
	enum v4l2_buf_type type;
	switch (io) {
//	case IO_METHOD_READ:
//		/* Nothing to do. */
//		break;
	case IO_METHOD_MMAP:
//	case IO_METHOD_USERPTR:
		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type))
			errno_exit("VIDIOC_STREAMOFF");
		break;
	}
}

void v4l2_camera::start_capturing(void)
{
	unsigned int i;
	enum v4l2_buf_type type;

	switch (io) {
//	case IO_METHOD_READ:
//		/* Nothing to do. */
//		break;
	case IO_METHOD_MMAP:
		for (i = 0; i < n_buffers; ++i) {
			struct v4l2_buffer buf;

			CLEAR(buf);
			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_MMAP;
			buf.index = i;

			if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
				errno_exit("VIDIOC_QBUF");
		}
		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
			errno_exit("VIDIOC_STREAMON");
		break;
//	case IO_METHOD_USERPTR:
//		for (i = 0; i < n_buffers; ++i) {
//			struct v4l2_buffer buf;
//
//			CLEAR(buf);
//			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//			buf.memory = V4L2_MEMORY_USERPTR;
//			buf.index = i;
//			buf.m.userptr = (unsigned long)buffers[i].start;
//			buf.length = buffers[i].length;
//
//			if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
//				errno_exit("VIDIOC_QBUF");
//		}
//		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//		if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
//			errno_exit("VIDIOC_STREAMON");
//		break;
	default:
		break;
	}
}

void v4l2_camera::uninit_device(void)
{
	unsigned int i;

	switch (io) {
	case IO_METHOD_READ:
		free(buffers[0].start);
		break;
	case IO_METHOD_MMAP:
		for (i = 0; i < n_buffers; ++i)
			if (-1 == munmap(buffers[i].start, buffers[i].length))
				errno_exit("munmap");
		break;
	case IO_METHOD_USERPTR:
		for (i = 0; i < n_buffers; ++i)
			free(buffers[i].start);
		break;
	default:
		break;
	}

	free(buffers);
}

void v4l2_camera::init_read(unsigned int buffer_size)
{
	buffers = (struct buffer          *)calloc(1, sizeof(*buffers));

	if (!buffers) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}

	buffers[0].length = buffer_size;
	buffers[0].start = (struct buffer          *) malloc(buffer_size);

	if (!buffers[0].start) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}
}

void v4l2_camera::init_mmap(void)
{
	struct v4l2_requestbuffers req;

	CLEAR(req);

	req.count = MAX_CHAN;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;

	if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
		if (EINVAL == errno) {
			fprintf(stderr, "%s does not support memory mapping\n", dev_name);
			exit(EXIT_FAILURE);
		} else {
			errno_exit("VIDIOC_REQBUFS");
		}
	}

	if (req.count < 2) {
		fprintf(stderr, "Insufficient buffer memory on %s\n", dev_name);
		exit(EXIT_FAILURE);
	}
	printf("*************%s qbuf cnt = %d\n", dev_name, req.count);
	buffers = (struct buffer          *)calloc(req.count, sizeof(*buffers));

	if (!buffers) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}

	for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
		struct v4l2_buffer buf;

		CLEAR(buf);

		buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory      = V4L2_MEMORY_MMAP;
		buf.index       = n_buffers;

		if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
			errno_exit("VIDIOC_QUERYBUF");

		buffers[n_buffers].length = buf.length;
		buffers[n_buffers].start  =	mmap(NULL /* start anywhere */,
										buf.length,
										PROT_READ | PROT_WRITE /* required */,
										MAP_SHARED /* recommended */,
										fd, buf.m.offset);
		memset(buffers[n_buffers].start,0x80,buf.length);

		if (MAP_FAILED == buffers[n_buffers].start)
			errno_exit("mmap");
	}
}

void v4l2_camera::init_userp(unsigned int buffer_size)
{
	struct v4l2_requestbuffers req;

	CLEAR(req);

	req.count  = MAX_CHAN;
	req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_USERPTR;

	if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
		if (EINVAL == errno) {
			fprintf(stderr, "%s does not support "
					"user pointer i/o\n", dev_name);
			exit(EXIT_FAILURE);
		} else {
			errno_exit("VIDIOC_REQBUFS");
		}
	}

	buffers = (struct buffer          *)calloc(4, sizeof(*buffers));

	if (!buffers) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}

	for (n_buffers = 0; n_buffers < MAX_CHAN; ++n_buffers) {
		buffers[n_buffers].length = buffer_size;
		buffers[n_buffers].start = malloc(buffer_size);

		if (!buffers[n_buffers].start) {
			fprintf(stderr, "Out of memory\n");
			exit(EXIT_FAILURE);
		}
	}
}



int v4l2_camera::init_device(void)
{
	struct v4l2_capability cap;
	struct v4l2_cropcap cropcap;
	struct v4l2_crop crop;
	struct v4l2_format fmt;
	unsigned int min;

	if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
		if (EINVAL == errno) {
			fprintf(stderr, "%s is no V4L2 device\n", dev_name);
			return FAILURE_DEVICEINIT;
		} else {
			errno_exit("VIDIOC_QUERYCAP");
		}
	}

	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		fprintf(stderr, "%s is no video capture device\n", dev_name);
		return FAILURE_DEVICEINIT;
	}

	switch (io) {
	case IO_METHOD_READ:
		if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
			fprintf(stderr, "%s does not support read i/o\n", dev_name);
			return FAILURE_DEVICEINIT;
		}
		break;
	case IO_METHOD_MMAP:
	case IO_METHOD_USERPTR:
		if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
			fprintf(stderr, "%s does not support streaming i/o\n", dev_name);
			return FAILURE_DEVICEINIT;
		}
		break;
	}

	/* Select video input, video standard and tune here. */

	CLEAR(cropcap);

	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (0 == xioctl(fd, VIDIOC_CROPCAP, &cropcap)) {
		crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		crop.c = cropcap.defrect; /* reset to default */

		if (-1 == xioctl(fd, VIDIOC_S_CROP, &crop)) {
		}
	} else {
		/* Errors ignored. */
	}

	CLEAR(fmt);

	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (force_format) {
		fprintf(stderr, "Set uyvy\r\n");
		fmt.fmt.pix.width       = imgwidth; //replace
		fmt.fmt.pix.height      = imgheight; //replace
		//  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_H264; //replace
		fmt.fmt.pix.pixelformat =imgformat;// V4L2_PIX_FMT_UYVY;
		fmt.fmt.pix.field       = V4L2_FIELD_ANY;
		//fmt.fmt.pix.code=0;
		printf("***************************width =%d height=%d\n",fmt.fmt.pix.width,fmt.fmt.pix.height);

		if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
		{
			 errno_exit("VIDIOC_S_FMT");
		}

		/* Note VIDIOC_S_FMT may change width and height. */
	} else {
		/* Preserve original settings as set by v4l2-ctl for example */
		if (-1 == xioctl(fd, VIDIOC_G_FMT, &fmt))
			errno_exit("VIDIOC_G_FMT");
	}

	/* Buggy driver paranoia. */
	min = fmt.fmt.pix.width * 2;
	if (fmt.fmt.pix.bytesperline < min)
		fmt.fmt.pix.bytesperline = min;
	min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
	if (fmt.fmt.pix.sizeimage < min)
		fmt.fmt.pix.sizeimage = min;

	switch (io) {
	case IO_METHOD_READ:
		init_read(fmt.fmt.pix.sizeimage);
		break;
	case IO_METHOD_MMAP:
		init_mmap();
		break;
	case IO_METHOD_USERPTR:
		init_userp(fmt.fmt.pix.sizeimage);
		break;
	}
	return EXIT_SUCCESS;
}

void v4l2_camera::close_device(void)
{
	if (-1 == close(fd))
		errno_exit("close");

	fd = -1;
}

int v4l2_camera::open_device(void)
{
	struct stat st;

	if (-1 == stat(dev_name, &st)) {
		fprintf(stderr, "Cannot identify '%s': %d, %s\n", dev_name, errno, strerror(errno));
		return FAILURE_DEVICEOPEN;
	}

	if (!S_ISCHR(st.st_mode)) {
		fprintf(stderr, "%s is no device\n", dev_name);
		return FAILURE_DEVICEOPEN;
	}

	fd = open(dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);
	if (-1 == fd) {
		fprintf(stderr, "Cannot open '%s': %d, %s\n", dev_name, errno, strerror(errno));
		return FAILURE_DEVICEOPEN;
	}
	return EXIT_SUCCESS;
}

bool v4l2_camera::Open()
{
	int ret;

	//if(CorrespondCamNum!=2)
	{
	ret = alloc_split_buffer();
	if(ret < 0)
		return false;
	}

	ret = open_device();
	if(ret < 0)
		return false;

	ret = init_device();
	if(ret < 0)
		return false;

	start_capturing();
	return true;
}


//--------------the decorator cap class------------
AsyncVCap4::AsyncVCap4(auto_ptr<BaseVCap> coreCap):
	m_core(coreCap),thread_state(THREAD_READY),sleepMs(DEFAULT_THREAD_SLEEP_MS)
{
	initLock();
	start_thread(capThread,this);
	cout<<" start a capture thread"<<endl;
}
AsyncVCap4:: ~AsyncVCap4()
{
	Close();
}
bool AsyncVCap4::Open()
{
	if(thread_state != THREAD_READY)
		return false;//do not support reopen.
	bool isOpened = m_core->Open();
	if(isOpened){
		Start();
	}
	return isOpened;
}

void AsyncVCap4::Close()
{
	thread_state = THREAD_STOPPING;
	while( THREAD_IDLE != thread_state){
		usleep(100*1000);
	}
	m_core->Close();
}
void AsyncVCap4::Capture(char* ptr)
{
	lock_read(ptr);
}

void AsyncVCap4::SetDefaultImg(char *p)
{
	m_core->SetDefaultImg(p);
}

 void AsyncVCap4::SavePic(const char* name)
 {
	m_core->SavePic(name);
 }

void AsyncVCap4::Run()
{

	do{
		usleep(100*1000);
	}while(THREAD_READY == thread_state);
	//cap in background thread
	while(thread_state == THREAD_RUNNING)
	{
		v4l2_camera * pcore = dynamic_cast<v4l2_camera*>(m_core.get());
		if(pcore){
			pcore->mainloop();
		}
//		usleep(sleepMs*1000);
	}
	thread_state = THREAD_IDLE;
	destroyLock();

}

void* AsyncVCap4::capThread(void*p)
{
	AsyncVCap4 *thread = (AsyncVCap4*)p;
	thread->Run();
	return NULL;
}

void AsyncVCap4::lock_read(char *ptr)
{
	char *pImg = m_core->GetDefaultImg();
	pthread_rwlock_rdlock(&rwlock);
	MEMCPY(ptr, pImg,m_core->GetTotalBytes());
	pthread_rwlock_unlock(&rwlock);
}

void AsyncVCap4::lock_write(char *ptr)
{
	char *pImg = m_core->GetDefaultImg();
	pthread_rwlock_wrlock(&rwlock);
	MEMCPY(pImg,ptr,m_core->GetTotalBytes());
	pthread_rwlock_unlock(&rwlock);
}
void  AsyncVCap4::Start()
{
	thread_state = THREAD_RUNNING;
}
void AsyncVCap4::destroyLock()
{
	pthread_rwlock_destroy(&rwlock);
	pthread_rwlockattr_destroy(&rwlockattr);
}

void AsyncVCap4::initLock()
{
	pthread_rwlockattr_init(&rwlockattr);
	pthread_rwlockattr_setpshared(&rwlockattr,2);
	pthread_rwlock_init(&rwlock,&rwlockattr);
}
