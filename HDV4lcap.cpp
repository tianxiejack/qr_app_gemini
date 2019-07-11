#include "HDV4lcap.h"
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
#include"StlGlDefines.h"
#include <osa_sem.h>
#if ARM_NEON_ENABLE
#include"yuv2rgb.h"
#endif
#if TRACK_MODE
#include "VideoProcessTrack.hpp"
#endif
#include <malloc.h>
#include <omp.h>
#include"Thread_Priority.h"
#include"mvdetectInterface.h"
#include "thread_idle.h"
#include "CenterPoint_caculate.h"
#include"Render_Agent.h"
#include "CheckMyself.h"
extern thread_idle tIdle;
extern Alg_Obj * queue_main_sub;
#define MEMCPY memcpy

#define INPUT_IMAGE_WIDTH 1920
#define INPUT_IMAGE_HEIGHT 1080
bool IsMvDetect=false;
using namespace std;
static bool Once_buffer=true;
int m_bufId[QUE_CHID_COUNT]={0};
extern bool saveSinglePic[CAM_COUNT];
extern void DeinterlaceYUV_Neon(unsigned char *lpYUVFrame, int ImgWidth, int ImgHeight, int ImgStride);
//Mat SDI_frame,VGA_frame;
//unsigned char * sdi_data_main[6];
//unsigned char * sdi_data_sub[6];
unsigned char * target_data[CAM_COUNT];
//static HDv4l_cam hdv4lcap(0,SDI_WIDTH,SDI_HEIGHT);

extern SelfCheck selfcheck;


unsigned char * sigle_yuyv[3] = {NULL,NULL,NULL};
unsigned char * select_bgr_data_sub=NULL;
unsigned char * select_bgr_data_main=NULL;
unsigned char * FPGA6_bgr_data_sub=NULL;
unsigned char * FPGA6_bgr_data_main=NULL;
unsigned char * FPGA4_bgr_data_sub=NULL;
unsigned char * FPGA4_bgr_data_main=NULL;
unsigned char * MVDECT_data_main[CAM_COUNT];
unsigned char * select_bgr_data_main_touch=NULL;
unsigned char * GRAY_data_main[CAM_COUNT];

unsigned char * vga_data=NULL;
CMvDectInterface   *pMvIF=NULL;
#if MVDECT
 MvDetect mv_detect(pMvIF);
#endif
HDv4l_cam::HDv4l_cam(int devId,int width,int height):io(IO_METHOD_USERPTR),imgwidth(width),
imgheight(height),buffers(NULL),memType(MEMORY_NORMAL),cur_CHANnum(0),
force_format(1),m_devFd(-1),n_buffers(0),bRun(false),Id(devId),BaseVCap()
{
		imgformat 	= V4L2_PIX_FMT_YUYV;//当成YUYV来采
		sprintf(dev_name, "/dev/video%d",devId);
			imgstride 	= imgwidth*2;
			bufSize 	= imgwidth * imgheight * 2;
			imgtype     = CV_8UC2;
			memType = MEMORY_NORMAL;
			bufferCount = 8;
			if(Once_buffer)
			{
				init_buffer();
				Once_buffer=false;
			}
			Src=NULL;
}

HDv4l_cam::~HDv4l_cam()
{
	stop_capturing();
	uninit_device();
	close_device();
}
void HDv4l_cam::HD_YUYV2UYV(unsigned char *dst,unsigned char *src, int ImgWidth, int ImgHeight)
{
	for(int j =0;j<ImgHeight;j++)
	{
		for(int i=0;i<ImgWidth*2/4;i++)
		{
			*(dst+j*ImgWidth*3+i*6+0)=*(src+j*ImgWidth*2+i*4+1);
			*(dst+j*ImgWidth*3+i*6+1)=*(src+j*ImgWidth*2+i*4+0);
			*(dst+j*ImgWidth*3+i*6+2)=*(src+j*ImgWidth*2+i*4+3);

			*(dst+j*ImgWidth*3+i*6+3)=*(src+j*ImgWidth*2+i*4+1);
			*(dst+j*ImgWidth*3+i*6+4)=*(src+j*ImgWidth*2+i*4+2);
			*(dst+j*ImgWidth*3+i*6+5)=*(src+j*ImgWidth*2+i*4+3);

		}
	}
}

