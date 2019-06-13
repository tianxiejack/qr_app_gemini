/*
 * Camera.cpp
 *
 *  Created on: Dec 31, 2016
 *      Author: wang
 * Refactored on Jan 6, 2017 by Hoover
 */
#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <string.h>
#include <stdbool.h>
#include "Camera.h"
#include "thread.h"
#include <unistd.h>
#include "buffer.h"
#include "overLapBuffer.h"
#include "StlGlDefines.h"
#include "FishCalib.h"
#include<sys/time.h>
#include <cuda.h>
#include <cuda_gl_interop.h>
#include <cuda_runtime_api.h>
#include <omp.h>
#include"assert.h"
#define MEMCPY memcpy


using namespace std;
using namespace cv;
unsigned char * temp_data_main[MAX_CC];
unsigned char * temp_data_sub[MAX_CC];
extern bool isEnhanceOn;
#if ENABLE_ENHANCE_FUNCTION
extern void cuHistEnh(cv::Mat src, cv::Mat dst);

extern void cuClahe(cv::Mat src, cv::Mat dst, unsigned int uiNrX = 8, unsigned int uiNrY = 8, float fCliplimit = 2.5, int procType = 0);

extern void cuUnhazed(cv::Mat src, cv::Mat dst);

extern "C" int rgbResize_(
	unsigned char *src, unsigned char *dst,
	int srcw, int srch, int dstw, int dsth );

extern "C" int uyvy2bgr_(
	unsigned char *dst, const unsigned char *src,
	int width, int height, cudaStream_t stream);

extern "C" int yuyv2bgr_ext_(
	unsigned char *dst, const unsigned char *src, unsigned char *gray,
	int width, int height, cudaStream_t stream);

extern "C" int yuyv2bgr_(
	unsigned char *dst, const unsigned char *src,
	int width, int height, cudaStream_t stream);


#endif




BaseVCap::~BaseVCap()
{
}
void BaseVCap::SavePic(const char* name)
{
	char * src = GetDefaultImg();
	Capture(src);
	cvSaveImage(name,src);
}
 unsigned char *BaseVCap::GetSrc()
 {
	 assert(false);
	 return NULL;
 }
void SmallVCap::Capture(char* ptr)
{
	get_buffer((unsigned char*)ptr, m_chId);
//	printf("capture:%d\n",m_chId);
};

void SmallVCap::CaptureFish(char* ptr)
{
//	printf("captureFish:pp\n");
	Mat outIMG = Mat(IMAGEHEIGHT, IMAGEWIDTH, CV_8UC4, ptr);//imread("./in.bmp");
	Mat inIMG= outIMG.clone();
	get_buffer((unsigned char*)inIMG.data, m_chId);
	//workaround bug#00038 green corner flashes from time to time,
	//by always setting first line to black.
	if((inIMG.data[0] != 192 && inIMG.data[1]!= 64)||
			(inIMG.data[2] != 112 && inIMG.data[3]!=64)){
		for(int i = 0; i < IMAGEWIDTH;  ){
			inIMG.data[i++] = 127;
			inIMG.data[i++] = 15;
			inIMG.data[i++] = 127;
			inIMG.data[i++] = 18;

			inIMG.data[i++] = 128;
			inIMG.data[i++] = 16;
			inIMG.data[i++] = 127;
			inIMG.data[i++] = 19;
		}
	}
	get_img(inIMG, outIMG);
//	printf("captureFish:%d\n",m_chId);
}

#define CLAP(min, max , val)	(val<min)?(min):((val>max)?(max):(val))



