/*
 * LoadShader.h
 *
 *  Created on: Nov 7, 2016
 *      Author: hoover
 */

#ifndef LOADSHADER_H_
#define LOADSHADER_H_
#include <GL/gl.h>

void initGlew();
GLuint CreateShaderProgram();
GLuint LoadShader(const char *shader_path, GLenum shader_type);
void LinkShader(GLuint shader_pragram, ...);  /// 以0结束


#endif /* LOADSHADER_H_ */
