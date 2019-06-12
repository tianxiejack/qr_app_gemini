#include "buffer.h"
#include "osa_que.h"
#include<sys/time.h>
#if !NO_ARM_NEON
#include <arm_neon.h>
#endif
#include<iostream>
#include<assert.h>
#include "StlGlDefines.h"
#include"Render_Agent.h"
using namespace std;
//using namespace cv;
typedef unsigned char byte;
Alg_Obj * queue_dis=NULL;
Alg_Obj * queue_main_sub=NULL;
void DeinterlaceYUV_Neon(unsigned char *lpYUVFrame, int ImgWidth, int ImgHeight, int ImgStride);

//void saveBmp(unsigned char *ptr, int chId)
//{
//	char buf[10];
//	sprintf(buf,"CHID=%d.bmp",chId);
//	IplImage* pic = cvCreateImage(Size(720, 576), IPL_DEPTH_8U, 3);
//	if(pic != NULL){
//		memcpy(pic->imageData, ptr, pic->imageSize);
//	}
//
//	try {
//		cvSaveImage(buf,pic);
//		waitKey(0);
//	} catch (cv::Exception& ex) {
////		printf("write error %s\n",ex.what());
//	}
//	cvReleaseImage(&pic);
//}
char dev_name[16];
#if !NO_ARM_NEON
inline void deInterlace(byte * pCurLine, byte * pTopLine, byte * pBotLine, byte * pDstLine)
{
    Mat img_tmp(1, IMAGE_WIDTH / 2, CV_32FC2, Scalar(0, 0));
    Mat img_sum(1, IMAGE_WIDTH / 2, CV_32FC2, Scalar(0, 0));
    Mat img_top(1, IMAGE_WIDTH / 2, CV_8UC2, pTopLine);
    Mat img_bot(1, IMAGE_WIDTH / 2, CV_8UC2, pBotLine);
    Mat img_lin(1, IMAGE_WIDTH / 2, CV_8UC2, pCurLine);
    Mat img_dst(1, IMAGE_WIDTH / 2, CV_8UC2, pDstLine);

    CvMat tmp = img_tmp;
    CvMat sum = img_sum;
    CvMat top = img_top;
    CvMat bot = img_bot;
    CvMat cur = img_lin;

    cvAcc(&top, &tmp);
    cvAcc(&bot, &tmp);
    cvAcc(&cur, &tmp);
    cvConvertScale(&tmp, &sum, 1.0 / 3.0);
    img_sum.convertTo(img_dst, CV_8UC2);
}
inline void deInterLace2(byte* src, int width, int height, int stride)
{
	int h=0;
#pragma omp parallel for
	for(h=1; h<height; h+=2)
	{
		byte* pCurLine = src + (h+0)*stride;
		byte* pTopLine = pCurLine - stride;
		byte* pBotLine = pCurLine + stride;

		Mat img_tmp(1, width, CV_32FC2, Scalar(0,0));
		Mat img_top(1, width, CV_8UC2, pTopLine);
		Mat img_bot(1, width, CV_8UC2, pBotLine);
		Mat img_mid(1, width, CV_8UC2, pCurLine);

		accumulate(img_top, img_tmp);
		accumulate(img_bot, img_tmp);
		accumulate(img_mid, img_tmp);
		img_tmp.convertTo(img_mid, CV_8UC2, 1/3.0);
	}
}
#endif
bool doValidate(unsigned char * pSrc, unsigned char * pDst, int i, int j){
	if(pSrc[i]!=pDst[j]){
	//	cout<<"~~Src["<<i<<"]("<<(int)pSrc[i]<<") != dst["<<j<<"]("<<(int)pDst[j]<<")"<<endl;
		return false;
	}
	else{
	//	cout<<"--Src["<<i<<"]("<<(int)pSrc[i]<<")==dst["<<j<<"]("<<(int)pDst[j]<<")"<<endl;
		return true;
	}

}
void validateSrcDst(unsigned char* pSrc, unsigned char * pDst){
	bool isValid = true;
    #define VALIDATE(i,j) {isValid &= doValidate(pSrc, pDst, i, j);}
// ----the 8 pixels----
	VALIDATE(0,0);//u0
	VALIDATE(1,1);//y0
	VALIDATE(2,2);//v0

	VALIDATE(0,4);//u0
	VALIDATE(3,5);//y1
	VALIDATE(2,6);//v0

	VALIDATE(4,8);//u1
	VALIDATE(5,9);//y2
	VALIDATE(6,10);//v1

	VALIDATE(4,12);//u1
	VALIDATE(7,13);//y3
	VALIDATE(6,14);//v1

	if(!isValid){
		printf("(%p)",pSrc);
		cout<<"<<src:";
		for(int i = 0; i < 8; i++){
			cout<<" "<<(int)pSrc[i];
		}
		cout<<endl;
		printf("(%p)",pDst);
		cout<<">>>>dst:";
		for(int i =0; i < 16; i++){
			cout<<","<<(int)pDst[i];
		}
		cout <<endl;
	}
}
void expandYUVPAL(const unsigned char* lpYUVFrame, unsigned char* pRGB, int width, int height)
{



#if	!NO_ARM_NEON
     int stride8x8 = width*2/8;
	static const int RGBADepth = DEFAULT_IMAGE_DEPTH;
    #pragma omp parallel for
	for(int y =0 ; y < height; y++){
		uint8x8_t * __restrict__ pSrc8x8_t,  * __restrict__ pDst08x8_t, * __restrict__ pDst18x8_t;
		int i, ii=0;
	//	unsigned char * pSrc, *pDst;
		pSrc8x8_t = (uint8x8_t *)(lpYUVFrame + y*width*2);
		pDst08x8_t = (uint8x8_t *)(pRGB + y*width*RGBADepth);
		pDst18x8_t = pDst08x8_t+1;

		for(i = 0; i < stride8x8; i ++, ii+=2){
			uint8x8_t src1, src2,tmp;
			tmp = src1 = src2 = pSrc8x8_t[i];
			tmp = vcopy_lane_u8(tmp,1,src1,3 );
			tmp = vcopy_lane_u8(tmp,5,src1,7 );
			src1 = vreinterpret_u8_u32(vcopy_lane_u32( vreinterpret_u32_u8(src1), 1, vreinterpret_u32_u8(tmp),0));
			pDst08x8_t[ii] = src1;
			src2 = vreinterpret_u8_u32(vcopy_lane_u32( vreinterpret_u32_u8(src2), 0, vreinterpret_u32_u8(src2),1));
			src2 = vreinterpret_u8_u32(vcopy_lane_u32( vreinterpret_u32_u8(src2), 1, vreinterpret_u32_u8(tmp),1));
			pDst18x8_t[ii] = src2;

		/*	pSrc = (unsigned char*)(&pSrc8x8_t[i]);
			pDst = (unsigned char*)(&pDst08x8_t[ii]);
		    validateSrcDst(pSrc, pDst);*/
		}

	}
#endif
}