void HDv4l_cam::RectFromPixels(unsigned char *src)
{
	int len=SDI_WIDTH-FPGA_SCREEN_WIDTH;
	for(int i=0;i<FPGA_SCREEN_HEIGHT;i++)
	{
		for(int j=(SDI_WIDTH*2*i);j<(SDI_WIDTH*2*i+FPGA_SCREEN_WIDTH*2);j++)
		{
			src[j-len*2*i]=src[j];
		}
	}
}
void save_SDIyuyv_pic(void *pic,int w,int h)
{
	FILE * fp;
	fp=fopen("./Van_save_YUV.yuv","w");
	fwrite(pic,w*h*2,1,fp);
	fclose(fp);
}

void HDv4l_cam::UYVnoXquar(unsigned char *dst,unsigned char *src, int ImgWidth, int ImgHeight)
{
	for(int j =0;j<ImgHeight;j++)
	{
		for(int i=0;i<ImgWidth*2/4;i++)
		{
			*(dst+j*ImgWidth*3+i*6+0)=*(src+j*ImgWidth*2+i*4+0);
			*(dst+j*ImgWidth*3+i*6+1)=*(src+j*ImgWidth*2+i*4+1);
			*(dst+j*ImgWidth*3+i*6+2)=*(src+j*ImgWidth*2+i*4+2);

			*(dst+j*ImgWidth*3+i*6+3)=*(src+j*ImgWidth*2+i*4+0);
			*(dst+j*ImgWidth*3+i*6+4)=*(src+j*ImgWidth*2+i*4+3);
			*(dst+j*ImgWidth*3+i*6+5)=*(src+j*ImgWidth*2+i*4+2);

		}
	}
}

void HDv4l_cam::UYVY2UYV(unsigned char *dst,unsigned char *src, int ImgWidth, int ImgHeight)
{
#if 0
	if (ImgWidth==FPGA_SCREEN_WIDTH) //4副先进行切割
		{
//#pragma omp parallel for
		for(int i=0;i<4;i++)
		{
			UYVnoXquar(dst+i*FPGA_SINGLE_PIC_W*FPGA_SINGLE_PIC_H*3,src+i*FPGA_SINGLE_PIC_W*FPGA_SINGLE_PIC_H*2,FPGA_SINGLE_PIC_W,FPGA_SINGLE_PIC_H);
		}
		}
	else
	{
		ImgHeight/=4;
//#pragma omp parallel for
		for(int i=0;i<4;i++)
		{
		UYVnoXquar(dst+i*ImgWidth*ImgHeight*3,src+i*ImgWidth*ImgHeight*2,ImgWidth,ImgHeight);
		}
	}
#endif
	UYVnoXquar(dst,src,1280,720);

}



void HDv4l_cam::UYVquar(unsigned char *dst,unsigned char *src, int ImgWidth, int ImgHeight)
{
	for(int j =0;j<ImgHeight;j++)
	{
		for(int i=0;i<ImgWidth*2/4;i++)
		{
			*(dst+j*ImgWidth*4+i*8+1)=*(src+j*ImgWidth*2+i*4+1);
			*(dst+j*ImgWidth*4+i*8+0)=*(src+j*ImgWidth*2+i*4+0);
			*(dst+j*ImgWidth*4+i*8+2)=*(src+j*ImgWidth*2+i*4+2);
			*(dst+j*ImgWidth*4+i*8+3)=0;

			*(dst+j*ImgWidth*4+i*8+5)=*(src+j*ImgWidth*2+i*4+3);
			*(dst+j*ImgWidth*4+i*8+4)=*(src+j*ImgWidth*2+i*4+0);
			*(dst+j*ImgWidth*4+i*8+6)=*(src+j*ImgWidth*2+i*4+2);
			*(dst+j*ImgWidth*4+i*8+7)=0;
		}
	}
}
void HDv4l_cam::UYVY2UYVx(unsigned char *dst,unsigned char *src, int ImgWidth, int ImgHeight)
{
	if (ImgWidth==FPGA_SCREEN_WIDTH) //4副先进行切割
		{
			UYVquar(dst,src,FPGA_SINGLE_PIC_W,FPGA_SINGLE_PIC_H*4);
		}
	else
	{
		UYVquar(dst,src,ImgWidth,ImgHeight);
	}
}