void  expandYuyvSDI(unsigned char* lpYUVFrame, unsigned char* pRGB, int width, int height)
{
#if	!NO_ARM_NEON
     int stride8x8 = width*2/8;
	static const int RGBADepth = 4;
    #pragma omp parallel for
	for(int y =0 ; y < height; y++){
		uint8x8_t * __restrict__ pSrc8x8_t,  * __restrict__ pDst08x8_t, * __restrict__ pDst18x8_t;
		int i, ii=0;
		pSrc8x8_t = (uint8x8_t *)(lpYUVFrame + y*width*2);
		pDst08x8_t = (uint8x8_t *)(pRGB + y*width*RGBADepth);
		pDst18x8_t = pDst08x8_t+1;



		for(i = 0; i < stride8x8; i ++, ii+=2){
				uint8x8_t src1, src2,tmp;
				tmp = src1 = src2 = pSrc8x8_t[i];
				tmp = vcopy_lane_u8(tmp,0,src1,2 );
				tmp = vcopy_lane_u8(tmp,4,src1,6 );
				src1 = vreinterpret_u8_u32(vcopy_lane_u32( vreinterpret_u32_u8(src1), 1, vreinterpret_u32_u8(tmp),0));

					pDst08x8_t[ii] = src1;
					src2 = vreinterpret_u8_u32(vcopy_lane_u32( vreinterpret_u32_u8(src2), 0, vreinterpret_u32_u8(src2),1));
					src2 = vreinterpret_u8_u32(vcopy_lane_u32( vreinterpret_u32_u8(src2), 1, vreinterpret_u32_u8(tmp),1));
					pDst18x8_t[ii] = src2;
		}
	}
#endif
}

void yuyv2YUxV(unsigned char *pYuvBuf, unsigned char* ptr, int width,int height)
{
	expandYuyvSDI( pYuvBuf,  ptr, width, height);
}


static void UYVx2RGB(unsigned char *lpYUV, int width, int height, Mat* RGBData)
{
	int i, j;
	unsigned char *lpSrc, *lpDst;
	int Y, U, V, R, G, B;

	for(j=0; j<height; j++)
	{
		for(i=0; i<width; i++)
		{
			lpSrc = lpYUV + j*width*4 + 4*i;
			lpDst = RGBData->data + j*width*3 + 3*i;
			U = *lpSrc;Y = *(lpSrc+1);V = *(lpSrc+2);
		   B = (Y + 1.402 * (U - 128.0));
		   G = (Y - 0.344 * (V - 128.0) - 0.714 * (U - 128.0));
		   R = (Y + 1.772 * (V - 128.0));

			*lpDst = CLAP(0,255,B); *(lpDst+1) = CLAP(0,255,G); *(lpDst+2) = CLAP(0,255,R);
		}
	}
}





void SmallVCap::SavePic(const char* name)
{
	get_buffer(m_buffer, m_chId);
	Mat img(576,720,CV_8UC3);
	UYVx2RGB(m_buffer, 720, 576, &img);
	try{
		imwrite(name,img);
//		waitKey(0);
	}catch(cv::Exception& os)
	{

	}
}
void SmallVCap::saveOverLap()
{
	get_bufferyuv(m_buffer, m_chId);
	UYVx2RGB(m_buffer, 720, 576, &yuvImg);
//	overLap.updateGap(m_chId,yuvImg);
}

void YUYV2UYVx(unsigned char *ptr,unsigned char *Yuyv, int ImgWidth, int ImgHeight)
{
	for(int j =0;j<ImgHeight;j++)
	{
		for(int i=0;i<ImgWidth*2/4;i++)
		{
			*(ptr+j*ImgWidth*4+i*8+1)=*(Yuyv+j*ImgWidth*2+i*4);
			*(ptr+j*ImgWidth*4+i*8+0)=*(Yuyv+j*ImgWidth*2+i*4+1);
			*(ptr+j*ImgWidth*4+i*8+2)=*(Yuyv+j*ImgWidth*2+i*4+3);
			*(ptr+j*ImgWidth*4+i*8+3)=0;

			*(ptr+j*ImgWidth*4+i*8+5)=*(Yuyv+j*ImgWidth*2+i*4+2);
			*(ptr+j*ImgWidth*4+i*8+4)=*(Yuyv+j*ImgWidth*2+i*4+1);
			*(ptr+j*ImgWidth*4+i*8+6)=*(Yuyv+j*ImgWidth*2+i*4+3);
			*(ptr+j*ImgWidth*4+i*8+7)=0;
		}
	}


	
}
//unsigned char *tmp_buffer=NULL;
//unsigned char *tmp_ptr=NULL;
void HDVCap::SavePic(const char* name)
{
	static unsigned char buffer[SDI_WIDTH*SDI_HEIGHT*2];
	static unsigned char ptr[SDI_WIDTH*SDI_HEIGHT*4];
	Mat img(SDI_HEIGHT,SDI_WIDTH,CV_8UC3);
//	OSA_semWait(&sem[m_chId],OSA_TIMEOUT_FOREVER);
//	memcpy(buffer,&sdi_data[m_chId],sizeof(buffer));
//	YUYV2UYVx(ptr,buffer,SDI_WIDTH,SDI_HEIGHT);
//	UYVx2RGB(ptr, SDI_WIDTH, SDI_HEIGHT, &img);
//	OSA_semSignal(&sem[m_chId]);
//	try{
//		imwrite(name,img);
//		waitKey(0);
//	}catch(cv::Exception& os)
}

