/*
 * ShaderParamArrays.cpp
 *
 *  Created on: Nov 7, 2016
 *      Author: hoover
 */

#include "ShaderParamArrays.h"


typedef struct{
	Attrib_vIdx attribIdx;
	const char *attribName;
} GLAttribElement;

typedef struct{
	Uniform_vIdx uniformIdx;
	const char *uniformName;
} GLUniformElement;

#define DECLARE_ATTRIB(a) {ATTRIB_##a, "##a##"}
#define DECLARE_UNIFORM(a) {UNIFORM_##a, "##a##"}

static GLAttribElement g_vAttributesArray[MAX_V_ATTRIB_COUNT]={
		DECLARE_ATTRIB(vVertex),
		DECLARE_ATTRIB(vTexCoords)
};
static GLUniformElement g_vUniformsArray[MAX_UNIFORM_COUNT]={
		DECLARE_UNIFORM(fIndex),
		DECLARE_UNIFORM(bowlImage)
};

const char* getAttribName(Attrib_vIdx i){
	return i>=MAX_V_ATTRIB_COUNT ? NULL : g_vAttributesArray[i].attribName;
}

const char* getUniformName(Uniform_vIdx i){
	return i>= MAX_UNIFORM_COUNT ? NULL : g_vUniformsArray[i].uniformName;
}
