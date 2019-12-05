#ifndef LINEOFRULER_H_
#define LINEOFRULER_H_


#include "StlGlDefines.h"
#include "GLShaderManager.h"
#include "GLMatrixStack.h"
#include "GLTriangleBatch.h"
#include"GLEnv.h"
class Calibrate{
public:
	Calibrate(GLMatrixStack &modelViewMat,GLMatrixStack	&projectionMat,GLShaderManager* mgr=NULL);
	~Calibrate(){};

	float Load();
	void Save(float value);
	void SetAngle(float value);
	void DrawRuler(GLEnv &m_env,float * pos);
	void DrawReferenceLine(GLEnv &m_env,float * pos);
	float GetAngle();
	void setRulerSpeed(float speed){rulerSpeed=speed;};
	float getRulerSpeed(){return rulerSpeed;};
private:
    void Init();
    float rulerSpeed;
	GLShaderManager * 	m_pShaderManager;
	GLMatrixStack &		modelViewMatrix;
	GLMatrixStack &		projectionMatrix;
	GLBatch p_RulerBatch;
	GLBatch p_ReferenceLineBatch;
	float angle;
};


#endif /* LINEOFRULER_H_ */