void save_yuyv_pic2(void *pic,int idx)
{
	FILE * fp;
	char buf[30];
	sprintf(buf,"./save_transYUV_%d.yuv",idx);
	fp=fopen(buf,"w");
	char data[20];
	fwrite(pic,1920*1080*2,1,fp);
	fclose(fp);
}


void my_save_rgb(char *filename,void *pic,int w,int h)
{
	Mat Pic(h,w,CV_8UC3,pic);
	imwrite(filename,Pic);
}

#if ENABLE_ENHANCE_FUNCTION
void YUYVEnhanceFour(unsigned char * dst,unsigned char * src,int w,int h)
{
#if 1//ENABLE_ENHANCE_FUNCTION
		static unsigned char * gpu_uyvyFour;
		static unsigned char * gpu_rgbFour;
		static unsigned char * gpu_enhFour;
		static unsigned char * cpu_uyvyFour;
		static bool once =true;
		static cudaStream_t m_cuStreamFour[2];
		if(once)
		{
			cudaMalloc((void **)&gpu_uyvyFour,FPGA_SINGLE_PIC_W*FPGA_SINGLE_PIC_H*4*2);
			cudaMalloc((void **)&gpu_rgbFour,FPGA_SINGLE_PIC_W*FPGA_SINGLE_PIC_H*4*3);
			cudaMalloc((void **)&gpu_enhFour,(FPGA_SINGLE_PIC_W)*(FPGA_SINGLE_PIC_H*4)*3);

			for(int i=0; i<2; i++){
				cudaStreamCreate(&m_cuStreamFour[0]);
			}
			once=false;
			cpu_uyvyFour=(unsigned char *)malloc(FPGA_SINGLE_PIC_W*FPGA_SINGLE_PIC_H*4*2);
		}
		int noww=FPGA_SINGLE_PIC_W,nowh=0;
	//	if(w=1280)
	//	{
	//		nowh=FPGA_SINGLE_PIC_H*4;
	//	}
	//	else if(w==1920)
		{
			nowh=FPGA_SINGLE_PIC_H*4;
		}
		cudaMemcpy(gpu_uyvyFour,src,noww*nowh*2,cudaMemcpyHostToDevice);
		//yuyv2bgr_(gpu_rgb,gpu_yuyv,SDI_WIDTH,SDI_HEIGHT,m_cuStream[0]);
		uyvy2bgr_(gpu_rgbFour,gpu_uyvyFour,noww,nowh,m_cuStreamFour[0]);
		Mat dst1(nowh,noww,CV_8UC3,gpu_enhFour);
		Mat src1(nowh,noww,CV_8UC3,gpu_rgbFour);
		cuClahe( src1,dst1);		//, 4,4,4.5,0);
		cudaMemcpy(dst,gpu_enhFour,noww*nowh*3,cudaMemcpyDeviceToHost);
#endif
}
void YUYVEnhance(unsigned char * dst,unsigned char * src,int w,int h)
{
#if 1//ENABLE_ENHANCE_FUNCTION
		static unsigned char * gpu_uyvy;
		static unsigned char * gpu_rgb;
		static unsigned char * gpu_enh;
		static unsigned char * cpu_uyvy;
		static bool once =true;
		static cudaStream_t m_cuStream[2];
		if(once)
		{
			cudaMalloc((void **)&gpu_uyvy,FPGA_SINGLE_PIC_W*FPGA_SINGLE_PIC_H*6*2);
			cudaMalloc((void **)&gpu_rgb,FPGA_SINGLE_PIC_W*FPGA_SINGLE_PIC_H*6*3);
			cudaMalloc((void **)&gpu_enh,(FPGA_SINGLE_PIC_W)*(FPGA_SINGLE_PIC_H*6)*3);

			for(int i=0; i<2; i++){
				cudaStreamCreate(&m_cuStream[i]);
			}
			once=false;
			cpu_uyvy=(unsigned char *)malloc(FPGA_SINGLE_PIC_W*FPGA_SINGLE_PIC_H*6*2);
		}
		int noww=FPGA_SINGLE_PIC_W,nowh=0;
	//	if(w=1280)
	//	{
	//		nowh=FPGA_SINGLE_PIC_H*4;
	//	}
	//	else if(w==1920)
		{
			nowh=FPGA_SINGLE_PIC_H*6;
		}
		cudaMemcpy(gpu_uyvy,src,noww*nowh*2,cudaMemcpyHostToDevice);
		//yuyv2bgr_(gpu_rgb,gpu_yuyv,SDI_WIDTH,SDI_HEIGHT,m_cuStream[0]);
		uyvy2bgr_(gpu_rgb,gpu_uyvy,noww,nowh,m_cuStream[0]);
		Mat dst1(nowh,noww,CV_8UC3,gpu_enh);
		Mat src1(nowh,noww,CV_8UC3,gpu_rgb);
		cuClahe( src1,dst1);		//, 4,4,4.5,0);
		cudaMemcpy(dst,gpu_enh,noww*nowh*3,cudaMemcpyDeviceToHost);
#endif
}



