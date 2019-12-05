/*
 * DynamicTrack.cpp
 *
 *  Created on: Apr 6, 2017
 *      Author: hoover
 */

#include "DynamicTrack.h"
#include "GLGeometryTransform.h"
#include <iostream>
#include "GLTools.h"
#include <cmath>
#include "GLEnv.h"
extern void* getDefaultShaderMgr();
extern void * getDefaultTransformPipeline(GLEnv &m_env);
using namespace std;

#define AMP_BIG 1.25f
#define ANGLE_DEC 3.0f
#define THE_MAX_ANGLE 359.99f
#define THE_MIN_ANGLE 0.01f

DynamicTrack::DynamicTrack(GLMatrixStack &modelViewMat,GLMatrixStack	&projectionMat,GLShaderManager* mgr)
					:m_WheelDist(DEFAULT_WHEEL_DIST), m_AxleDist((DEFAULT_AXLE_DIST)*(DEFAULT_VEHICLE_SCALE_HEIGHT)),
					m_arcWidth(DEFAULT_ARC_WIDTH_INI),startList(0),
					m_trackCount(DEFAULT_DYNAM_TRACK_COUNT),p_TriangleBatch(NULL),m_ptracks(NULL),
					m_angle(10.0),m_deltaAngle(DEFAULT_DELTA_ANGLE),m_bReverseMode(false),
					m_pShaderManager(mgr),modelViewMatrix(modelViewMat),projectionMatrix(projectionMat)
{
		if(NULL == m_pShaderManager){
			m_pShaderManager = (GLShaderManager*)getDefaultShaderMgr();
		}
		m_ptracks = new unsigned int[m_trackCount];
		for(int i = 0; i < m_trackCount; i ++){
			m_ptracks[i] = i;
		}
		p_TriangleBatch = new GLTriangleBatch[m_trackCount];
		Init();
}

void DynamicTrack::Init()
{

    GLuint &base = startList;
    float deltaAngle = m_deltaAngle;

	//显示列表索引相加，以获取最终的显示列表索引
	for(int i = 0; i < m_trackCount; i ++){
	   float radius = 0.0f;
	   GetArcRadius(i, radius);
	   float start_angle = 0.0f;
	   GetArcStartAngle(i, start_angle);

	   UpdateArc(i,radius, m_arcWidth, start_angle, start_angle + deltaAngle, true);

	}
	SetAngle(m_angle);
}
void DynamicTrack::DrawTracks(GLEnv &m_env)
{
	GLGeometryTransform * pTransformPipeline = (GLGeometryTransform *)getDefaultTransformPipeline(m_env);
	const float* Colors[6];
	Colors[0]=vGreen;
	Colors[1]=vGreen;
	Colors[2]=vGreen;
	Colors[3]=vGreen;
	Colors[4]=vWhite;
	Colors[5]=vLtGrey;
	glEnable(GL_BLEND);
	modelViewMatrix.PushMatrix();
	modelViewMatrix.Translate(m_center[0]- m_WheelDist/2, m_center[1], m_center[2]);
	for(int i = 0; i < m_trackCount; i++){
		m_pShaderManager->UseStockShader(GLT_SHADER_TEXTURE_MODULATE, pTransformPipeline->GetModelViewProjectionMatrix(), Colors[i], CAM_COUNT+1);

		p_TriangleBatch[i].Draw();
	}
	modelViewMatrix.PopMatrix();
}
void DynamicTrack::UpdateArcs(float arcWidth, float deltaAngle)
{
	for(int i = 0; i < m_trackCount; i++){
		float radius = 0.0f;
		GetArcRadius(i, radius);
		float end_angle = 0, start_angle =0.0f;
		bool isPositive = GetArcStartAngle(i, start_angle);
		float reverse_factor = m_bReverseMode ? -1.0f : 1.0f;
		if(!isPositive){
			start_angle = 180.0f - start_angle;
			end_angle = start_angle - reverse_factor*deltaAngle;
		}
		else{
			end_angle =  start_angle + reverse_factor*deltaAngle;
		}
		UpdateArc(i, radius, arcWidth, start_angle,end_angle,false);
	}
}