void HDv4l_cam::YUVquar(unsigned char *dst,unsigned char *src, int ImgWidth, int ImgHeight)
{
	for(int j =0;j<ImgHeight;j++)
		{
			for(int i=0;i<ImgWidth*2/4;i++)
			{
				*(dst+j*ImgWidth*4+i*8+1)=*(src+j*ImgWidth*2+i*4);
				*(dst+j*ImgWidth*4+i*8+0)=*(src+j*ImgWidth*2+i*4+1);
				*(dst+j*ImgWidth*4+i*8+2)=*(src+j*ImgWidth*2+i*4+3);
				*(dst+j*ImgWidth*4+i*8+3)=0;

				*(dst+j*ImgWidth*4+i*8+5)=*(src+j*ImgWidth*2+i*4+2);
				*(dst+j*ImgWidth*4+i*8+4)=*(src+j*ImgWidth*2+i*4+1);
				*(dst+j*ImgWidth*4+i*8+6)=*(src+j*ImgWidth*2+i*4+3);
				*(dst+j*ImgWidth*4+i*8+7)=0;
			}
		}
}
void HDv4l_cam::YUYV2UYVx(unsigned char *dst,unsigned char *src, int ImgWidth, int ImgHeight)
{
#if 1
	if (ImgWidth==FPGA_SCREEN_WIDTH) //4副先进行切割
		{
		YUVquar(dst,src,FPGA_SINGLE_PIC_W,FPGA_SINGLE_PIC_H*4);
		//	RectFromPixels(src);
			//如果w=1280 h=1080,则进行截取
			//否则直接转换
		}
	else
	{
		YUVquar(dst,src,ImgWidth,ImgHeight);
	}
#if 0
	if(ImgWidth==FPGA_SCREEN_WIDTH)
		{
		static int a=0;
			if(a++==50)
			{
				save_SDIyuyv_pic(src,ImgWidth,ImgHeight);
			}
		}
#endif
#endif
	//YUVquar(dst,src,ImgWidth,ImgHeight);
#if 0
	unsigned char pp[1280*1080*4];
	if(ImgWidth==FPGA_SCREEN_WIDTH)
	{
		for(int i=0;i<1280*1080*4;i++)
			pp[i]=i;
		memcpy(dst,pp,1280*1080*4);
	}

#endif
#if 0
	int t[10]={0};
 timeval startT[20]={0};
	gettimeofday(&startT[4],0);
	ImgHeight/=4;
#pragma omp parallel for
for(int i=0;i<4;i++)
{
	YUVquar(dst+4*i*ImgWidth*ImgHeight,src+2*i*ImgWidth*ImgHeight,ImgWidth,ImgHeight);
}
	gettimeofday(&startT[5],0);
			t[2]=((startT[5].tv_sec-startT[4].tv_sec)*1000000+(startT[5].tv_usec-startT[4].tv_usec))/1000.0;
			printf("YUYV->UYVX=%d ms    \n",t[2]);
#endif
}
void HDv4l_cam::YUYV2RGB(unsigned char * src,unsigned char * dst,int w,int h)
{
#if 0
	int t[10]={0};
	timeval startT[20]={0};
	bool enhance=false;
#if 1
	if (w==FPGA_SCREEN_WIDTH) //4副先进行切割
	{
		RectFromPixels(src);
		//如果w=1280 h=1080,则进行截取
		//否则直接转换
	}
	gettimeofday(&startT[4],0);

	nv21_to_rgb(dst, w,h,src);
	gettimeofday(&startT[5],0);
			t[2]=((startT[5].tv_sec-startT[4].tv_sec)*1000000+(startT[5].tv_usec-startT[4].tv_usec))/1000.0;
			printf("deltatimet[5]-t[4] =%d ms    width=%d\n",t[2],w);
#else
	if(w==MAX_SCREEN_WIDTH)//６副图
		{

			Mat Src(h,w,CV_8UC2,src);
			Mat Dst(h,w,CV_8UC3,dst);
			gettimeofday(&startT[4],0);
			cvtColor(Src,Dst,CV_YUV2BGR_YUYV);
			gettimeofday(&startT[5],0);
			t[2]=((startT[5].tv_sec-startT[4].tv_sec)*1000000+(startT[5].tv_usec-startT[4].tv_usec))/1000.0;
			printf("deltatimet[5]-t[4] =%d ms    1920\n",t[2]);

		}
		else if (w==FPGA_SCREEN_WIDTH) //4副先进行切割
		{
#if 0
	//		gettimeofday(&startT[6],0);
			Mat Src(SDI_HEIGHT,SDI_WIDTH,CV_8UC2,src);
			Rect rect(0,0,w,h);
			Mat Roi=Src(rect);
			Mat Dst(h,w,CV_8UC3,dst);
			cvtColor(Roi,Dst,CV_YUV2BGR_YUYV);
#else
			RectFromPixels(src);
			Mat Src(h,w,CV_8UC2,src);
			Mat Dst(h,w,CV_8UC3,dst);
			gettimeofday(&startT[6],0);
			cvtColor(Src,Dst,CV_YUV2BGR_YUYV);
			gettimeofday(&startT[7],0);
			t[3]=((startT[7].tv_sec-startT[6].tv_sec)*1000000+(startT[7].tv_usec-startT[6].tv_usec))/1000.0;
			printf("deltatimet[7]-t[6] =%d ms    1280\n",t[3]);

#endif

			//如果w=1280 h=1080,则进行截取
			//否则直接转换
		}
#endif
	if(enhance)
	{
//todo
	}
#endif
}