void YUYVEnhance_Chosen(unsigned char * dst,unsigned char * src,int w,int h)
{
#if 1//ENABLE_ENHANCE_FUNCTION
		static unsigned char * gpu_uyvy;
		static unsigned char * gpu_rgb;
		static unsigned char * gpu_enh;
		static unsigned char * cpu_uyvy;
		static bool once =true;
		static cudaStream_t m_cuStream[2];
		if(once)
		{
			cudaMalloc((void **)&gpu_uyvy,w*h*2);
			cudaMalloc((void **)&gpu_rgb,w*h*3);
			cudaMalloc((void **)&gpu_enh,w*h*3);

			for(int i=0; i<2; i++){
				cudaStreamCreate(&m_cuStream[i]);
			}
			once=false;
			cpu_uyvy=(unsigned char *)malloc(w*h*2);
		}
		int noww=w,nowh=h;
		cudaMemcpy(gpu_uyvy,src,noww*nowh*2,cudaMemcpyHostToDevice);
		//yuyv2bgr_(gpu_rgb,gpu_yuyv,SDI_WIDTH,SDI_HEIGHT,m_cuStream[0]);
		uyvy2bgr_(gpu_rgb,gpu_uyvy,noww,nowh,m_cuStream[0]);
		Mat dst1(nowh,noww,CV_8UC3,gpu_enh);
		Mat src1(nowh,noww,CV_8UC3,gpu_rgb);
		cuClahe( src1,dst1);		//, 4,4,4.5,0);
		cudaMemcpy(dst,gpu_enh,noww*nowh*3,cudaMemcpyDeviceToHost);
#endif
}
#endif



