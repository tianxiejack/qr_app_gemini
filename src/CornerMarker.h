/*
 * CornerMarker.h
 *
 *  Created on: May 3, 2017
 *      Author: hoover
 */

#ifndef CORNERMARKER_H_
#define CORNERMARKER_H_

#include "StlGlDefines.h"
#include "GLShaderManager.h"
#include "GLMatrixStack.h"
#include "GLTriangleBatch.h"
#include "GLEnv.h"
#include <opencv/cv.hpp>
using namespace cv;

class CornerMarker{
public:
	CornerMarker(GLMatrixStack &modelViewMat,GLMatrixStack	&projectionMat,GLShaderManager* mgr=NULL,const float* Clr=vRed);
	~CornerMarker();
	inline void SetPosition(float x, float y, float z=0.0f){mx=x; my=y; mz=z;};
	void Draw(GLEnv &m_env,float rx, float ry, float rz,const float* color = NULL);
private:
	GLShaderManager * 	m_pShaderManager;
	GLMatrixStack &		modelViewMatrix;
	GLMatrixStack &		projectionMatrix;
	GLTriangleBatch *   p_TriangleBatch;

	GLTriangleBatch     m_TriangleBatch;
	float mx, my, mz;
	const float* p_Color;

};

class CornerMarkerGroup{
public:
	CornerMarkerGroup(GLMatrixStack &modelViewMat,GLMatrixStack	&projectionMat,GLShaderManager* mgr=NULL);
	~CornerMarkerGroup(){};
	void SetCornerPos(CORNER_POSITION cornerId,float x, float y, float z = 0.0f);
	void SaveCorners(const char *fileName=NULL);
	void DrawCorner(GLEnv &m_env,CORNER_POSITION cornerId, const float* color=NULL, float rx=0.0f,float ry = 1.0f, float rz=0.0f);
	inline bool isAdjusting(){return mAdjustingId>=0&& mAdjustingId<CORNER_COUNT;}
	void Adjust(CORNER_POSITION id);
	void StopAdjust();
	void Move(float dx, float dy);
private:
	inline void doAdjust(CORNER_POSITION id){mAdjustingId = id;}
	void LoadCorners( const char *fileName=NULL);
	CornerMarker m_CornerMarker;
	Mat M;
	int mAdjustingId;
};


#endif /* CORNERMARKER_H_ */
