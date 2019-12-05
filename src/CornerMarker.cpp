/*
 * CornerMarker.cpp
 *
 *  Created on: May 3, 2017
 *      Author: hoover
 */
#include "CornerMarker.h"
#include "GLGeometryTransform.h"
#include "StopWatch.h"
#include <iostream>
#include <GL/glut.h>
using namespace std;

extern void* getDefaultShaderMgr();
extern void* getDefaultTransformPipeline(GLEnv &m_env);
CornerMarker::CornerMarker(GLMatrixStack &modelViewMat,GLMatrixStack	&projectionMat,GLShaderManager* mgr,
		const float* Color)
	: m_pShaderManager(mgr),modelViewMatrix(modelViewMat),projectionMatrix(projectionMat),p_Color(Color)
{
	if(NULL == m_pShaderManager){
			m_pShaderManager = (GLShaderManager*)getDefaultShaderMgr();
		}
	SetPosition(70.0f,50.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	//gltMakeSphere(m_TriangleBatch, 0.03f, 20, 18);
	gltMakeTorus(m_TriangleBatch, 0.022f, 0.003f,30,30);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

CornerMarker::~CornerMarker()
{

}

void CornerMarker::Draw(GLEnv &m_env, float rx, float ry, float rz,const float* color)
{
	GLGeometryTransform * pTransformPipeline = (GLGeometryTransform *)getDefaultTransformPipeline(m_env);
	if(!color){
		color = p_Color;
	}
	static CStopWatch	rotTimer;
	float yRot = rotTimer.GetElapsedSeconds() * 60.0f;
	   const M3DMatrix44f &mCamera = pTransformPipeline->GetModelViewProjectionMatrix();

	    // 将光源位置变换到视觉坐标系
	    M3DVector4f vLightPos = { 0.0f, 10.0f, 5.0f, 1.0f };
	    M3DVector4f vLightEyePos;
	    m3dTransformVector4(vLightEyePos, vLightPos, mCamera);

	modelViewMatrix.PushMatrix();
	modelViewMatrix.Translate(mx, my, mz);

	modelViewMatrix.PushMatrix();
	modelViewMatrix.Rotate(yRot * -2.0f, rx, ry, rz);
	m_pShaderManager->UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF, pTransformPipeline->GetModelViewMatrix(),
			pTransformPipeline->GetProjectionMatrix(),vLightEyePos, color);

	m_TriangleBatch.Draw();
	modelViewMatrix.PopMatrix();

	modelViewMatrix.PopMatrix();
}

//------------------CornerMarkerGroup implementation------------------
#define DEFAULT_CORNER_YML						"CornerPosition.yml"
CornerMarkerGroup::CornerMarkerGroup(GLMatrixStack &modelViewMat,GLMatrixStack	&projectionMat,GLShaderManager* mgr)
	:m_CornerMarker(modelViewMat, projectionMat, mgr),mAdjustingId(-1),
	 M((unsigned int )CORNER_COUNT,3,CV_32F)
{
	FileStorage YMLFile;
	YMLFile.open(DEFAULT_CORNER_YML,FileStorage::READ);
    if(YMLFile.isOpened()){
    	YMLFile["CornerGroup"] >> M;
    	YMLFile.release();
    	cout<<"Successfully read corners "<<endl;
    }
    else{// init default positions

		M.at<float>(CORNER_FRONT_LEFT,0 ) = -0.5f;
		M.at<float>(CORNER_FRONT_LEFT,1 ) = -0.6f;
		M.at<float>(CORNER_FRONT_LEFT,2 ) = 0.1f;

		M.at<float>(CORNER_FRONT_RIGHT,0 ) = 0.5f;
		M.at<float>(CORNER_FRONT_RIGHT,1 ) = -0.6f;
		M.at<float>(CORNER_FRONT_RIGHT,2 ) = 0.1f;

		M.at<float>(CORNER_REAR_LEFT,0 ) = -0.5f;
		M.at<float>(CORNER_REAR_LEFT,1 ) = 0.6f;
		M.at<float>(CORNER_REAR_LEFT,2 ) = 0.1f;

		M.at<float>(CORNER_REAR_RIGHT,0 ) = 0.5f;
		M.at<float>(CORNER_REAR_RIGHT,1 ) = 0.6f;
		M.at<float>(CORNER_REAR_RIGHT,2 ) = 0.1f;
		cout<<"read corners from default"<<endl;
    }
}

void CornerMarkerGroup::SetCornerPos(CORNER_POSITION i,float x, float y, float z)
{
	M.at<float>(i,0 ) = x;
	M.at<float>(i,1 ) = y;
	M.at<float>(i,2 ) = z;
}

void CornerMarkerGroup::SaveCorners(const char *fileName)
{
	FileStorage YMLFile;
	if(!fileName){
		fileName = DEFAULT_CORNER_YML;
	}
	YMLFile.open(fileName,FileStorage::WRITE);

	if(YMLFile.isOpened()){
	    YMLFile<<"CornerGroup"<< M;
	    YMLFile.release();
	 }
	else{
		cout<<"Failed to Open "<<fileName<<" to save corner\n";
	}
}
void CornerMarkerGroup::DrawCorner(GLEnv &m_env,CORNER_POSITION i, const float* color, float rotx, float roty,float rotz)
{
	m_CornerMarker.SetPosition(
			M.at<float>(i,0 ) ,
			M.at<float>(i,1 ) ,
			M.at<float>(i,2 ) );
      if(mAdjustingId == i){// whitten the one being adjusted and rotate differently
		rotx = 1.0f;
		roty = 0.0f;
		color = vWhite;
      	}
	m_CornerMarker.Draw(m_env,rotx,roty,rotz, color );
}

void CornerMarkerGroup::LoadCorners( const char *fileName)
{
	FileStorage YMLFile;
	if(fileName){
		YMLFile.open(fileName,FileStorage::READ);
	}

    if(YMLFile.isOpened()){
    	YMLFile["CornerGroup"] >> M;
    	YMLFile.release();
    }
    else{
    	cout<<"Loading Corners from"<<DEFAULT_CORNER_YML<<endl;
    }
}

void CornerMarkerGroup::Move(float dx, float dy)
{
      assert(isAdjusting());
  	M.at<float>(mAdjustingId, 0) += dx;
	M.at<float>(mAdjustingId, 1) += dy; 
}

void CornerMarkerGroup::StopAdjust()
{
    glutSetCursor(GLUT_CURSOR_NONE);
    doAdjust(CORNER_COUNT);
}

 void CornerMarkerGroup::Adjust(CORNER_POSITION id)
{
     glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
     doAdjust( id);
}