/*
void HDVCap::YUYVEnhance(unsigned char *ptr,unsigned char *temp_data,int w,int h)
{
#if 1
	static unsigned char * gpu_yuyv;
			static unsigned char * gpu_rgb;
			static unsigned char * gpu_enh;

			static bool once =true;
			static cudaStream_t m_cuStream[2];
			if(once)
			{
				cudaMalloc((void **)&gpu_yuyv,w*h*2);
				cudaMalloc((void **)&gpu_rgb,w*h*3);
				cudaMalloc((void **)&gpu_enh,(w)*(h)*3);

				for(int i=0; i<2; i++){
					cudaStreamCreate(&m_cuStream[i]);
				}
				once=false;
			}
			cudaMemcpy(gpu_yuyv,temp_data,w*h*2,cudaMemcpyHostToDevice);
#if ENABLE_ENHANCE_FUNCTION
			yuyv2bgr_(gpu_rgb,gpu_yuyv,w,h,m_cuStream[0]);

			Mat dst1(h,w,CV_8UC3,gpu_enh);
			Mat src1(h,w,CV_8UC3,gpu_rgb);
			cuClahe( src1,dst1, 4,4,4.5,0);

			cudaMemcpy(ptr,gpu_enh,w*h*3,cudaMemcpyDeviceToHost);
#endif
#endif
}
*/
void HDVCap::YUYV2RGB(unsigned char*dst,unsigned char *src,int w,int h)
{
#if 1
	int t[10]={0};
 timeval startT[20]={0};
	gettimeofday(&startT[4],0);
	//memcpy(dst,src,w*h*2);
	Mat rgbmat(h,w,CV_8UC3,dst);
	Mat yuyvmat(h,w,CV_8UC2,src);
	cvtColor(yuyvmat,rgbmat,CV_YUV2BGR_YUYV);
	gettimeofday(&startT[5],0);
			t[2]=((startT[5].tv_sec-startT[4].tv_sec)*1000000+(startT[5].tv_usec-startT[4].tv_usec))/1000.0;
			printf("deltatimet[5]-t[4] =%d ms   \n",t[2]);
#endif
}

void HDVCap::CaptureTouchSrc(char* ptr,bool istouchsrc)
{
	if(istouchsrc && m_qid==MAIN_ONE_OF_TEN)
	{
			int nowqid=MAIN_ONE_OF_TEN_TOUCH;
			get_buffer((unsigned char *)ptr,nowqid);
#if ENABLE_ENHANCE_FUNCTION
		if(ptr[0]==SRCUYVY)
		{
			{
				static int count =0;
				if(count==50)
				{
					Mat Srcyuv(CENTER_HEIGHT,CENTER_WIDTH,CV_8UC2,ptr);
					unsigned char *p=(unsigned char *)malloc(CENTER_WIDTH*CENTER_HEIGHT*3);
					Mat Dst(CENTER_HEIGHT,CENTER_WIDTH,CV_8UC3,p);
					cvtColor( Srcyuv,Dst,CV_YUV2RGB_UYVY);
					imwrite("before.bmp",Dst);
				}
				YUYVEnhance_Chosen((unsigned char *)ptr,(unsigned char *)ptr,CENTER_WIDTH,CENTER_HEIGHT);
				if(count==50)
				{
					Mat m(CENTER_HEIGHT,CENTER_WIDTH,CV_8UC3,ptr);
					imwrite("after.bmp",m);
				}
				count ++;
			}
			ptr[0]=SRCUYVY;
		}
#endif
	}
	else
	{
		printf(" HDVCap::CaptureTouchSrc error!\n");
		assert(false);
	}
}
void HDVCap::Capture(char* ptr){
	get_buffer((unsigned char *)ptr,m_qid);
#if ENABLE_ENHANCE_FUNCTION
		if(ptr[0]==SRCUYVY)
		{
				YUYVEnhanceFour((unsigned char *)ptr,(unsigned char *)ptr,1280,720);
			ptr[0]=SRCUYVY;
		}
#endif
}



//--------------the decorator cap class------------
AsyncVCap::AsyncVCap(auto_ptr<BaseVCap> coreCap):
	m_core(coreCap),thread_state(THREAD_READY),sleepMs(DEFAULT_THREAD_SLEEP_MS)
{
	initLock();
	start_thread(capThread,this);
	cout<<" start a capture thread"<<endl;
}
AsyncVCap:: ~AsyncVCap()
{
	Close();
}
bool AsyncVCap::Open()
{
	if(thread_state != THREAD_READY)
		return false;//do not support reopen.
	bool isOpened = m_core->Open();
	if(isOpened){
		Start();
	}
	return isOpened;
}

void AsyncVCap::Close()
{
	thread_state = THREAD_STOPPING;
	while( THREAD_IDLE != thread_state){
		usleep(100*1000);
	}
	m_core->Close();
}
void AsyncVCap::Capture(char* ptr)
{
	lock_read(ptr);
}

void AsyncVCap::SetDefaultImg(char *p)
{
	m_core->SetDefaultImg(p);
}

 void AsyncVCap::SavePic(const char* name)
 {
	m_core->SavePic(name);
 }
 