void HDv4l_cam::YUYV2GRAY(unsigned char * src,unsigned char * dst,int w,int h)
{
	for(int i=0;i<w*h;i++)
	{
    *(dst++) =*(src) ;
    src+=2;
	}
}


bool HDv4l_cam::Open()
{
	int ret;
/*	setCurChannum();
	if(ret < 0)
			return false;
*/
	static bool Once=true;
	if(Once)
	{
		 start_queue();
		Once=false;
	}

	ret = open_device();
	if(ret < 0)
		return false;

	ret = init_device();
	if(ret < 0)
		return false;


	run();
	return true;
}

void HDv4l_cam::run()
{
	start_capturing();


	bRun = true;
}

void HDv4l_cam::stop()
{
	stop_capturing();
	bRun = false;
}


int HDv4l_cam::open_device(void)
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

	m_devFd = open(dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);
	if (-1 == m_devFd) {
		fprintf(stderr, "Cannot open '%s': %d, %s\n", dev_name, errno, strerror(errno));
		return FAILURE_DEVICEOPEN;
	}
	return EXIT_SUCCESS;
}




int HDv4l_cam::init_device(void)
{
	struct v4l2_capability cap;
		struct v4l2_cropcap cropcap;
		struct v4l2_crop crop;
		struct v4l2_format fmt;
		unsigned int min;

		if (-1 == xioctl(m_devFd, VIDIOC_QUERYCAP, &cap)) {
			if (EINVAL == errno) {
				fprintf(stderr, "%s is no V4L2 device\n", dev_name);
				return FAILURE_DEVICEINIT;
			} else {
				errno_exit("HD_VIDIOC_QUERYCAP");
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

		if (0 == xioctl(m_devFd, VIDIOC_CROPCAP, &cropcap)) {
			crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			crop.c = cropcap.defrect; /* reset to default */

			if (-1 == xioctl(m_devFd, VIDIOC_S_CROP, &crop)) {
			}
		} else {
			/* Errors ignored. */
		}

		CLEAR(fmt);

		fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (force_format) {
			fprintf(stderr, "Set HDyuyv\r\n");
			fmt.fmt.pix.width       = imgwidth; //replace  因为假设成GREY 来采集UYVY,所以GREY的宽度要乘以2
			fmt.fmt.pix.height      = imgheight; //replace
			fmt.fmt.pix.pixelformat = imgformat;
			fmt.fmt.pix.field       = V4L2_FIELD_ANY;
			//fmt.fmt.pix.code=0;
			//printf("******width =%d height=%d\n",fmt.fmt.pix.width,fmt.fmt.pix.height);

			if (-1 == xioctl(m_devFd, VIDIOC_S_FMT, &fmt))
			{
				 errno_exit("HD_VIDIOC_S_FMT");
			}

			/* Note VIDIOC_S_FMT may change width and height. */
		} else {
			/* Preserve original settings as set by v4l2-ctl for example */
			if (-1 == xioctl(m_devFd, VIDIOC_G_FMT, &fmt))
				errno_exit("HD_VIDIOC_G_FMT");
		}


		/* Buggy driver paranoia. */
		min = fmt.fmt.pix.width;
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




void HDv4l_cam::errno_exit(const char *s)
{
	fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));
	exit(EXIT_FAILURE);
}

int HDv4l_cam::xioctl(int fh, int request, void *arg)
{
	int ret;
	do {
		ret = ioctl(fh, request, arg);
	} while (-1 == ret && EINTR == errno);

	return ret;
}

void yuyv2gray(unsigned char* src,unsigned char* dst,int width,int height)
{
	for(int j = 0;j<height*width;j++)
	{
			dst[j] = src[2*j +1];
	}
	return ;
}

int HDv4l_cam::GetNowPicIdx(unsigned char *src)
{
	int picIdx=-1;
	picIdx=(int)*src;
	return picIdx;
}
int HDv4l_cam::ChangeIdx2chid(int idx)
{//0~9
	//int picidx=(GetNowPicIdx()+2);
	//return picidx;
	return 0;
}

void save_yuyv(char *filename,void *pic,int w,int h)
{
	FILE * fp;
	char buf[30];
	fp=fopen(filename,"w");
	fwrite(pic,w*h*2,1,fp);
	fclose(fp);
}
void save_rgb(char *filename,void *pic,int w,int h)
{
	Mat Pic(h,w,CV_8UC3,pic);
	imwrite(filename,Pic);
}

void save_single_pic(char *filename,void *pic,int w,int h)
{
	unsigned char dst[SDI_WIDTH*SDI_HEIGHT*3];
	Mat Src(h,w,CV_8UC2,pic);
	Mat Dst(h,w,CV_8UC3,dst);
	cvtColor(Src,Dst,CV_YUV2BGR_UYVY);
	imwrite(filename,Dst);
}
void save_yuyv2rgb(char *filename,void *pic,int w,int h)
{
	char bmp[1920*1080*4];
	Mat Pic(h,w,CV_8UC2,pic);
	Mat BMP(h,w,CV_8UC4,bmp);

}
void save_gray(char *filename,void *pic,int w,int h)
{
	Mat Pic(h,w,CV_8UC1,pic);
	imwrite(filename,Pic);
}

int HDv4l_cam::read_frame(int now_pic_format)
{
	int t[10]={0};
 timeval startT[20]={0};


	struct v4l2_buffer buf;
	int i=0;
	static int  count=0;
		CLEAR(buf);
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_USERPTR;

		if (-1 == xioctl(m_devFd, VIDIOC_DQBUF, &buf)) {
					switch (errno) {
					case EAGAIN:
						return -1;
					case EIO:
					//	printf("EIO~~~~~~~~~~~");
						/* Could ignore EIO, see spec. */
						/* fall through */
					default:
						errno_exit("VIDIOC_DQBUF");
					}
				}
				assert(buf.index < n_buffers);
			 if (buffers[buf.index].start!=NULL)
			{
				 char filename[20];
				 static int mvDectCount=0;
				 static int mv_count=0;
				 int CC_enh_mvd=0;
					int chid[2]={-1,-1};
					int chid_touch[2]={-1,-1};
					int nowGrayidx=-1;
					int nowpicW=SDI_WIDTH,nowpicH=SDI_HEIGHT;
					unsigned char **transformed_src_main=NULL;
					unsigned char **transformed_src_main_touch=NULL;
					unsigned char **transformed_src_sub=NULL;
					switch(now_pic_format)
					{
					case 0:
						chid[MAIN]=0;
				//		chid[SUB]=SUB_FPGA_FOUR;
						nowpicW=FPGA_SCREEN_WIDTH;
						nowpicH=FPGA_SCREEN_HEIGHT;
						transformed_src_main=&sigle_yuyv[0];
			//			transformed_src_sub=&FPGA4_bgr_data_sub;
						break;
					case 1:
						chid[MAIN]=1;
						nowpicW=FPGA_SCREEN_WIDTH;
						nowpicH=FPGA_SCREEN_HEIGHT;
						transformed_src_main=&sigle_yuyv[1];
						break;
					case 2:
						chid[MAIN]=2;
						nowpicW=FPGA_SCREEN_WIDTH;
						nowpicH=FPGA_SCREEN_HEIGHT;
						transformed_src_main=&sigle_yuyv[2];
						break;
					default:
						break;
					}
			//					UYVY2UYV(*transformed_src_main,(unsigned char *)buffers[buf.index].start,nowpicW,nowpicH);

					selfcheck.JudgeByPixels((unsigned char *)buffers[buf.index].start,now_pic_format);
					HD_YUYV2UYV(*transformed_src_main,(unsigned char *)buffers[buf.index].start,nowpicW,nowpicH);
						Src=*transformed_src_main;

						if(Data2Queue(*transformed_src_main,nowpicW,nowpicH,chid[MAIN]))
						{
							if(getEmpty(&*transformed_src_main, chid[MAIN]))
							{
							}
						}

			}
					if (-1 ==xioctl(m_devFd, VIDIOC_QBUF, &buf)){
						fprintf(stderr, "VIDIOC_QBUF error %d, %s\n", errno, strerror(errno));
						exit(EXIT_FAILURE);
					}
	return 0;

}
void HDv4l_cam::stop_capturing(void)
{
	enum v4l2_buf_type type;
		switch (io) {
	//	case IO_METHOD_READ:
	//		/* Nothing to do. */
	//		break;
		case IO_METHOD_MMAP:
		case IO_METHOD_USERPTR:
			type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			if (-1 == xioctl(m_devFd, VIDIOC_STREAMOFF, &type))
				errno_exit("VIDIOC_STREAMOFF");
			break;
		}
}

void HDv4l_cam::start_capturing(void)
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

			if (-1 == xioctl(m_devFd, VIDIOC_QBUF, &buf))
				errno_exit("HD_VIDIOC_QBUF");
		}
		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (-1 == xioctl(m_devFd, VIDIOC_STREAMON, &type))
			errno_exit("HD_VIDIOC_STREAMON");
		break;
	case IO_METHOD_USERPTR:
		for (i = 0; i < n_buffers; ++i) {
			struct v4l2_buffer buf;

			CLEAR(buf);
			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_USERPTR;
			buf.index = i;
			buf.m.userptr = (unsigned long)buffers[i].start;
			buf.length = buffers[i].length;

			if (-1 == xioctl(m_devFd, VIDIOC_QBUF, &buf))
				errno_exit("HD_VIDIOC_QBUF");
		}
		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (-1 == xioctl(m_devFd, VIDIOC_STREAMON, &type))
			errno_exit("HD_VIDIOC_STREAMON");
		break;
	default:
		break;
	}
}

