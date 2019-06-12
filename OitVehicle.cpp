/*
 * OitVehicle.cpp
 *
 *  Created on: Nov 20, 2016
 *      Author: hoover
 */

#include "OitVehicle.h"
#include "GLRender.h"
#include "GLShaderManager.h"
#include "GLBatch.h"
#include "STLASCIILoader.h"
#include "GL/gl.h"
#include <iostream>
#include <GLGeometryTransform.h>
#include "glm.h"
#include "StlGlDefines.h"
Render render;
using namespace std;

// Load a TGA as a 2D Texture. Completely initialize the state
bool OitVehicle::LoadTGATexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode)
{
	GLbyte *pBits;
	int nWidth, nHeight, nComponents;
	GLenum eFormat;
	
	// Read the texture bits
	pBits = gltReadTGABits(szFileName, &nWidth, &nHeight, &nComponents, &eFormat);
	if(pBits == NULL) 
		return false;
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, nComponents, nWidth, nHeight, 0,
				 eFormat, GL_UNSIGNED_BYTE, pBits);
	
    free(pBits);
    
    if(minFilter == GL_LINEAR_MIPMAP_LINEAR || 
       minFilter == GL_LINEAR_MIPMAP_NEAREST ||
       minFilter == GL_NEAREST_MIPMAP_LINEAR ||
       minFilter == GL_NEAREST_MIPMAP_NEAREST)
        glGenerateMipmap(GL_TEXTURE_2D);
    
	return true;
}

OitVehicle::OitVehicle(GLMatrixStack &modelViewMat,GLMatrixStack	&projectionMat,GLShaderManager* mgr):
	m_pShaderManager(mgr),modelViewMatrix(modelViewMat),projectionMatrix(projectionMat),
	pVehicleLoader(NULL),mode(USER_BLEND),blendmode(5),msFBO(0),vehicleTexture(0),
	ADSTextureShader(0),locAmbient(0),locDiffuse(0),locSpecular(0),locLight(0),locMVP(0),locMV(0),
	locNM(0),locTexture(0),msResolve(0),oitResolve(0),depthTextureName(0),m_screenWidth(0),m_screenHeight(0),
	m_vehicleScale(DEFAULT_VEHICLE_SCALE)
{
	if(NULL == m_pShaderManager){
		m_pShaderManager = (GLShaderManager*)getDefaultShaderMgr();
	}
	GLfloat t[3]= {DEFAULT_VEHICLE_TRANSLATION_0,DEFAULT_VEHICLE_TRANSLATION_1, DEFAULT_VEHICLE_TRANSLATION_2};
	SetTranslate(t);
	SetTankDis(render.getTankDistance());
}

void OitVehicle::DrawVehicle(GLEnv &m_env)
{
	GLGeometryTransform * pTransformPipeline = (GLGeometryTransform *)getDefaultTransformPipeline(m_env);
    if (mode == USER_BLEND)
        {
            // Setup blend state
	   glDisable(GL_DEPTH_TEST);
          glEnable(GL_BLEND);
          Render::SwitchBlendMode(blendmode);
        }

	    modelViewMatrix.PushMatrix();
	    glEnable(GL_CULL_FACE);
	    modelViewMatrix.Translate(m_vehicleTranslation[0], m_vehicleTranslation[1], m_vehicleTranslation[2]);
	    modelViewMatrix.Scale(m_vehicleScale, DEFAULT_VEHICLE_SCALE_HEIGHT, m_vehicleScale);
	     
		
		static const GLfloat vEyeLight[] = { -100.0f, 100.0f, 100.0f };
		static const GLfloat vAmbientColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
		static const GLfloat vDiffuseColor[] = { 0.1f, 0.0f, 0.0f, 1.0f };
		static const GLfloat vSpecularColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		GLMmaterial* material;
		GLMmodel* model = pVehicleLoader;
		GLMgroup* group = pVehicleLoader->groups;
		bool hasMaterial = false;
		glActiveTexture(GL_TEXTURE20);
		glBindTexture(GL_TEXTURE_2D, vehicleTexture);
		glUseProgram(ADSTextureShader);
        if(model->materials){// use the first group only.
        	material = &model->materials[group->material];
        	if(material){
        		hasMaterial = true;
        	}
        }

        if(hasMaterial){
			glUniform4fv(locAmbient, 1, material->ambient);
			glUniform4fv(locDiffuse, 1, material->diffuse);
			glUniform4fv(locSpecular, 1, material->specular);
        }else{//use default material
			glUniform4fv(locAmbient, 1, vAmbientColor);
			glUniform4fv(locDiffuse, 1, vDiffuseColor);
			glUniform4fv(locSpecular, 1,vSpecularColor);
        }

		glUniform3fv(locLight, 1, vEyeLight);

		glUniformMatrix4fv(locMVP, 1, GL_FALSE, pTransformPipeline->GetModelViewProjectionMatrix());
		glUniformMatrix4fv(locMV, 1, GL_FALSE, pTransformPipeline->GetModelViewMatrix());
		glUniformMatrix3fv(locNM, 1, GL_FALSE, pTransformPipeline->GetNormalMatrix());
		glUniform1i(locTexture, 20);

	    m_vehicle.Draw();

        glDisable(GL_CULL_FACE);
	    modelViewMatrix.PopMatrix();

}

