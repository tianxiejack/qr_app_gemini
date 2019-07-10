/*
 * overLapRegion.h

 *
 *  Created on: August 10, 2017
 *      Author: van
 *
 *  save current CAM_COUNT images
 *  cut and save/get overLapRegion images
 *  caculate gain of each images
 */
#if USE_GAIN
#include"StlGlDefines.h"
using namespace std;
using namespace cv;

#ifndef OVERLAPREGION_H_
#define OVERLAPREGION_H_
#define MAXSLICES 512
class overLapRegion
{
public:
	bool van_save_coincidence();
	void brightness_blance();
	void push_overLap_triangle(int direction,uint x){vectors[direction].push_back(x);};
	void push_overLap_PointleftAndright(int direction_leftcam,cv::Point2f pointleft,
			cv::Point2f pointright);
	bool beExist();
	static  overLapRegion * GetoverLapRegion();
	void set_change_gain(bool);
	bool get_change_gain();
	void SetSingleHightLightState(bool state){EnableSingleHightLight=state;};
	bool GetSingleHightLightState(){return EnableSingleHightLight;};
	int GetHeightSlices(){return HEIGHT_SLICES;};
	float getPercentX(int camidx,int leftorright);
	private:
	overLapRegion();
	static  overLapRegion *overlapregion;
	std::vector<int> vectors[CAM_COUNT];
	std::vector<int> max_min[CAM_COUNT];
	Mat roi_image[CAM_COUNT][ROI_COUNT];
	int HEIGHT_SLICES;
	bool IsDealingWithSLICES;
	bool CHANGE_GAIN;
	bool EnableSingleHightLight;
	vector<cv::Point2f> m_pointLeft[CAM_COUNT];
	vector<cv::Point2f> m_pointRight[CAM_COUNT];
	unsigned char *Src4,*temp4;
	unsigned char *Src6,*temp6;
	float midX[CAM_COUNT][ROI_COUNT];

};

#endif
#endif