#define CLAP(min, max , val)	(val<min)?(min):((val>max)?(max):(val))

static void YUV2RGB(unsigned char *lpYUV, int width, int height)
{
	assert(false);
/*	int i, j;
	unsigned char *lpSrc, *lpDst;
	int Y, U, V, R, G, B;
	Mat RGBData(height, width, CV_8UC3);

	for(j=0; j<height; j++)
	{
		for(i=0; i<width; i++)
		{
			lpSrc = lpYUV + j*width*4 + 4*i;
			lpDst = RGBData.data + j*width*3 + 3*i;
			U = *lpSrc;Y = *(lpSrc+1);V = *(lpSrc+2);

//			R= 1.164*(Y-16) + 1.596*(V-128);

//			G = 1.164*(Y-16) - 0.813*(V-128) - 0.392*(U-128);

//			B = 1.164*(Y-16) + 2.017*(U-128);
		   B = (Y + 1.402 * (U - 128.0));
		   G = (Y - 0.344 * (V - 128.0) - 0.714 * (U - 128.0));
		   R = (Y + 1.772 * (V - 128.0));

			*lpDst = CLAP(0,255,B); *(lpDst+1) = CLAP(0,255,G); *(lpDst+2) = CLAP(0,255,R);
		}
	}
	imwrite("RGB_1.bmp",RGBData);
*/
}

void yuv2UYVx(const unsigned char *pYuvBuf,unsigned char* ptr, int width,int height,int chId)
{
	expandYUVPAL(pYuvBuf, ptr, width*W_EXPAND_TIMES, height*H_EXPAND_TIMES);
}

#ifndef TIMEOUT_MS_GETBUFFER
#define TIMEOUT_MS_GETBUFFER 250 //OSA_TIMEOUT_FOREVER
#endif