void DynamicTrack::UpdateArc(unsigned int i,float radius, float arcWidth, float start_angle, float end_angle, bool isNewBuf)
{
	bool bFlipTexture = (0 ==(i%2));
	gltMakeArc(p_TriangleBatch[i], radius, radius+arcWidth,32, 1, start_angle, end_angle, isNewBuf,bFlipTexture);
}


bool DynamicTrack::GetArcRadius(int i, float& value)
{
	float ret = 0.0f;
	float tan_Theta = tan(DEGREE2RAD(m_angle));
	bool bPositive = true;
	float amplify=1.0f;
	do{
		if(m_angle==THE_MAX_ANGLE)
		{
			amplify=AMP_BIG;
		}
		float Rb = abs(m_AxleDist*amplify/tan_Theta) - m_WheelDist /2.0f;
		bPositive = tan_Theta>-0.00001? true:false;
		switch (i)
		{
		case 1:
			Rb = Rb + m_WheelDist - m_arcWidth;
			//deliberately let through
		case 0:
		    ret = Rb;
		    break;
		case 3:
			Rb = Rb + m_WheelDist - m_arcWidth;
			//deliberately let through
		case 2:
			ret = sqrt(Rb*Rb + m_AxleDist*m_AxleDist*amplify*amplify);
			break;
		default:
			break;
		}
	}while (0);
	value = ret;

    return bPositive;
}

bool DynamicTrack::GetArcStartAngle(int i, float & value)
{
	float ret = 0.0f;
	float tan_Theta = tan(DEGREE2RAD(m_angle));
	bool bPositive = true;
	float amplify=1.0f;
	do{
		if(m_angle==359.99)
		{
			amplify=AMP_BIG;
		}
		float Rb = abs(m_AxleDist*amplify/tan_Theta) - m_WheelDist /2.0f;
		bPositive = tan_Theta> -0.00001f? true:false;
		switch (i)
		{
			case 0: //left rear tire
			case 1: //right rear tire
				ret = 0.0f;
				break;
			case 3: //right front tire
				Rb = (Rb+m_WheelDist);
				//deliberately let through
			case 2://left front tire
                ret = RAD2DEGREE(atan(amplify*m_AxleDist / Rb));
				break;
			default:
				break;
		}
	}while(0);
	value = ret;
	return bPositive;
}

void DynamicTrack::UpdateArcCenter()
{
	float r[3]={0.0f, 0.0f, -0.49f};
	bool isPositive = GetArcRadius(0, r[0]);
	if(isPositive)
	{
		r[0] = -1*r[0];
	}
	else{
		r[0] = r[0] + m_WheelDist;
	}
	r[1]= -1.0*DEFAULT_REAR_TIRE_OFFSET;
	SetAcrCenter(r);
}

void DynamicTrack::SetAngle(float angle)
{
	static float lastangle=0.0f,delta_angle=0.0f,last_m_angle=0.0f;

	float temp_angle=0.0f;
	static float last_temp_angle=0.0f;
	float amplify=1.0f;
	if(angle>=290.0)
	{
		temp_angle=angle-290.0;
	}
	else
	{
		temp_angle=angle+70.0;
	}

	delta_angle=temp_angle-last_temp_angle;//temp_angle-lastangle;

	if(delta_angle<-70.0f && last_temp_angle==140.0f)
	{
		last_temp_angle=temp_angle;
		temp_angle=140.0f;
	}
	else if(delta_angle>70.0f && last_m_angle==0.0f)
	{
		last_temp_angle=temp_angle;
		temp_angle=0.0f;
	}


	if(temp_angle>=70.0f)
	{
		m_angle=temp_angle-70.0f;
	}
	else
	{
		m_angle=temp_angle+290.0f;
	}

	if(m_angle>THE_MAX_ANGLE || m_angle<THE_MIN_ANGLE)
	{
		m_angle=THE_MAX_ANGLE;
		amplify=ANGLE_DEC;
	}
	UpdateArcCenter();
	UpdateArcs(m_arcWidth, DEFAULT_DELTA_ANGLE/amplify);

}

void DynamicTrack::RefreshAngle()
{
	SetAngle(m_angle);
}

