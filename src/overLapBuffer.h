/*
 * overLapBuffer.h
 *
 *  Created on: Feb 16, 2017
 *      Author: wang
 */

#ifndef OVERLAPBUFFER_H_
#define OVERLAPBUFFER_H_

#include "StlGlDefines.h"
#include "opencv2/opencv.hpp"
using namespace cv;

enum{
	ENUM_NOW=0,
	ENUM_OLD,
	ENUM_COUNT
};
enum{
	ENUM_RIGHT=0,
	ENUM_LEFT,
};
typedef struct{
	Mat buf[ENUM_COUNT];
}mark;

typedef struct{
	mark direction[ENUM_COUNT];
}Gap;

class overLapBuffer {
public:
	overLapBuffer();
	virtual ~overLapBuffer();
	void setPoints(Point2f P[][2][4]);
	void updateGap(int index, Mat& buf);
private:
	void updateGapLeft(int index, Mat& buf);
	void updateGapRight(int index, Mat& buf);
	void updateGapBuffer(int chId, int mark, Mat& buf);
	void swap(int index, int mark);
	void copyToGap(int index, int mark, Mat& buf);
	void calcDiffSum(int chId, int mark);
	void calcDiffCh(int chId);
	double getFrameAbsDiffSum(Mat& pCurImg, Mat& pPreImg, Rect zoneT);
	void computeStatus(int chId, int mark);
	int compareStatus(int chId, bool left, bool right);
	void updateOverlap(int chId, int alpha);
	Rect calcRect(Point2f pos[4]);

	Gap gap[CAM_COUNT];
	Rect zone[CAM_COUNT][ENUM_COUNT];
	double status[CAM_COUNT][ENUM_COUNT];
};

extern overLapBuffer overLap;
#endif /* OVERLAPBUFFER_H_ */