void HDv4l_cam::uninit_device(void)
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
		for (i = 0; i < n_buffers; ++i){
			if(memType == MEMORY_NORMAL){
				free(buffers[i].start);
			}else{
					cudaFreeHost(buffers[i].start);
						}
		}
		break;
	default:
		break;
	}

	free(buffers);
}

void HDv4l_cam::init_read(unsigned int buffer_size)
{
	buffers = (struct buffer *)calloc(1, sizeof(*buffers));

	if (!buffers) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}

	buffers[0].length = buffer_size;
	buffers[0].start = (struct buffer *) malloc(buffer_size);

	if (!buffers[0].start) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}
}

void HDv4l_cam::init_mmap(void)
{
	struct v4l2_requestbuffers req;

	CLEAR(req);

	req.count = bufferCount;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;

	if (-1 == xioctl(m_devFd, VIDIOC_REQBUFS, &req)) {
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

//	printf("%s qbuf cnt = %d\n", dev_name, req.count);

	buffers = (struct buffer *)calloc(req.count, sizeof(*buffers));

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

		if (-1 == xioctl(m_devFd, VIDIOC_QUERYBUF, &buf))
			errno_exit("VIDIOC_QUERYBUF");

		buffers[n_buffers].length = buf.length;
		buffers[n_buffers].start  =	mmap(NULL /* start anywhere */,
										buf.length,
										PROT_READ | PROT_WRITE /* required */,
										MAP_SHARED /* recommended */,
										m_devFd, buf.m.offset);
		memset(buffers[n_buffers].start,0x80,buf.length);

		if (MAP_FAILED == buffers[n_buffers].start)
			errno_exit("mmap");
	}
}

