/*
 * DynamicTrack.cpp
 *
 *  Created on: Apr 6, 2017
 *      Author: hoover
 */

#include "LineofRuler.h"
#include "GLGeometryTransform.h"
#include <iostream>
#include "GLTools.h"
#include <cmath>
extern void* getDefaultShaderMgr();
extern void * getDefaultTransformPipeline(GLEnv &m_env);
using namespace std;

#define RULER_FILE "./rulerangle.yml"

Calibrate::Calibrate(GLMatrixStack &modelViewMat,GLMatrixStack	&projectionMat,GLShaderManager* mgr)
					:m_pShaderManager(mgr),
					 modelViewMatrix(modelViewMat),projectionMatrix(projectionMat),angle(0.0)
{
	rulerSpeed=0.0f;
		if(NULL == m_pShaderManager){
			m_pShaderManager = (GLShaderManager*)getDefaultShaderMgr();
		}
		Init();
}

void Calibrate::Init()
{

    //GLuint &base = startList;

}
void Calibrate::DrawRuler(GLEnv &m_env,float * pos)
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
//	modelViewMatrix.Translate(m_center[0]- m_WheelDist/2, m_center[1], m_center[2]);
	GLfloat vTracks[50][3];
	GLfloat fixBBDPos[3];
	GLfloat Track_to_Vehicle_width_rate = DEFAULT_TRACK2_VEHICLE_WIDTH_RATE;
	GLfloat   TrackLength = DEFAULT_TRACK_LENGTH_METER;
	int i = 0,j=0;

	float cross_width=0.0,cross_height=0.0,margin_width=0.0,margin_height=0.0;

//right front
	vTracks[i][0] =pos[j++];
	vTracks[i][1] =pos[j++];
	vTracks[i++][2] =pos[j++];

	vTracks[i][0] = pos[j++];
	vTracks[i][1] = pos[j++];
	vTracks[i++][2] = pos[j++];

//right rear
	vTracks[i][0] =pos[j++];
	vTracks[i][1] =pos[j++];
	vTracks[i++][2] =pos[j++];

	vTracks[i][0] = pos[j++];
	vTracks[i][1] =pos[j++];
	vTracks[i++][2] =pos[j++];

//left_front
	vTracks[i][0] =pos[j++];
	vTracks[i][1] =pos[j++];
	vTracks[i++][2] =pos[j++];

	vTracks[i][0] =pos[j++];
	vTracks[i][1] =pos[j++];
	vTracks[i++][2] =pos[j++];

//left rear
	vTracks[i][0] =pos[j++];
	vTracks[i][1] =pos[j++];
	vTracks[i++][2] = pos[j++];

	vTracks[i][0] = pos[j++];
	vTracks[i][1] =pos[j++];
	vTracks[i++][2] = pos[j++];

	//add left_front
		vTracks[i][0] =pos[j++];
		vTracks[i][1] =pos[j++];
		vTracks[i++][2] =pos[j++];

		vTracks[i][0] =pos[j++];
		vTracks[i][1] =pos[j++];
		vTracks[i++][2] =pos[j++];

	//add left rear
		vTracks[i][0] =pos[j++];
		vTracks[i][1] =pos[j++];
		vTracks[i++][2] = pos[j++];

		vTracks[i][0] = pos[j++];
		vTracks[i][1] =pos[j++];
		vTracks[i++][2] = pos[j++];

	p_RulerBatch.Begin(GL_LINES, i);
	p_RulerBatch.CopyVertexData3f(vTracks);
	p_RulerBatch.End();

//	glDisable(GL_BLEND);
//	shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vLtYellow);
	glLineWidth(2.0f);

	/*for(int i = 0; i < 4; i++)*/
	{
		m_pShaderManager->UseStockShader(GLT_SHADER_TEXTURE_MODULATE, pTransformPipeline->GetModelViewProjectionMatrix(), Colors[4], CAM_COUNT+1);

		p_RulerBatch.Draw();
	}
	modelViewMatrix.PopMatrix();
}

float Calibrate::Load()
{
	float angle_data=0.0;
	FILE * fp=fopen(RULER_FILE,"r");
	if(fp!=NULL)
	{
		fscanf(fp,"%f\n",&angle_data);
		fclose(fp);
	}
	return angle_data;
}