void OitVehicle::DrawVehicle_second(GLEnv &m_env)
{
	GLGeometryTransform * pTransformPipeline = (GLGeometryTransform *)getDefaultTransformPipeline(m_env);
    if (mode == USER_BLEND)
        {
            // Setup blend state
	   glDisable(GL_DEPTH_TEST);
          glEnable(GL_BLEND);
          Render::SwitchBlendMode(blendmode);
        }

	    modelViewMatrix.PushMatrix();
	    glEnable(GL_CULL_FACE);
	    modelViewMatrix.Translate(m_vehicleTranslation[0]-6.0, m_vehicleTranslation[3], m_vehicleTranslation[2]);
	    modelViewMatrix.Scale(m_vehicleScale, DEFAULT_VEHICLE_SCALE_HEIGHT, m_vehicleScale);


		static const GLfloat vEyeLight[] = { -100.0f, 100.0f, 100.0f };
		static const GLfloat vAmbientColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
		static const GLfloat vDiffuseColor[] = { 0.1f, 0.0f, 0.0f, 1.0f };
		static const GLfloat vSpecularColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		GLMmaterial* material;
		GLMmodel* model = pVehicleLoader;
		GLMgroup* group = pVehicleLoader->groups;
		bool hasMaterial = false;
		glActiveTexture(GL_TEXTURE20);
		glBindTexture(GL_TEXTURE_2D, vehicleTexture);
		glUseProgram(ADSTextureShader);
        if(model->materials){// use the first group only.
        	material = &model->materials[group->material];
        	if(material){
        		hasMaterial = true;
        	}
        }

        if(hasMaterial){
			glUniform4fv(locAmbient, 1, material->ambient);
			glUniform4fv(locDiffuse, 1, material->diffuse);
			glUniform4fv(locSpecular, 1, material->specular);
        }else{//use default material
			glUniform4fv(locAmbient, 1, vAmbientColor);
			glUniform4fv(locDiffuse, 1, vDiffuseColor);
			glUniform4fv(locSpecular, 1,vSpecularColor);
        }

		glUniform3fv(locLight, 1, vEyeLight);

		glUniformMatrix4fv(locMVP, 1, GL_FALSE, pTransformPipeline->GetModelViewProjectionMatrix());
		glUniformMatrix4fv(locMV, 1, GL_FALSE, pTransformPipeline->GetModelViewMatrix());
		glUniformMatrix3fv(locNM, 1, GL_FALSE, pTransformPipeline->GetNormalMatrix());
		glUniform1i(locTexture, 20);

	    m_vehicle.Draw();

        glDisable(GL_CULL_FACE);
	    modelViewMatrix.PopMatrix();


}

