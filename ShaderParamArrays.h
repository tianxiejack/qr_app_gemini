/*
 * ShaderParamArrays.h
 *
 *  Created on: Nov 7, 2016
 *      Author: hoover
 */

#ifndef SHADERPARAMARRAYS_H_
#define SHADERPARAMARRAYS_H_

#include <GL/gl.h>

typedef enum{
	ATTRIB_vVertex,
	ATTRIB_vTexCoords,
	MAX_V_ATTRIB_COUNT
} Attrib_vIdx;

typedef enum{
	UNIFORM_fIndex,
	UNIFORM_bowlImage,
	MAX_UNIFORM_COUNT
} Uniform_vIdx;

extern const char* getAttribName(Attrib_vIdx i);
extern const char* getUniformName(Uniform_vIdx i);


#endif /* SHADERPARAMARRAYS_H_ */