void HDv4l_cam::init_userp(unsigned int buffer_size)
{
	struct v4l2_requestbuffers req;
	cudaError_t ret = cudaSuccess;
	unsigned int page_size;
	page_size=getpagesize();
	buffer_size=(buffer_size+page_size-1)&~(page_size-1);
	CLEAR(req);

	req.count  = bufferCount;//  different
	req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_USERPTR;

	if (-1 == xioctl(m_devFd, VIDIOC_REQBUFS, &req)) {
		if (EINVAL == errno) {
			fprintf(stderr, "%s does not support "
					"user pointer i/o\n", dev_name);
			exit(EXIT_FAILURE);
		} else {
			errno_exit("VIDIOC_REQBUFS");
		}
	}
	if (req.count < 2) {
			fprintf(stderr, "Insufficient buffer memory on %s\n", dev_name);
			exit(EXIT_FAILURE);
		}
//	printf("%s qbuf cnt = %d\n", dev_name, req.count);
	buffers = (struct buffer  *)calloc(req.count, sizeof(*buffers));

	if (!buffers) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}

	for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
		buffers[n_buffers].length = buffer_size;
		if(memType == MEMORY_NORMAL)
			buffers[n_buffers].start = memalign(page_size,buffer_size);
	else // MEMORY_LOCKED
			 ret = cudaHostAlloc(&buffers[n_buffers].start, buffer_size, cudaHostAllocDefault);
			assert(ret == cudaSuccess);
		//cudaFreeHost();

		if (!buffers[n_buffers].start) {
			fprintf(stderr, "Out of memory\n");
			exit(EXIT_FAILURE);
		}
	}
}