void get_buffer(unsigned char* ptr, int chId)
{
	AlgLink_Obj_Multi *alghand_mul = &alglink_obj_mul[chId];
	Alg_Obj *alg_handle=alghand_mul->algLink_handle;
	int bufId=-1, width=0, height=0;
	unsigned char * bufdata;
	int state;
	int cnt=0;
	    for(;;){
	    	state=OSA_bufGetFull(&alg_handle->bufHndl[chId],&bufId,TIMEOUT_MS_GETBUFFER);
	       if(state != 0){
	    	    	//todo : if status == OSA_TIMEOUT(do somthing)
	    			return;
	    	   }
		if(OSA_queIsEmpty(&alg_handle->bufHndl[chId].fullQue))
			break;
		else
		{
			cnt++;
			OSA_bufPutEmpty(&alg_handle->bufHndl[chId],bufId);
		}
	}



//    if(cnt >= 2)
//	    printf("chId:%d,cnt:%d\n",chId,cnt);

	bufdata = (unsigned char *)alg_handle->bufHndl[chId].bufInfo[bufId].virtAddr;
	width   = alg_handle->bufHndl[chId].bufInfo[bufId].width;
	height  = alg_handle->bufHndl[chId].bufInfo[bufId].height;

//    yuv2UYVx(bufdata, ptr, width, height,chId);
	int w=SDI_WIDTH;
	if(chId==MAIN_FPGA_FOUR ||chId==SUB_FPGA_FOUR)
	{
		w=FPGA_SCREEN_WIDTH;
	}
//	if(Render_Agent:: IsUsePart()&&chId==MAIN_ONE_OF_TEN_TOUCH)
	if(chId==MAIN_ONE_OF_TEN_TOUCH)
	{
		Center_Rect *temprect=Render_Agent::GetCurrentRect();
		int factor=(int )(Render_Agent::GetTouchForesightPosY()*1080);
			int y=temprect->y+factor;
			if(y+temprect->height>1080)
				y=1080-temprect->height;


			if(bufdata[0]==SRCUYVY)
			{
				memcpy(ptr,bufdata+y*temprect->width*2,temprect->width*temprect->height*2);
				ptr[0]=SRCUYVY;
			}
			else if(bufdata[0]==SRCUYVUYV)
			{
				memcpy(ptr,bufdata+y*temprect->width*3,temprect->width*temprect->height*3);
				ptr[0]=SRCUYVUYV;
			}
	}
	else
	{
		memcpy(ptr,bufdata,w*SDI_HEIGHT*3);
	}
	OSA_bufPutEmpty(&alg_handle->bufHndl[chId],bufId);
}
void get_bufferyuv(unsigned char* ptr, int chId)
{
	AlgLink_Obj_Multi *alghand_mul = &alglink_obj_mul[chId];
	Alg_Obj *alg_handle=alghand_mul->algLink_handle;
	int bufId=-1, width=0, height=0;
	unsigned char * bufdata;
	int state;
	int cnt=0;
    for(;;){
		state=OSA_bufGetFull(&alg_handle->bufHndl[chId],&bufId,OSA_TIMEOUT_FOREVER);
		if(state != 0)
			return;
		if(OSA_queIsEmpty(&alg_handle->bufHndl[chId].fullQue))
			break;
		else
		{
			cnt++;
			OSA_bufPutEmpty(&alg_handle->bufHndl[chId],bufId);
		}
	}
//    if(cnt >= 2)
//	    printf("chId:%d,cnt:%d\n",chId,cnt);

	bufdata = (unsigned char *)alg_handle->bufHndl[chId].bufInfo[bufId].virtAddr;
	width   = alg_handle->bufHndl[chId].bufInfo[bufId].width;
	height  = alg_handle->bufHndl[chId].bufInfo[bufId].height;

	memcpy(ptr,bufdata,PANO_TEXTURE_WIDTH*PANO_TEXTURE_HEIGHT*4);
//    yuv2UYVx(bufdata, ptr, width, height,chId);
	OSA_bufPutEmpty(&alg_handle->bufHndl[chId],bufId);
}

void init_buffer()
{
	queue_main_sub = (Alg_Obj *)alg_buf_init();
	alg_obj_init(queue_main_sub);
}

void destroy_buffer()
{
	alg_buf_destroy(queue_main_sub);
}
