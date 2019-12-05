/*
 * DynamicTrack.h
 *
 *  Created on: Apr 6, 2017
 *      Author: hoover
 */

#ifndef DYNAMICTRACK_H_
#define DYNAMICTRACK_H_


#include "StlGlDefines.h"
#include "GLShaderManager.h"
#include "GLMatrixStack.h"
#include "GLTriangleBatch.h"
#include "GLEnv.h"
class DynamicTrack{
public:
	DynamicTrack(GLMatrixStack &modelViewMat,GLMatrixStack	&projectionMat,GLShaderManager* mgr=NULL);
	~DynamicTrack(){delete []m_ptracks; delete []p_TriangleBatch;};
    void SetArcDeltaAngle(float angle){m_deltaAngle = angle;};
	void SetWheelDistance(float wheelDist){m_WheelDist = wheelDist;};
	void SetAxleDistance(float l){m_AxleDist = l;};
	void SetAcrCenter(float center[3]){m_center[0]=center[0];m_center[1]=center[1];m_center[2]=center[2];};
	void SetArcWidth(float arcWidth){m_arcWidth = arcWidth;};
	void UpdateArcs(float arcWidth, float deltaAngle);
	void DrawTracks(GLEnv &m_env);
	float* GetArcCenter(){return m_center;};
	float& GetArcWidth(){return m_arcWidth;};
	float& GetWheelDistance(){return m_WheelDist;};
	// note angle in degree.
	void SetAngle(float angle);
	void RefreshAngle();
	void SetReverseMode(bool bRevMode){m_bReverseMode = bRevMode;};
private:
	//draw an arc (centered in m_center) of radius
	void UpdateArc(unsigned int i, float radius, float arcWidth, float start_angle, float end_angle, bool isNewBuf);
	void UpdateArcCenter();
    void Init();
    bool GetArcRadius(int i, float &v); // get the ith arc's radius
    bool GetArcStartAngle(int i, float &v);// get the ith arc's start angle
	GLShaderManager * 	m_pShaderManager;
	GLMatrixStack &		modelViewMatrix;
	GLMatrixStack &		projectionMatrix;
	GLTriangleBatch *   p_TriangleBatch;
	GLuint startList;
	float m_WheelDist, m_AxleDist, m_arcWidth, m_angle, m_deltaAngle;// Note: angles in degree
	float m_center[3];
	const unsigned int m_trackCount;
	unsigned int *m_ptracks;
	bool m_bReverseMode;
};


#endif /* DYNAMICTRACK_H_ */