void Calibrate::Save(float value)
{
	FILE * fp=fopen(RULER_FILE,"w");
	char writedata[512];
	if(value<0)
	{
		for(;;)
		{
			if(value>0.0)
			{
				break;
			}
			value=value+360.0;
		}
	}
	else
	{
		for(;;)
		{
			if(value<360.0)
			{
				break;
			}
			value=value-360.0;
		}
	}
	if(fp!=NULL)
	{
		sprintf(writedata,"%f\n",value);
		fwrite(writedata,strlen(writedata),1,fp);
		fclose(fp);
	}
}

void Calibrate::SetAngle(float value)
{
	if(value<0)
	{
		for(;;)
		{
			if(value>0.0)
			{
				break;
			}
			value=value+360.0;
		}
	}
	else
	{
		for(;;)
		{
			if(value<360.0)
			{
				break;
			}
			value=value-360.0;
		}
	}
	angle=value;
}

float Calibrate::GetAngle()
{
	float angle_data=0.0;
	angle_data=angle;
	if(angle_data<0)
	{
		for(;;)
		{
			if(angle_data>0.0)
			{
				break;
			}
			angle_data=angle_data+360.0;
		}
	}
	else
	{
		for(;;)
		{
			if(angle_data<360.0)
			{
				break;
			}
			angle_data=angle_data-360.0;
		}
	}

	return angle_data;
}

void Calibrate::DrawReferenceLine(GLEnv &m_env,float * pos)
{
	GLGeometryTransform * pTransformPipeline = (GLGeometryTransform *)getDefaultTransformPipeline(m_env);
	const float* Colors[6];
	Colors[0]=vBRed;
	Colors[1]=vYellow;
	Colors[2]=vLtBlue;
	Colors[3]=vGreen;
	Colors[4]=vWhite;
	Colors[5]=vLtGrey;
	glEnable(GL_BLEND);
	modelViewMatrix.PushMatrix();
	GLfloat vTracks[50][3];
	GLfloat fixBBDPos[3];
	GLfloat Track_to_Vehicle_width_rate = DEFAULT_TRACK2_VEHICLE_WIDTH_RATE;
	GLfloat   TrackLength = DEFAULT_TRACK_LENGTH_METER;
	int i = 0,j=0;

	float cross_width=0.0,cross_height=0.0,margin_width=0.0,margin_height=0.0;

//right front
	vTracks[i][0] =pos[j++];
	vTracks[i][1] =pos[j++];
	vTracks[i++][2] =pos[j++];

	vTracks[i][0] = pos[j++];
	vTracks[i][1] = pos[j++];
	vTracks[i++][2] = pos[j++];

//right rear
	vTracks[i][0] =pos[j++];
	vTracks[i][1] =pos[j++];
	vTracks[i++][2] =pos[j++];

	vTracks[i][0] = pos[j++];
	vTracks[i][1] =pos[j++];
	vTracks[i++][2] =pos[j++];

//left_front
	vTracks[i][0] =pos[j++];
	vTracks[i][1] =pos[j++];
	vTracks[i++][2] =pos[j++];

	vTracks[i][0] =pos[j++];
	vTracks[i][1] =pos[j++];
	vTracks[i++][2] =pos[j++];

//left rear
	vTracks[i][0] =pos[j++];
	vTracks[i][1] =pos[j++];
	vTracks[i++][2] = pos[j++];

	vTracks[i][0] = pos[j++];
	vTracks[i][1] =pos[j++];
	vTracks[i++][2] = pos[j++];

	//add left_front
		vTracks[i][0] =pos[j++];
		vTracks[i][1] =pos[j++];
		vTracks[i++][2] =pos[j++];

		vTracks[i][0] =pos[j++];
		vTracks[i][1] =pos[j++];
		vTracks[i++][2] =pos[j++];

	//add left rear
		vTracks[i][0] =pos[j++];
		vTracks[i][1] =pos[j++];
		vTracks[i++][2] = pos[j++];

		vTracks[i][0] = pos[j++];
		vTracks[i][1] =pos[j++];
		vTracks[i++][2] = pos[j++];

		p_ReferenceLineBatch.Begin(GL_LINES, i);
		p_ReferenceLineBatch.CopyVertexData3f(vTracks);
		p_ReferenceLineBatch.End();

//	glDisable(GL_BLEND);
//	shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vLtYellow);
	glLineWidth(2.0f);

	/*for(int i = 0; i < 4; i++)*/
	{
		m_pShaderManager->UseStockShader(GLT_SHADER_FLAT, pTransformPipeline->GetModelViewProjectionMatrix(), Colors[0], CAM_COUNT+1);

		p_ReferenceLineBatch.Draw();
	}

	modelViewMatrix.PopMatrix();
}