void HDv4l_cam::close_device(void)
{
	if (-1 == close(m_devFd))
		errno_exit("close");

	m_devFd = -1;
}

void HDv4l_cam::mainloop(int now_pic_format)
{
	fd_set fds;
	struct timeval tv;
	int ret;

		FD_ZERO(&fds);
		FD_SET(m_devFd, &fds);

		/* Timeout. */
		tv.tv_sec = 2;
		tv.tv_usec = 0;

		ret = select(m_devFd + 1, &fds, NULL, NULL, &tv);

		if (-1 == ret)
		{
			if (EINTR == errno)
				return;

			errno_exit("select");
		}else if (0 == ret)
		{
			fprintf(stderr, "select timeout\n");
			return;
			//exit(EXIT_FAILURE);
		}
			if (-1 == read_frame(now_pic_format))  /* EAGAIN - continue select loop. */
				return;
}


//--------------the decorator cap class------------
HDAsyncVCap4::HDAsyncVCap4(auto_ptr<BaseVCap> coreCap,int now_format):
	m_core(coreCap),thread_state(THREAD_READY),sleepMs(DEFAULT_THREAD_SLEEP_MS)
{
	pic_format=now_format;
	initLock();
	start_thread(capThread,this);
	cout<<" HDstart a capture thread"<<endl;
}
HDAsyncVCap4:: ~HDAsyncVCap4()
{
	Close();
}
bool HDAsyncVCap4::Open()
{
	if(thread_state != THREAD_READY)
		return false;//do not support reopen.
	bool isOpened = m_core->Open();
	if(isOpened){
		Start();
	}
	return isOpened;
}

void HDAsyncVCap4::Close()
{
	thread_state = THREAD_STOPPING;
	while( THREAD_IDLE != thread_state){
		usleep(100*1000);
	}
	m_core->Close();
}


void HDAsyncVCap4::Capture(char* ptr)
{
	lock_read(ptr);
}

void HDAsyncVCap4::SetDefaultImg(char *p)
{
	m_core->SetDefaultImg(p);
}

 void HDAsyncVCap4::SavePic(const char* name)
 {
	m_core->SavePic(name);
 }

 unsigned char * HDAsyncVCap4::GetSrc()
 {
	return m_core->GetSrc();
};
void HDAsyncVCap4::Run()
{
	do{
		usleep(100*1000);
	}while(THREAD_READY == thread_state);
	//cap in background thread
	while(thread_state == THREAD_RUNNING)
	{
		if(tIdle.isToIdle(pic_format))
		{
			usleep(500*1000);
		}
		else
		{
			HDv4l_cam * pcore = dynamic_cast<HDv4l_cam*>(m_core.get());
			if(pcore){
				pcore->mainloop(pic_format);
			}
		}
//		usleep(sleepMs*1000);
	}
	thread_state = THREAD_IDLE;
	destroyLock();

}

