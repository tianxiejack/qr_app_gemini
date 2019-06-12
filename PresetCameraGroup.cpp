
#include <iostream>
#include "assert.h"
#include "PresetCameraGroup.h"

#define DEFAULT_PRESET_CAMERA_FRONT_YML			"00.prs"
#define DEFAULT_PRESET_CAMERA_FRONT_LEFT_YML	"01.prs"
#define DEFAULT_PRESET_CAMERA_REAR_LEFT_YML		"02.prs"
#define DEFAULT_PRESET_CAMERA_REAR_YML			"03.prs"
#define DEFAULT_PRESET_CAMERA_REAR_RIGHT_YML	"04.prs"
#define DEFAULT_PRESET_CAMERA_FRONT_RIGHT_YML	"05.prs"
using namespace std;
PresetCamera::PresetCamera(const char* fileName):pFileName(fileName)
{
	mCameraFrame.MoveForward(-75.0f);// default position.
}

void PresetCamera::LoadCameraFrom(const char* fileName)
{
	mYMLFile.open(fileName,FileStorage::READ);
	GLFrame &tmpFrame = mCameraFrame;
    if(mYMLFile.isOpened()){
    	
    	M3DVector3f tmpVector;
    	Mat M(3,3,CV_32F);
    	mYMLFile["GLFrame"] >> M;
    	for(int i = 0; i< 3; i++)
    		tmpVector[i] = M.at<float>(0,i );
    	tmpFrame.SetOrigin(tmpVector);

    	for(int i = 0; i< 3; i++)
    		tmpVector[i] = M.at<float>(1,i);
    	tmpFrame.SetForwardVector(tmpVector);

    	for(int i = 0; i< 3; i++)
    	    tmpVector[i] = M.at<float>(2,i);
    	tmpFrame.SetUpVector(tmpVector);

    	mYMLFile.release();
    }
    else{
    	cout<<"PresetCamera Fail to load "<<fileName<<",use default camera"<<endl;
    }
}

const GLFrame & PresetCamera::GetFrame()
{
	return mCameraFrame;
}

void PresetCamera::SetFrame(const GLFrame& frame)
{
	mCameraFrame = frame;
}

void PresetCamera::SaveCameraTo(const char* fileName)
{
	if(fileName==NULL)
		fileName = this->pFileName;
	mYMLFile.open(fileName,FileStorage::WRITE);
	    if(mYMLFile.isOpened()){
	    	GLFrame &tmpFrame = mCameraFrame;
	    	M3DVector3f tmpVector;
	    	Mat M(3,3,CV_32F);

	    	tmpFrame.GetOrigin(tmpVector);
	    	for(int i = 0; i< 3; i++){
	    		M.at<float>(0,i ) = tmpVector[i];
	    	}

	    	tmpFrame.GetForwardVector(tmpVector);
	    	for(int i = 0; i< 3; i++){
	    		M.at<float>(1,i ) = tmpVector[i];
	    	}

	    	tmpFrame.GetUpVector(tmpVector);
	    	for(int i = 0; i< 3; i++)
	    		M.at<float>(2,i ) = tmpVector[i];

	    	mYMLFile<<"GLFrame"<< M;
	    	mYMLFile.release();
	    }
	    else{
	    	cout<<"PresetCamera Fail to write "<<fileName<<endl;
	    }
}
//-------------------------------
PresetCameraGroup::PresetCameraGroup()
{
	pYMLFileNames[DEFAULT_RPESET_CAMERA_FRONT]= DEFAULT_PRESET_CAMERA_FRONT_YML;
	pYMLFileNames[DEFAULT_RPESET_CAMERA_FRONT_RIGHT]= DEFAULT_PRESET_CAMERA_FRONT_RIGHT_YML;
	pYMLFileNames[DEFAULT_RPESET_CAMERA_REAR_RIGHT]= DEFAULT_PRESET_CAMERA_REAR_RIGHT_YML;
	pYMLFileNames[DEFAULT_RPESET_CAMERA_REAR]= DEFAULT_PRESET_CAMERA_REAR_YML;
	pYMLFileNames[DEFAULT_RPESET_CAMERA_REAR_LEFT]= DEFAULT_PRESET_CAMERA_REAR_LEFT_YML;
	pYMLFileNames[DEFAULT_RPESET_CAMERA_FRONT_LEFT]= DEFAULT_PRESET_CAMERA_FRONT_LEFT_YML;
}
const GLFrame &  PresetCameraGroup::GetCameraFrame(unsigned int idx)
{
	assert(idx < DEFAULT_RPESET_CAMERA_COUNT);
	 return mCameras[idx].GetFrame();
}

void PresetCameraGroup::LoadCameras()
{
	for(int i = 0; i < DEFAULT_RPESET_CAMERA_COUNT; i ++){
		mCameras[i].LoadCameraFrom(pYMLFileNames[i]);
	}
}

void PresetCameraGroup::SetCameraFrame(unsigned int idx, const GLFrame & frame)
{
    assert(idx < DEFAULT_RPESET_CAMERA_COUNT);
    mCameras[idx].SetFrame(frame);
}

void PresetCameraGroup::SaveCameras()
{
	for(int i = 0; i < DEFAULT_RPESET_CAMERA_COUNT; i ++){
			mCameras[i].SaveCameraTo();
		}
}

void PresetCameraGroup::SaveCameraTo(unsigned int idx, const char* fileName)
{
	assert(idx < DEFAULT_RPESET_CAMERA_COUNT);
	if(NULL == fileName)
		fileName = pYMLFileNames[idx];
	mCameras[idx].SaveCameraTo(fileName);
}

void PresetCameraGroup::LoadCameraFrom(unsigned int idx, const char* filename)
{
	assert(idx < DEFAULT_RPESET_CAMERA_COUNT);
	if(NULL == filename)
		filename = pYMLFileNames[idx];
	mCameras[idx].LoadCameraFrom(filename);
}






	