void OitVehicle::DrawVehicle_third(GLEnv &m_env)
{
	GLGeometryTransform * pTransformPipeline = (GLGeometryTransform *)getDefaultTransformPipeline(m_env);
    if (mode == USER_BLEND)
        {
            // Setup blend state
	   glDisable(GL_DEPTH_TEST);
          glEnable(GL_BLEND);
          Render::SwitchBlendMode(blendmode);
        }

	    modelViewMatrix.PushMatrix();
	    glEnable(GL_CULL_FACE);
	    modelViewMatrix.Translate(m_vehicleTranslation[0]+6.0, m_vehicleTranslation[4], m_vehicleTranslation[2]);
	    modelViewMatrix.Scale(m_vehicleScale, DEFAULT_VEHICLE_SCALE_HEIGHT, m_vehicleScale);


		static const GLfloat vEyeLight[] = { -100.0f, 100.0f, 100.0f };
		static const GLfloat vAmbientColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
		static const GLfloat vDiffuseColor[] = { 0.1f, 0.0f, 0.0f, 1.0f };
		static const GLfloat vSpecularColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		GLMmaterial* material;
		GLMmodel* model = pVehicleLoader;
		GLMgroup* group = pVehicleLoader->groups;
		bool hasMaterial = false;
		glActiveTexture(GL_TEXTURE20);
		glBindTexture(GL_TEXTURE_2D, vehicleTexture);
		glUseProgram(ADSTextureShader);
        if(model->materials){// use the first group only.
        	material = &model->materials[group->material];
        	if(material){
        		hasMaterial = true;
        	}
        }

        if(hasMaterial){
			glUniform4fv(locAmbient, 1, material->ambient);
			glUniform4fv(locDiffuse, 1, material->diffuse);
			glUniform4fv(locSpecular, 1, material->specular);
        }else{//use default material
			glUniform4fv(locAmbient, 1, vAmbientColor);
			glUniform4fv(locDiffuse, 1, vDiffuseColor);
			glUniform4fv(locSpecular, 1,vSpecularColor);
        }

		glUniform3fv(locLight, 1, vEyeLight);

		glUniformMatrix4fv(locMVP, 1, GL_FALSE, pTransformPipeline->GetModelViewProjectionMatrix());
		glUniformMatrix4fv(locMV, 1, GL_FALSE, pTransformPipeline->GetModelViewMatrix());
		glUniformMatrix3fv(locNM, 1, GL_FALSE, pTransformPipeline->GetNormalMatrix());
		glUniform1i(locTexture, 20);

	    m_vehicle.Draw();

        glDisable(GL_CULL_FACE);
	    modelViewMatrix.PopMatrix();

}

void OitVehicle::PrepareBlendMode()
{
	// User selected order independant transparency
    if (mode == USER_OIT)
    {
        // Use OIT, setup sample masks
        glSampleMaski(0, 0x01);
        glEnable(GL_SAMPLE_MASK);

		// Prevent depth test from culling covered surfaces
        glDepthFunc(GL_ALWAYS);
    }
}
void OitVehicle::InitVehicle()
{
#define T(x) (pVehicleLoader->triangles[(x)])

	static GLMgroup* group = pVehicleLoader->groups;
	int poly_count = pVehicleLoader->numtriangles;
	static GLMtriangle* triangle;
	GLTriangleBatch *pBatch = &m_vehicle;
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	M3DVector3f vVertex[3];
	M3DVector3f vNormal[3];
	M3DVector2f vTexture[3];
	cout<<"InitVehicle start: "<<poly_count<<endl;
	pBatch->BeginMesh(poly_count*3);
	int vertexCount = pVehicleLoader->numvertices;
	int TextureCount = pVehicleLoader->numtexcoords;
	while(group){
		poly_count = group->numtriangles;
		for(int x = 0 ; x < poly_count ; x++)
		  {	
		 	triangle = &T(group->triangles[x]);

			if(pVehicleLoader->normals){
				//normal mapping
				for(int i=0; i<sizeof(vNormal)/sizeof(M3DVector3f); i++)
				{
					for(int j=0; j<sizeof(M3DVector3f)/sizeof(float); j++)
						vNormal[i][j] = pVehicleLoader->normals[j + 3*triangle->nindices[i]];
				}
				//cout<<"x="<<x<<",normal ok"<<endl;
			}
	//vertex Mapping
			if(pVehicleLoader->vertices){
				for(int i=0; i<sizeof(vVertex)/sizeof(M3DVector3f); i++)
				{
					for(int j=0; j<sizeof(M3DVector3f)/sizeof(float); j++)
						vVertex[i][j] = pVehicleLoader->vertices[j + 3*triangle->vindices[i]];
				}
				//cout<<"x="<<x<<",vertex ok"<<endl;
			}
			
	//texture mapping
			if(pVehicleLoader->texcoords){
				for(int i=0; i<sizeof(vTexture)/sizeof(M3DVector2f); i++)
				{
					for(int j=0; j<sizeof(M3DVector2f)/sizeof(float); j++)
						vTexture[i][j] = pVehicleLoader->texcoords[j + 2*triangle->tindices[i]];
				}
				//cout<<"x="<<x<<",texture ok"<<endl;
			}
			pBatch->AddTriangle(vVertex, vNormal, vTexture);
		  }
	    group = group->next;
	}
	pBatch->End();
	cout<<"InitVehicle done"<<endl;
	initDimensions();
}