void AsyncVCap::Run()
{
	char *p = new char[m_core->GetTotalBytes()];
	do{
		usleep(100*1000);
	}while(THREAD_READY == thread_state);
	//cap in background thread
	while(thread_state == THREAD_RUNNING)
	{
		m_core->Capture(p);
		lock_write(p);
		usleep(sleepMs*1000);
	}
	thread_state = THREAD_IDLE;
	destroyLock();
	delete []p;
}

void* AsyncVCap::capThread(void*p)
{
	AsyncVCap *thread = (AsyncVCap*)p;
	thread->Run();
	return NULL;
}

void AsyncVCap::lock_read(char *ptr)
{
	char *pImg = m_core->GetDefaultImg();
	pthread_rwlock_rdlock(&rwlock);
	MEMCPY(ptr, pImg,m_core->GetTotalBytes());
	pthread_rwlock_unlock(&rwlock);
}

void AsyncVCap::lock_write(char *ptr)
{
	char *pImg = m_core->GetDefaultImg();
	pthread_rwlock_wrlock(&rwlock);
	MEMCPY(pImg,ptr,m_core->GetTotalBytes());
	pthread_rwlock_unlock(&rwlock);
}
void  AsyncVCap::Start()
{
	thread_state = THREAD_RUNNING;
}
void AsyncVCap::destroyLock()
{
	pthread_rwlock_destroy(&rwlock);
	pthread_rwlockattr_destroy(&rwlockattr);
}

void AsyncVCap::initLock()
{
	pthread_rwlockattr_init(&rwlockattr);
	pthread_rwlockattr_setpshared(&rwlockattr,2);
	pthread_rwlock_init(&rwlock,&rwlockattr);
}
//--------------V4lVcap instance------------------
V4lVcap:: ~V4lVcap()
{
	Close();
}
bool V4lVcap::Open()
{
	if(capUSB)
		cvReleaseCapture(&capUSB);
	capUSB = cvCreateCameraCapture(m_deviceId);

	if(capUSB){
		cvSetCaptureProperty(capUSB, CV_CAP_PROP_FRAME_WIDTH, width);
		cvSetCaptureProperty(capUSB, CV_CAP_PROP_FRAME_HEIGHT, height);
		cvSetCaptureProperty(capUSB, CV_CAP_PROP_FPS, 30);
		cout<<" starting a USB capture at "<<width<<"x"<<height<<endl;
	}
	return capUSB != NULL;
}
void V4lVcap::Close()
{
	if(capUSB)
		cvReleaseCapture(&capUSB);
	capUSB= NULL;
}
void V4lVcap::Capture(char* ptr)
{
	assert(ptr);
	IplImage *tmp;
	char * src = NULL;
	if(capUSB){
		tmp = cvQueryFrame(capUSB);
		if(tmp){
	//		cout<<"img depth="<<tmp->depth<<",width="<<tmp->width<<",height="<<tmp->height<<",widthstep="<<tmp->widthStep<<endl;
			src = tmp->imageData;
		}
	}
	
	if(!src){
		src= GetDefaultImg();
	}
	if(src != ptr)
		MEMCPY(ptr,src , GetTotalBytes());
}
//-------------------BMPVCap implementation-------
BMPVcap::BMPVcap(const char * fileName):pic(NULL)
{
	assert(fileName);
	strncpy(pFileName, fileName, 64);
}
BMPVcap:: ~BMPVcap()
{
	Close();
}
bool BMPVcap::Open()
{
	bool ret = false;
//	cout<<"   BMPVCcap opening "<<pFileName<<endl;
	if(pic)
		cvReleaseImage(&pic);
	pic = cvLoadImage(pFileName);
	if(pic == NULL)
	{
		cerr<<"failed 2 load "<<pFileName<<" filled with color bar"<<endl;
		pic = cvCreateImage(Size(width, height), IPL_DEPTH_8U, depth);
		if(pic != NULL){
			int pic_bytes =0;
			pic_bytes=pic->height*pic->width*pic->depth;
			if(pic_bytes >GetTotalBytes() )
				pic_bytes=GetTotalBytes();
			memcpy(pic->imageData,GetDefaultImg(),pic_bytes);
			ret = true;
		}
	}
	else{
		Mat ycrcb;
		Mat im=cvarrToMat(pic);
#if 1
	/*	cvtColor( im,yuv_alpha,CV_RGB2BGR); //RGB->RGB
		memcpy(yuv_alpha.data,im.data,im.rows*im.cols*3);*/
		cvtColor( im, ycrcb, CV_RGB2YCrCb );
			cvtColor( im,yuv_alpha,CV_RGB2BGR); //RGB->RGB
			memcpy(yuv_alpha.data,im.data,im.rows*im.cols*3);
			Vec3b pix;
			Vec3b pix_alpha;
			for (int r = 0; r < ycrcb.rows; r++)
			{
				for (int c = 0; c < ycrcb.cols; c++)
				{
					pix = ycrcb.at<Vec3b>(r,c);
					pix_alpha.val[0]=pix.val[1];
					pix_alpha.val[1]=pix.val[0];
					pix_alpha.val[2]=pix.val[2];
					yuv_alpha.at<Vec3b>(r,c) = pix_alpha;
				}
			}
#else
				cvtColor( im, ycrcb, CV_RGB2YCrCb );
				cvtColor( im,yuv_alpha,CV_RGB2RGBA);
				memcpy(yuv_alpha.data,im.data,im.rows*im.cols*3);
				Vec3b pix;
				Vec4b pix_alpha;
				for (int r = 0; r < ycrcb.rows; r++)
				{
					for (int c = 0; c < ycrcb.cols; c++)
					{
						pix = ycrcb.at<Vec3b>(r,c);
						pix_alpha.val[0]=pix.val[1];
						pix_alpha.val[1]=pix.val[0];
						pix_alpha.val[2]=pix.val[2];
						pix_alpha.val[3]=pix.val[0];
						yuv_alpha.at<Vec4b>(r,c) = pix_alpha;
					}
				}
#endif
		cout<<"BMPVCap Open "<<pFileName<<" OK."<<endl;
		ret = true;
	}
	return ret;
}
void BMPVcap::Close()
{
	if(pic)
		cvReleaseImage(&pic);
	pic = NULL;
}
void BMPVcap::Capture(char* ptr)
{
	assert(ptr);
	if(pic)
	{
		memcpy(ptr,yuv_alpha.data,W_EXPAND_TIMES*H_EXPAND_TIMES*yuv_alpha.rows*yuv_alpha.cols*yuv_alpha.channels());
	}
}

