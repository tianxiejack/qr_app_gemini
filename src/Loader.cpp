/*
 * Loader.cpp
 *
 *  Created on: Nov 7, 2016
 *      Author: hoover
 */

#include <GL/glew.h>
#include <cmath>
#include <iostream>
#include <stdio.h>
#include <string.h>

#include "LoadShader.h"
#include <GL/glext.h>
#include <fstream>
#include <vector>
#include <stdarg.h>
#include <stdlib.h>
#include "ShaderParamArrays.h"

static const GLchar *getFileData(const char * path);
static bool compileShader(GLuint &shader_id, const char *path);
static const int MAX_LOG_LEN = 16 * 1024;
using namespace std;

static void _log(const char *format, va_list args)
{
    char buf[MAX_LOG_LEN];
    vsnprintf(buf, MAX_LOG_LEN - 3, format, args);
    strcat(buf, "\n");
    printf("%s", buf);
    fflush(stdout);
}
void log( const char *format, ... )
{
    va_list args;
    va_start(args, format);
    _log(format, args);
    va_end(args);
}
GLuint LoadShader( const char *shader_path, GLenum shader_type )
{

    GLuint shader_id = glCreateShader(shader_type);
    if(shader_id == 0)
     {
         cerr << "Could not create OpenGL shader " << endl;
         return 0;
     }

    bool bRet = compileShader(shader_id, shader_path);
    if(!bRet)
    {
        glDeleteShader(shader_id);
        return 0;
    }

    return shader_id;
}

static const GLchar* getFileData( const char * path )
{
    FILE* infile;
    infile = fopen(path, "rb");
    if(!infile)
    {
    	cerr<<"FATAL: failed to open:"<<path<<endl;
        return NULL;
    }
    else{
    	cout<<"open OK:"<<path<<endl;
    }
    fseek(infile, 0, SEEK_END);
    int len = ftell(infile);
    fseek(infile, 0, SEEK_SET);

    GLchar *source = new GLchar[len + 1];
    fread(source, 1, len, infile);
    fclose(infile);
    source[len] = 0;
    return const_cast<const GLchar *>(source);
}

bool compileShader( GLuint &shader_id, const char *path )
{
    const GLchar *shader_code = getFileData(path);
    if(strlen(shader_code) <= 0 )
        return 0;

    GLint Result = GL_FALSE;
    GLint InfoLogLength;
    cout<<"============="<<path<<"================="<<endl<<shader_code<<endl<<"----------eof-----------"<<endl;
    glShaderSource(shader_id, 1, &shader_code, NULL);
    glCompileShader(shader_id);
    delete []shader_code;

    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &Result);
    if ( !Result ){
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &InfoLogLength);
        std::vector<char> ShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(shader_id, InfoLogLength, NULL, &ShaderErrorMessage[0]);
        printf("%s\n", &ShaderErrorMessage[0]);
        //log("%s", &ShaderErrorMessage[0]);
        return false;
    }

    return true;
}



bool LoadTessellationShader( GLuint shader_program, const char *tes_shader_path )
{
    GLuint shader = glCreateShader(GL_TESS_CONTROL_SHADER);
    bool bRet = compileShader(shader, tes_shader_path);
    if(!bRet)
        return false;
    glAttachShader(shader_program, shader);
    glLinkProgram(shader_program);

    glDeleteProgram(shader);

    return true;
}

void LinkShader( GLuint shader_pragram, ... )
{
	GLint linked;
	GLuint programObject = shader_pragram;

    va_list args;
    va_start(args, shader_pragram);
    while (true)
    {
        GLuint shader_id = va_arg(args, GLuint);
        if(shader_id <= 0)
            break;
        glAttachShader(shader_pragram, shader_id);
        glDeleteShader(shader_id);
    }
    va_end(args);

    for(int i = 0; i < MAX_V_ATTRIB_COUNT; i++){
    	glBindAttribLocation( shader_pragram,i,getAttribName((Attrib_vIdx)i));
    }
    glLinkProgram(shader_pragram);

    glGetProgramiv (programObject, GL_LINK_STATUS, &linked);

        if (!linked) {

            GLint infoLen = 0;

            glGetProgramiv (programObject, GL_INFO_LOG_LENGTH, &infoLen);

            if (infoLen > 1) {

                char* infoLog = (char*) malloc (sizeof (char) * infoLen);
                glGetProgramInfoLog (programObject, infoLen, NULL, infoLog);
                cerr << "Error linking program: " << infoLog << endl;
                free (infoLog);

            }

            glDeleteProgram (programObject);
            exit(1);

        }
        cout<<"Shaders Linked OK!"<<endl;
}

GLuint CreateShaderProgram()
{
    return glCreateProgram();
}

void initGlew()
{
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
	  /* Problem: glewInit failed, something is seriously wrong. */
	  cerr<<"glewInit failed"<<endl;
	  exit(1);
	}
}