void OitVehicle::initDimensions()
{
	glmDimensions(pVehicleLoader, m_dimensions,m_yMaxMins);
}

void OitVehicle::InitShaders()
{
	ADSTextureShader = m_pShaderManager->LoadShaderPairWithAttributes("ADSTexture.vp", "ADSTexture.fp", 3, GLT_ATTRIBUTE_VERTEX, "vVertex",
			GLT_ATTRIBUTE_NORMAL, "vNormal",GLT_ATTRIBUTE_TEXTURE0,"vTexture0");
	
	locAmbient = glGetUniformLocation(ADSTextureShader, "ambientColor");
	locDiffuse = glGetUniformLocation(ADSTextureShader, "diffuseColor");
	locSpecular = glGetUniformLocation(ADSTextureShader, "specularColor");
	locLight = glGetUniformLocation(ADSTextureShader, "vLightPosition");
	locMVP = glGetUniformLocation(ADSTextureShader, "mvpMatrix");
	locMV  = glGetUniformLocation(ADSTextureShader, "mvMatrix");
	locNM  = glGetUniformLocation(ADSTextureShader, "normalMatrix");
	locTexture = glGetUniformLocation(ADSTextureShader, "colorMap");

	glGenTextures(1, &vehicleTexture);
	glBindTexture(GL_TEXTURE_2D, vehicleTexture);
	if(!LoadTGATexture(pVehicleLoader->texturename, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE)){
		cout<<"failed to load vehicle texture"<<pVehicleLoader->texturename<<endl;
	}
}

void OitVehicle::initFBOs(int screenWidth, int screenHeight)
{

	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
    // Create depth texture
    glGenTextures(1, &depthTextureName);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, depthTextureName);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_DEPTH_COMPONENT24, screenWidth, screenHeight, GL_FALSE);
             
    // Setup HDR render texture
    glGenTextures(1, msTexture);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, msTexture[0]);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGBA8, screenWidth, screenHeight, GL_FALSE);
    
    // Create and bind an FBO
    glGenFramebuffers(1, &msFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, msFBO);

    // Attach texture to first color attachment and the depth RBO
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, msTexture[0], 0);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, depthTextureName, 0);

    // Reset framebuffer binding
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    // Load oit resolve shader
    oitResolve =  gltLoadShaderPairWithAttributes("basic.vs", "oitResolve.fs", 3, 
                            GLT_ATTRIBUTE_VERTEX, "vVertex", 
                            GLT_ATTRIBUTE_NORMAL, "vNormal", 
                            GLT_ATTRIBUTE_TEXTURE0, "vTexCoord0");
    glBindFragDataLocation(oitResolve, 0, "oColor");
    glLinkProgram(oitResolve);

	// Load multisample resolve shader
    msResolve =  gltLoadShaderPairWithAttributes("basic.vs", "msResolve.fs", 3,
                            GLT_ATTRIBUTE_VERTEX, "vVertex", 
                            GLT_ATTRIBUTE_NORMAL, "vNormal", 
                            GLT_ATTRIBUTE_TEXTURE0, "vTexCoord0");

    glBindFragDataLocation(msResolve, 0, "oColor");
    glLinkProgram(msResolve);

    // Make sure all went well
    gltCheckErrors(oitResolve);
    gltCheckErrors(msResolve);
    
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Create a matrix that maps geometry to the screen. 1 unit in the x directionequals one pixel
// of width, same with the y direction.
//
void OitVehicle::GenerateOrtho2DMat(GLuint imageWidth, GLuint imageHeight)
{
    float right = (float)imageWidth;
    float quadWidth = right;
    float left  = 0.0f;
    float top = (float)imageHeight;
    float quadHeight = top;
    float bottom = 0.0f;

	// set ortho matrix
	for(int i=0; i<sizeof(orthoMatrix)/sizeof(orthoMatrix[0]); i++)
		orthoMatrix[i] = 0.0;

	orthoMatrix[0] = (float)(2 / (right));
	orthoMatrix[5] = (float)(2 / (top));
	orthoMatrix[10] = (float)(-2 / (1.0 - 0.0));

    orthoMatrix[12] = -1.0f;
    orthoMatrix[13] = -1.0f;
    orthoMatrix[14] = -1.0f;
    orthoMatrix[15] =  1.0;

    // set screen quad vertex array
    screenQuad.Reset();
    screenQuad.Begin(GL_TRIANGLE_STRIP, 4, 1);
        screenQuad.Color4f(0.0f, 1.0f, 0.0f, 1.0f);
        screenQuad.MultiTexCoord2f(0, 0.0f, 0.0f);
        screenQuad.Vertex3f(0.0f, 0.0f, 0.0f);

        screenQuad.Color4f(0.0f, 1.0f, 0.0f, 1.0f);
        screenQuad.MultiTexCoord2f(0, 1.0f, 0.0f);
        screenQuad.Vertex3f(right, 0.0f, 0.0f);

        screenQuad.Color4f(0.0f, 1.0f, 0.0f, 1.0f);
        screenQuad.MultiTexCoord2f(0, 0.0f, 1.0f);
        screenQuad.Vertex3f(0.0f, top, 0.0f);

        screenQuad.Color4f(0.0f, 1.0f, 0.0f, 1.0f);
        screenQuad.MultiTexCoord2f(0, 1.0f, 1.0f);
        screenQuad.Vertex3f(right, top, 0.0f);
    screenQuad.End();
}