//-----------------NVcamVcap implementation--------
NVcamVcap::~NVcamVcap()
{
	Close();
}

bool NVcamVcap::Open()
{
	if((capCSI !=NULL) && (capCSI->isOpened()))
		capCSI->release();
	
	char pGstPipelineString[1024];
	sprintf(pGstPipelineString,
			"nvcamerasrc sensor-id=%d ! video/x-raw(memory:NVMM), width=(int)%d, height=(int)%d,format=(string)I420, framerate=(fraction)60/1 ! nvvidconv flip-method=0 ! video/x-raw, format=(string)BGRx ! videoconvert ! appsink",
			m_sensorId, width, height);
	cout<<"NVcamVcap opening "<<pGstPipelineString<<endl;
	capCSI = new VideoCapture(pGstPipelineString); //open the default camera
	if((capCSI==NULL) || (!capCSI->isOpened())) { // check if we succeeded
		cerr << "Failed 2 open camera "<<m_sensorId<< endl;
		return false;
	}
	cout<<"NVcamVcap opening OK"<<endl;
	return true;
}
void NVcamVcap::Close()
{
	if((capCSI !=NULL) && (capCSI->isOpened()))
		capCSI->release();
	delete capCSI;
}
void NVcamVcap::Capture(char* ptr)
{
	assert(ptr);
	if(capCSI && capCSI->isOpened()){
		Mat frameDst(height,width,CV_8UC3,ptr);
		*capCSI>>(frameDst);
	}
	else{
		char * src = GetDefaultImg();
		if(ptr != src)
		    MEMCPY(ptr, src, GetTotalBytes());
	}
}