void* HDAsyncVCap4::capThread(void*p)
{
	HDAsyncVCap4 *thread = (HDAsyncVCap4*)p;
	thread->Run();
	return NULL;
}

void HDAsyncVCap4::lock_read(char *ptr)
{
	int pic_size=0;
	char *pImg = m_core->GetDefaultImg();
	pthread_rwlock_rdlock(&rwlock);
	pic_size=SDI_WIDTH*SDI_HEIGHT*2;
	MEMCPY(ptr, pImg,pic_size);
	pthread_rwlock_unlock(&rwlock);
}

void HDAsyncVCap4::lock_write(char *ptr)
{
	int pic_size=0;
	char *pImg = m_core->GetDefaultImg();
	pthread_rwlock_wrlock(&rwlock);
	pic_size=SDI_WIDTH*SDI_HEIGHT*2;
	MEMCPY(pImg,ptr,pic_size);
	pthread_rwlock_unlock(&rwlock);
}
void  HDAsyncVCap4::Start()
{
	thread_state = THREAD_RUNNING;
}
void HDAsyncVCap4::destroyLock()
{
	pthread_rwlock_destroy(&rwlock);
	pthread_rwlockattr_destroy(&rwlockattr);
}

void HDAsyncVCap4::initLock()
{
	pthread_rwlockattr_init(&rwlockattr);
	pthread_rwlockattr_setpshared(&rwlockattr,2);
	pthread_rwlock_init(&rwlock,&rwlockattr);
}


bool HDv4l_cam::getEmpty(unsigned char** pYuvBuf, int chId)
{
	int status=0;
	bool ret = true;
	while(1)
	{
		status = OSA_bufGetEmpty(&queue_main_sub->bufHndl[chId],&m_bufId[chId],0);
		if(status == 0)
		{
			*pYuvBuf = (unsigned char*)queue_main_sub->bufHndl[chId].bufInfo[m_bufId[chId]].virtAddr;
			break;
		}else{
			if(!OSA_bufGetFull(&queue_main_sub->bufHndl[chId],&m_bufId[chId],OSA_TIMEOUT_FOREVER))
			{
				if(!OSA_bufPutEmpty(&queue_main_sub->bufHndl[chId],m_bufId[chId]))
				{
					;
				}
			}
		}
	}
	 return ret;
}

bool HDv4l_cam::Data2Queue(unsigned char *pYuvBuf,int width,int height,int chId)
{
	int status;

//	chId = 0;
	if(chId>=QUE_CHID_COUNT)//if(chId >= CAM_COUNT+1)
		return false;
	queue_main_sub->bufHndl[chId].bufInfo[m_bufId[chId]].width=width;
	queue_main_sub->bufHndl[chId].bufInfo[m_bufId[chId]].height=height;
	queue_main_sub->bufHndl[chId].bufInfo[m_bufId[chId]].strid=width;
	OSA_bufPutFull(&queue_main_sub->bufHndl[chId],m_bufId[chId]);
	return true;
}

void  HDv4l_cam::start_queue()
{
#if 0
	getEmpty(&select_bgr_data_main,MAIN_ONE_OF_TEN);
	getEmpty(&select_bgr_data_sub,SUB_ONE_OF_TEN );
	getEmpty(&FPGA6_bgr_data_main,MAIN_FPGA_SIX);
	getEmpty(&FPGA6_bgr_data_sub, SUB_FPGA_SIX);
	getEmpty(&FPGA4_bgr_data_main,MAIN_FPGA_FOUR);
	getEmpty(&FPGA4_bgr_data_sub, SUB_FPGA_FOUR);
	getEmpty(&select_bgr_data_main_touch, MAIN_ONE_OF_TEN_TOUCH);
#endif
	for(int j = 0;j<3;j++)
		getEmpty(&(sigle_yuyv[j]),j);
}


