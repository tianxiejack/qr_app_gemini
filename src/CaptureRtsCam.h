/*
 * Capture.hpp
 *
 *  Created on: Feb 18, 2019
 *      Author: wj
 */

#ifndef CPATURE_HPP_
#define CPATURE_HPP_

#include <iostream>
#include <fstream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "queue_display.h"
#define CAPTURE_PURE_VIRTUAL =0
using namespace std;
extern Alg_Obj * queue_main_sub;
typedef void (*CaptureFrameCallback)(const cv::Mat frame,const int chId);
class Capture{
public:
	Capture(){};
public:
	virtual ~Capture() {};
	virtual void init(std::string devname,int chId,int width=1920,int height=1080,CaptureFrameCallback callback=NULL)CAPTURE_PURE_VIRTUAL;
	virtual void uninit()CAPTURE_PURE_VIRTUAL;

	//virtual void setparam();
};

Capture *RTSPCapture_Create();
void RTSPCapture_Create(Capture *obj);

class RTSCam{
public:
	RTSCam();
	~RTSCam();
	//read param
	int ReadOnvifConfigFile();
	void SetMat(cv::Mat frame,int chid);
	bool Data2Queue(unsigned char *pYuvBuf,int width,int height,int chId);
	void uyvy2uyv(unsigned char *dst,unsigned char *src, int ImgWidth, int ImgHeight);
	bool getEmpty(unsigned char** pYuvBuf, int chId);
	void  start_queue();
private:
	char devname[128];
	Capture *Rstcam[3];
};


#endif /* DETECTOR_HPP_ */
