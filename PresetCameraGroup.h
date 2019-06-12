/*
 * GLRender.h
 *
 *  Created on: Dec 27, 2016
 *      Author: hoover
 */
#include "GLFrame.h"
#include "StlGlDefines.h"
#include <opencv/cv.hpp>
using namespace cv;

class PresetCamera
{
public:
	PresetCamera(const char* fileName = NULL);
	void LoadCameraFrom(const char* fileName = NULL);
	const GLFrame & GetFrame();
	void SetFrame(const GLFrame & frame);
	void SaveCameraTo(const char* fileName = NULL);
private:
	const char* pFileName;
	FileStorage mYMLFile;
	GLFrame mCameraFrame;
};

class PresetCameraGroup
{
public:
	PresetCameraGroup();
	const GLFrame & GetCameraFrame(unsigned int idx);
	void LoadCameras();
	void SetCameraFrame(unsigned int idx, const GLFrame & frame);
	void SaveCameras();
	void SaveCameraTo(unsigned int idx, const char* fileName=NULL);
	void LoadCameraFrom(unsigned int idx,const char* filename = NULL);
private:
	enum {
		DEFAULT_RPESET_CAMERA_FRONT,
		DEFAULT_RPESET_CAMERA_FRONT_RIGHT,
		DEFAULT_RPESET_CAMERA_REAR_RIGHT,
		DEFAULT_RPESET_CAMERA_REAR,
		DEFAULT_RPESET_CAMERA_REAR_LEFT,
		DEFAULT_RPESET_CAMERA_FRONT_LEFT,
		DEFAULT_RPESET_CAMERA_COUNT
	};
	const char *pYMLFileNames[DEFAULT_RPESET_CAMERA_COUNT];
	PresetCamera mCameras[DEFAULT_RPESET_CAMERA_COUNT];
};