void OitVehicle::SetupResolveProg(GLEnv &m_env)
{
	GLGeometryTransform * pTransformPipeline = (GLGeometryTransform *)getDefaultTransformPipeline(m_env);
    glUseProgram(msResolve);

    // Set projection matrix
    glUniformMatrix4fv(glGetUniformLocation(msResolve, "pMatrix"),
        1, GL_FALSE, pTransformPipeline->GetProjectionMatrix());

    // Set MVP matrix
    glUniformMatrix4fv(glGetUniformLocation(msResolve, "mvMatrix"),
        1, GL_FALSE, pTransformPipeline->GetModelViewMatrix());

    // Now setup the right textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, msTexture[0]);
    glUniform1i(glGetUniformLocation(msResolve, "origImage"), 0);

    glUniform1i(glGetUniformLocation(msResolve, "sampleCount"), 8);

    glActiveTexture(GL_TEXTURE0);

	gltCheckErrors(msResolve);
}

void OitVehicle::SetupOITResolveProg(GLEnv &m_env)
{
	GLGeometryTransform * pTransformPipeline = (GLGeometryTransform *)getDefaultTransformPipeline(m_env);
    glUseProgram(oitResolve);

    // Set projection matrix
    glUniformMatrix4fv(glGetUniformLocation(oitResolve, "pMatrix"),
        1, GL_FALSE, pTransformPipeline->GetProjectionMatrix());

    // Set MVP matrix
    glUniformMatrix4fv(glGetUniformLocation(oitResolve, "mvMatrix"),
        1, GL_FALSE, pTransformPipeline->GetModelViewMatrix());

    // Now setup the right textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, msTexture[0]);
    glUniform1i(glGetUniformLocation(oitResolve, "origImage"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, depthTextureName);
    glUniform1i(glGetUniformLocation(oitResolve, "origDepth"), 1);

    glUniform1f(glGetUniformLocation(oitResolve, "sampleCount"), 8);

    glActiveTexture(GL_TEXTURE0);
    gltCheckErrors(oitResolve);
}

void OitVehicle::ChangeSize(int screenWidth, int screenHeight)
{
   GenerateOrtho2DMat(screenWidth, screenHeight);

    // Resize textures
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, depthTextureName);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_DEPTH_COMPONENT24, screenWidth, screenHeight, GL_FALSE);

    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, msTexture[0]);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGBA8, screenWidth, screenHeight, GL_FALSE);

    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;
}

void OitVehicle::updateFBOs(GLEnv &m_env)
{
    // Clean up all state 
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_BLEND);
    glDisable(GL_SAMPLE_MASK);
    glSampleMaski(0, 0xffffffff);

    // Resolve multisample buffer
    projectionMatrix.PushMatrix();
      projectionMatrix.LoadMatrix(orthoMatrix);
      modelViewMatrix.PushMatrix();
        modelViewMatrix.LoadIdentity();
		// Setup and Clear the default framebuffer
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glViewport(0, 0, m_screenWidth, m_screenHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        if (mode == USER_OIT)
            SetupOITResolveProg(m_env);
        else if (mode == USER_BLEND)
            SetupResolveProg(m_env);

		// Draw a full-size quad to resolve the multisample surfaces
        screenQuad.Draw();
      modelViewMatrix.PopMatrix();
    projectionMatrix.PopMatrix();
    
	// Reset texture state
    glEnable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
}
