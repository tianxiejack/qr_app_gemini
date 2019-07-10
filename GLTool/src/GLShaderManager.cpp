/* GLShaderManager.h

Copyright (c) 2009, Richard S. Wright Jr.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list 
of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this list 
of conditions and the following disclaimer in the documentation and/or other 
materials provided with the distribution.

Neither the name of Richard S. Wright Jr. nor the names of other contributors may be used 
to endorse or promote products derived from this software without specific prior 
written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <GLShaderManager.h>
#include <GLTools.h>
#include <stdarg.h>
#if USE_GAIN
#include <math.h>
#include"../../GLRender.h"
#include"../../overLapRegion.h"
#endif
///////////////////////////////////////////////////////////////////////////////
// Stock Shader Source Code
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Identity Shader (GLT_SHADER_IDENTITY)
// This shader does no transformations at all, and uses the current
// glColor value for fragments.
// It will shade between verticies.
static const char *szIdentityShaderVP = "attribute vec4 vVertex;"
										"void main(void) "
										"{ gl_Position = vVertex; "
										"}";
									
static const char *szIdentityShaderFP = 
#ifdef OPENGL_ES
										"precision mediump float;"
#endif
										"uniform vec4 vColor;"
										"void main(void) "
										"{ gl_FragColor = vColor;"
										"}";
									

///////////////////////////////////////////////////////////////////////////////
// Flat Shader (GLT_SHADER_FLAT)
// This shader applies the given model view matrix to the verticies, 
// and uses a uniform color value.
static const char *szFlatShaderVP =	"uniform mat4 mvpMatrix;"
									"attribute vec4 vVertex;"
									"void main(void) "
									"{ gl_Position = mvpMatrix * vVertex; "
									"}";
									
static const char *szFlatShaderFP = 
#ifdef OPENGL_ES
									"precision mediump float;"
#endif
									"uniform vec4 vColor;"
									"void main(void) "
									"{ gl_FragColor = vColor; "
									"}";

///////////////////////////////////////////////////////////////////////////////
// GLT_SHADER_SHADED
// Point light, diffuse lighting only
static const char *szShadedVP =		"uniform mat4 mvpMatrix;"
									"attribute vec4 vColor;"
									"attribute vec4 vVertex;"
									"varying vec4 vFragColor;"
									"void main(void) {"
									"vFragColor = vColor; "
									" gl_Position = mvpMatrix * vVertex; "
									"}";

static const char *szShadedFP =     
#ifdef OPENGL_ES
									"precision mediump float;"
#endif
									"varying vec4 vFragColor; "
									"void main(void) { "
									" gl_FragColor = vFragColor; "
									"}";
									
// GLT_SHADER_DEFAULT_LIGHT
// Simple diffuse, directional, and vertex based light
static const char *szDefaultLightVP = "uniform mat4 mvMatrix;"
									  "uniform mat4 pMatrix;"
									  "varying vec4 vFragColor;"
									  "attribute vec4 vVertex;"
									  "attribute vec3 vNormal;"
									  "uniform vec4 vColor;"
									  "void main(void) { "
									  " mat3 mNormalMatrix;"
									  " mNormalMatrix[0] = mvMatrix[0].xyz;"
									  " mNormalMatrix[1] = mvMatrix[1].xyz;"
									  " mNormalMatrix[2] = mvMatrix[2].xyz;"
									  " vec3 vNorm = normalize(mNormalMatrix * vNormal);"
									  " vec3 vLightDir = vec3(0.0, 0.0, 1.0); "
									  " float fDot = max(0.0, dot(vNorm, vLightDir)); "
									  " vFragColor.rgb = vColor.rgb * fDot;"
									  " vFragColor.a = vColor.a;"
									  " mat4 mvpMatrix;"
									  " mvpMatrix = pMatrix * mvMatrix;"
									  " gl_Position = mvpMatrix * vVertex; "
									  "}";


static const char *szDefaultLightFP =	
#ifdef OPENGL_ES
										"precision mediump float;"
#endif
										"varying vec4 vFragColor; "
										"void main(void) { "
										" gl_FragColor = vFragColor; "
										"}";

//GLT_SHADER_POINT_LIGHT_DIFF
// Point light, diffuse lighting only
static const char *szPointLightDiffVP =	  "uniform mat4 mvMatrix;"
										  "uniform mat4 pMatrix;"
										  "uniform vec3 vLightPos;"
										  "uniform vec4 vColor;"
										  "attribute vec4 vVertex;"
										  "attribute vec3 vNormal;"
										  "varying vec4 vFragColor;"
										  "void main(void) { "
										  " mat3 mNormalMatrix;"
										  " mNormalMatrix[0] = normalize(mvMatrix[0].xyz);"
										  " mNormalMatrix[1] = normalize(mvMatrix[1].xyz);"
										  " mNormalMatrix[2] = normalize(mvMatrix[2].xyz);"
										  " vec3 vNorm = normalize(mNormalMatrix * vNormal);"
										  " vec4 ecPosition;"
										  " vec3 ecPosition3;"
										  " ecPosition = mvMatrix * vVertex;"
										  " ecPosition3 = ecPosition.xyz /ecPosition.w;"
										  " vec3 vLightDir = normalize(vLightPos - ecPosition3);"
										  " float fDot = max(0.0, dot(vNorm, vLightDir)); "
										  " vFragColor.rgb = vColor.rgb * fDot;"
										  " vFragColor.a = vColor.a;"
										  " mat4 mvpMatrix;"
										  " mvpMatrix = pMatrix * mvMatrix;"
										  " gl_Position = mvpMatrix * vVertex; "
										  "}";


static const char *szPointLightDiffFP = 
#ifdef OPENGL_ES
										"precision mediump float;"
#endif
										"varying vec4 vFragColor; "
										"void main(void) { "
										" gl_FragColor = vFragColor; "
										"}";

//GLT_SHADER_TEXTURE_REPLACE
// Just put the texture on the polygons

static const char *szTextureSDIVP =	"uniform mat4 mvpMatrix;"
										"attribute vec4 vVertex;"
										"attribute vec2 vTexCoord0;"
										"varying vec2 vTex;"
										"void main(void) "
										"{ vTex = vTexCoord0;"
										" gl_Position = mvpMatrix * vVertex; "
										"}";

static const char *szTextureSDIFP =
#ifdef OPENGL_ES
										"precision mediump float;"
#endif
										"varying vec2 vTex;"
										"uniform sampler2D textureUnit0;"
										"const mat4 colorMatrix = mat4(1, 1,0, 1, "
												"0, -0.344, 0,1.773, "
				                             				" 1.403, -0.714, 0, 0,"
				                              				"0, 0, 1,0 )"
				                       					" * mat4(1, 0, 0, 0,"
				                              				" 0, 1, 0, 0,"
				                              				" 0, 0, 1, 0,"
				                              				" 0, -0.5, -0.5, 1);"
										"void main(void) "
										"{ vec4 t0 = texture2D(textureUnit0, vTex); "
										"  gl_FragColor = clamp(colorMatrix* vec4(t0.g,"
                                          				" t0.r,"
                                           				" 1.0,"
                                          				" t0.b)"
                     							" , 0.0, 1.0); "
										"}";


static const char *szTextureReplaceVP =	"uniform mat4 mvpMatrix;"
										"attribute vec4 vVertex;"
										"attribute vec2 vTexCoord0;"
										"varying vec2 vTex;"
										"void main(void) "
										"{ vTex = vTexCoord0;" 
										" gl_Position = mvpMatrix * vVertex; "
										"}";


static const char *szTextureReplaceFP =
#ifdef OPENGL_ES
										"precision mediump float;"
#endif

										"varying vec2 vTex;"
										"uniform sampler2D textureUnit0;"
										"const mat4 colorMatrix = mat4(1, 1, 1, 0,"
										"0, -0.344, 1.773, 0,"
		                             				" 1.403, -0.714, 0, 0,"
		                              				"0, 0, 0, 1)"
		                       					" * mat4(1, 0, 0, 0,"
		                              				" 0, 1, 0, 0,"
		                              				" 0, 0, 1, 0,"
		                              				" 0, -0.5, -0.5, 1);"
										"void main(void) "
										"{ vec4 t0 = texture2D(textureUnit0, vTex); "
										"  gl_FragColor = clamp(colorMatrix* vec4(t0.g,"
                                          				" t0.b,"
                                           				" t0.r,"
                                          				" 1)"
                     							" , 0.0, 1.0); "
										"}";



									
static const char *szTextureReplaceFP_bright =
#ifdef OPENGL_ES
										"precision mediump float;"
#endif
#if USE_GAIN
		"uniform vec4      vanColor;  " //van_add
		"varying vec2 vTex;"
		"uniform sampler2D textureUnit0;"
		"const mat4 colorMatrix = mat4(1, 1, 1, 0,"
		"0, -0.344, 1.773, 0,"
     				" 1.403, -0.714, 0, 0,"
      				"0, 0, 0, 1)"
					" * mat4(1, 0, 0, 0,"
      				" 0, 1, 0, 0,"
      				" 0, 0, 1, 0,"
      				" 0, -0.5, -0.5, 1);"
		"void main(void) "
		"{ vec4 t0 = texture2D(textureUnit0, vTex); "

		"  gl_FragColor = clamp(colorMatrix* vec4(t0.g,"
          				" t0.b,"
           				" t0.r,"
          				" 1)"
					" , 0.0, 1.0); "

		"gl_FragColor.r=(pow((gl_FragColor.r ), vanColor.r));"
		"gl_FragColor.g=(pow((gl_FragColor.g ), vanColor.g));"
		"gl_FragColor.b=(pow((gl_FragColor.b),  vanColor.b));"

		"if(gl_FragColor.r<0.0)"
		"gl_FragColor.r=0.0f;"
		"else if(gl_FragColor.r>255.0)"
		"gl_FragColor.r=255.0f;"

			"if(gl_FragColor.g<0.0)"
			"gl_FragColor.g=0.0f;"
			"else if(gl_FragColor.g>255.0)"
			"gl_FragColor.g=255.0f;"

			"if(gl_FragColor.b<0.0)"
			"gl_FragColor.b=0.0f;"
			"else if(gl_FragColor.b>255.0)"
			"gl_FragColor.b=255.0f;"

 //van_add
		"}";
											//min max

#else
										"varying vec2 vTex;"
										"uniform sampler2D textureUnit0;"
										"const mat4 colorMatrix = mat4(1, 1, 1, 0,"
										"0, -0.344, 1.773, 0,"
		                             				" 1.403, -0.714, 0, 0,"
		                              				"0, 0, 0, 1)"
		                       					" * mat4(1, 0, 0, 0,"
		                              				" 0, 1, 0, 0,"
		                              				" 0, 0, 1, 0,"
		                              				" 0, -0.5, -0.5, 1);"
										"void main(void) "
										"{ vec4 t0 = texture2D(textureUnit0, vTex); "
										"  gl_FragColor = clamp(colorMatrix* vec4(t0.g,"
                                          				" t0.b,"
                                           				" t0.r,"
                                          				" 1)"
                     							" , 0.0, 1.0); "
										"}";
#endif

										static const char *szTextureENHANCEVP =	"uniform mat4 mvpMatrix;"
																				"attribute vec4 vVertex;"
																				"attribute vec2 vTexCoord0;"
																				"varying vec2 vTex;"
																				"void main(void) "
																				"{ vTex = vTexCoord0;"
																				" gl_Position = mvpMatrix * vVertex; "
																				"}";
										static const char *szTextureENHANCEFP =
										#ifdef OPENGL_ES
																				"precision mediump float;"
										#endif
																				"varying vec2 vTex;"
																				"uniform sampler2D textureUnit0;"
																				"void main(void) "
																				"{  gl_FragColor= texture2D(textureUnit0, vTex); "
																				"}";
static const char *szTextureORIVP =	"uniform mat4 mvpMatrix;"
										"attribute vec4 vVertex;"
										"attribute vec2 vTexCoord0;"
										"varying vec2 vTex;"
										"void main(void) "
										"{ vTex = vTexCoord0;"
										" gl_Position = mvpMatrix * vVertex; "
										"}";

static const char *szTextureORIFP =
#ifdef OPENGL_ES
										"precision mediump float;"
#endif
										"varying vec2 vTex;"
										"uniform sampler2D textureUnit0;"
										"void main(void) "
										"{ gl_FragColor = texture2D(textureUnit0, vTex); "
										"}";

// Just put the texture on the polygons
static const char *szTextureRectReplaceVP =	"uniform mat4 mvpMatrix;"
                                        "attribute vec4 vVertex;"
                                        "attribute vec2 vTexCoord0;"
                                        "varying vec2 vTex;"
                                        "void main(void) "
                                        "{ vTex = vTexCoord0;" 
                                        " gl_Position = mvpMatrix * vVertex; "
                                        "}";

static const char *szTextureRectReplaceFP = 
#ifdef OPENGL_ES
                                        "precision mediump float;"
#endif
                                        "varying vec2 vTex;"
                                        "uniform sampler2DRect textureUnit0;"
                                        "void main(void) "
                                        "{ gl_FragColor = texture2DRect(textureUnit0, vTex); "
                                        "}";



//GLT_SHADER_TEXTURE_MODULATE
// Just put the texture on the polygons, but multiply by the color (as a unifomr)
static const char *szTextureModulateVP ="uniform mat4 mvpMatrix;"
										"attribute vec4 vVertex;"
										"attribute vec2 vTexCoord0;"
										"varying vec2 vTex;"
										"void main(void) "
										"{ vTex = vTexCoord0;" 
										" gl_Position = mvpMatrix * vVertex; "
										"}";
									
static const char *szTextureModulateFP =
#ifdef OPENGL_ES
										"precision mediump float;"
#endif
										"varying vec2 vTex;"
										"uniform sampler2D textureUnit0;"
										"uniform vec4 vColor;"
										"void main(void) "
										"{ gl_FragColor = vColor * texture2D(textureUnit0, vTex); "
										"}";



//GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF
// Point light (Diffuse only), with texture (modulated)
static const char *szTexturePointLightDiffVP =	  "uniform mat4 mvMatrix;"
												  "uniform mat4 pMatrix;"
												  "uniform vec3 vLightPos;"
												  "uniform vec4 vColor;"
												  "attribute vec4 vVertex;"
												  "attribute vec3 vNormal;"
												  "varying vec4 vFragColor;"
												  "attribute vec2 vTexCoord0;"
												  "varying vec2 vTex;"
												  "void main(void) { "
												  " mat3 mNormalMatrix;"
												  " mNormalMatrix[0] = normalize(mvMatrix[0].xyz);"
												  " mNormalMatrix[1] = normalize(mvMatrix[1].xyz);"
												  " mNormalMatrix[2] = normalize(mvMatrix[2].xyz);"
												  " vec3 vNorm = normalize(mNormalMatrix * vNormal);"
												  " vec4 ecPosition;"
												  " vec3 ecPosition3;"
												  " ecPosition = mvMatrix * vVertex;"
												  " ecPosition3 = ecPosition.xyz /ecPosition.w;"
												  " vec3 vLightDir = normalize(vLightPos - ecPosition3);"
												  " float fDot = max(0.0, dot(vNorm, vLightDir)); "
												  " vFragColor.rgb = vColor.rgb * fDot;"
												  " vFragColor.a = vColor.a;"
												  " vTex = vTexCoord0;"
												  " mat4 mvpMatrix;"
												  " mvpMatrix = pMatrix * mvMatrix;"
												  " gl_Position = mvpMatrix * vVertex; "
												  "}";


static const char *szTexturePointLightDiffFP =	
#ifdef OPENGL_ES
												"precision mediump float;"
#endif



												"varying vec4 vFragColor;"
											    "varying vec2 vTex;"
												"uniform sampler2D textureUnit0;"
												"void main(void) { "
												" gl_FragColor = vFragColor * texture2D(textureUnit0, vTex);"
												"}";

//GLT_SHADER_TEXTURE_BLENDING
static const char *szTextureBlendingENHANCEVP =	"uniform mat4 mvpMatrix;"
										"attribute vec4 vVertex;"
										"attribute vec2 vTexCoord0;"
										"attribute vec2 vTexCoord1;"
										"attribute vec2 vTexCoord2;"
										"varying vec2 vTex0;"
										"varying vec2 vTex1;"
										"varying vec2 vTex2;"
										"void main(void) "
										"{ vTex0 = vTexCoord0;"
										" vTex1 = vTexCoord1;"
										" vTex2 = vTexCoord2;"
										" gl_Position = mvpMatrix * vVertex; "
										"}";


static const char *szTextureBlendingENHANCEFP =	"varying vec2 vTex0;"
								"varying vec2 vTex1;"
								"varying vec2 vTex2;"
								"uniform sampler2D textureUnit0;"
								"uniform sampler2D textureUnit1;"
								"uniform sampler2D textureUnit2;"
								"void main(void) "
								"{ vec4 t0 = texture2D(textureUnit0, vTex0.st);"
								" vec4 t1 = texture2D(textureUnit1, vTex1.st);"
								" vec4 t2 = texture2D(textureUnit2, vTex2.st);"
								"gl_FragColor = mix(t0, t1, t2.a); "
								"}";

// Just put the texture on the polygons., note: now we use UYVx as texuture. Must convert to BGR
static const char *szTextureBlendingVP =	"uniform mat4 mvpMatrix;"
										"attribute vec4 vVertex;"
										"attribute vec2 vTexCoord0;"
										"attribute vec2 vTexCoord1;"
										"attribute vec2 vTexCoord2;"
										"varying vec2 vTex0;"
										"varying vec2 vTex1;"
										"varying vec2 vTex2;"
										"void main(void) "
										"{ vTex0 = vTexCoord0;"
										" vTex1 = vTexCoord1;"
										" vTex2 = vTexCoord2;"
										" gl_Position = mvpMatrix * vVertex; "
										"}";

static const char *szTextureBlendingFP =
#ifdef OPENGL_ES
										"precision mediump float;"
#endif
#if USE_GAIN
					"uniform vec4      van_gain_Color;  " //van_add
					"uniform vec4      van_gain_Color2;  " //van_add
					"varying vec2 vTex0;"
					"varying vec2 vTex1;"
					"varying vec2 vTex2;"
					"uniform sampler2D textureUnit0;"
					"uniform sampler2D textureUnit1;"
					"uniform sampler2D textureUnit2;"
					"const mat4 colorMatrix = mat4(1, 1, 1, 0,"
					"0, -0.344, 1.773, 0,"
                 				" 1.403, -0.714, 0, 0,"
                  				"0, 0, 0, 1)"
           					" * mat4(1, 0, 0, 0,"
                  				" 0, 1, 0, 0,"
                  				" 0, 0, 1, 0,"
                  				" 0, -0.5, -0.5, 1);"
					"void main(void) "
					"{ vec4 t0 = texture2D(textureUnit0, vTex0.st);"
					"  vec4 t0Out = clamp(colorMatrix* vec4(t0.g,"
                      				" t0.b,"
                       				" t0.r,"
                      				" 1)"
 							" , 0.0, 1.0); "
					"t0Out.r=(pow((t0Out.r ), van_gain_Color.r));"
						"t0Out.g=(pow((t0Out.g ), van_gain_Color.g));"
						"t0Out .b=(pow((t0Out.b),  van_gain_Color.b));"

						"if(t0Out.r<0.0)"
						"t0Out.r=0.0f;"
						"else if(t0Out.r>255.0)"
						"t0Out.r=255.0f;"

							"if(t0Out.g<0.0)"
							"t0Out.g=0.0f;"
							"else if(t0Out.g>255.0)"
							"t0Out.g=255.0f;"

							"if(t0Out.b<0.0)"
							"t0Out.b=0.0f;"
							"else if(t0Out.b>255.0)"
							"t0Out.b=255.0f;"
					" vec4 t1 = texture2D(textureUnit1, vTex1.st);"
					"  vec4 t1Out = clamp(colorMatrix* vec4(t1.g,"
                      				" t1.b,"
                       				" t1.r,"
                      				" 1)"
 							" , 0.0, 1.0); "
					"t1Out.r=(pow((t1Out.r ), van_gain_Color2.r));"
						"t1Out.g=(pow((t1Out.g ), van_gain_Color2.g));"
						"t1Out.b=(pow((t1Out.b),  van_gain_Color2.b));"

						"if(t1Out.r<0.0)"
						"t1Out.r=0.0f;"
						"else if(t1Out.r>255.0)"
						"t1Out.r=255.0f;"

							"if(t1Out.g<0.0)"
							"t1Out.g=0.0f;"
							"else if(t1Out.g>255.0)"
							"t1Out.g=255.0f;"

							"if(t1Out.b<0.0)"
							"t1Out.b=0.0f;"
							"else if(t1Out.b>255.0)"
							"t1Out.b=255.0f;"
					" vec4 t2 = texture2D(textureUnit2, vTex2.st);"
					"gl_FragColor = mix(t0Out, t1Out, t2.a); "
					"}";






#else


										"varying vec2 vTex0;"
										"varying vec2 vTex1;"
										"varying vec2 vTex2;"
										"uniform sampler2D textureUnit0;"
										"uniform sampler2D textureUnit1;"
										"uniform sampler2D textureUnit2;"
										"const mat4 colorMatrix = mat4(1, 1, 1, 0,"
										"0, -0.344, 1.773, 0,"
		                             				" 1.403, -0.714, 0, 0,"
		                              				"0, 0, 0, 1)"
		                       					" * mat4(1, 0, 0, 0,"
		                              				" 0, 1, 0, 0,"
		                              				" 0, 0, 1, 0,"
		                              				" 0, -0.5, -0.5, 1);"
										"void main(void) "
										"{ vec4 t0 = texture2D(textureUnit0, vTex0.st);"
										"  vec4 t0Out = clamp(colorMatrix* vec4(t0.g,"
                                          				" t0.b,"
                                           				" t0.r,"
                                          				" 1)"
                     							" , 0.0, 1.0); "
										" vec4 t1 = texture2D(textureUnit1, vTex1.st);"
										"  vec4 t1Out = clamp(colorMatrix* vec4(t1.g,"
                                          				" t1.b,"
                                           				" t1.r,"
                                          				" 1)"
                     							" , 0.0, 1.0); "
										" vec4 t2 = texture2D(textureUnit2, vTex2.st);"
										"gl_FragColor = mix(t0Out, t1Out, t2.a); "
										"}";


#endif


										static const char *szTextureLineBlendingVP=	"uniform mat4 mvpMatrix;"
																						"attribute vec4 vVertex;"
																						"attribute vec2 vTexCoord0;"
																						"attribute vec2 vTexCoord1;"
																						"attribute vec2 vTexCoord2;"
																						"attribute vec2 vTexCoord3;"
																						"attribute vec2 vTexCoord4;"
																						"varying vec2 vTex0;"
																						"varying vec2 vTex1;"
																						"varying vec2 vTex2;"
																						"varying vec2 vTex3;"
																						"varying vec2 vTex4;"
																						"void main(void) "
																						"{ vTex0 = vTexCoord0;"
																						" vTex1 = vTexCoord1;"
																						" vTex2 = vTexCoord2;"
																						" vTex3 = vTexCoord3;"
																						" vTex4 = vTexCoord4;"
																						" gl_Position = mvpMatrix * vVertex; "
																						"}";


										static const char *szTextureLineBlendingFP =
										#ifdef OPENGL_ES
																				"precision mediump float;"
										#endif
																						"varying vec2 vTex0;"
																						"varying vec2 vTex1;"
																						"varying vec2 vTex2;"
																						"varying vec2 vTex3;"
																						"varying vec2 vTex4;"
																						"uniform sampler2D textureUnit0;"
																						"uniform sampler2D textureUnit1;"
																						"uniform sampler2D textureUnit2;"
																						"uniform sampler2D textureUnit3;"
																						"uniform sampler2D textureUnit4;"
																						"const mat4 colorMatrix = mat4(1, 1, 1, 0,"
																						"0, -0.344, 1.773, 0,"
														                             				" 1.403, -0.714, 0, 0,"
														                              				"0, 0, 0, 1)"
														                       					" * mat4(1, 0, 0, 0,"
														                              				" 0, 1, 0, 0,"
														                              				" 0, 0, 1, 0,"
														                              				" 0, -0.5, -0.5, 1);"
																						"void main(void) "
																						"{ vec4 t0 = texture2D(textureUnit0, vTex0.st);"
																						"  vec4 t0Out = clamp(colorMatrix* vec4(t0.g,"
												                                          				" t0.b,"
												                                           				" t0.r,"
												                                          				" 1)"
												                     							" , 0.0, 1.0); "
																						" vec4 t1 = texture2D(textureUnit1, vTex1.st);"
																						"  vec4 t1Out = clamp(colorMatrix* vec4(t1.g,"
												                                          				" t1.b,"
												                                           				" t1.r,"
												                                          				" 1)"
												                     							" , 0.0, 1.0); "
																						" vec4 t2 = texture2D(textureUnit2, vTex2.st);"
												" vec4 tgain0 = texture2D(textureUnit3, vTex3.st);"
														" tgain0 = tgain0*10;"
													    "vec4 tpic0Out=(pow(t0Out,  1/tgain0));"
												" vec4 tgain1 = texture2D(textureUnit4, vTex4.st);"
														" tgain1 = tgain1*10;"
													    "vec4 tpic1Out=(pow(t1Out,  1/tgain1));"
														"gl_FragColor = mix(tpic0Out, tpic1Out, t2.a); "
										//			"gl_FragColor =tgain1; "
																						"}";
//////////////////////////
										static const char *szReflectionVP =	"uniform mat4 mvpMatrix;"
																				"attribute vec4 vVertex;"
																				"attribute vec2 vTexCoord0;"
																				"attribute vec2 vTexCoord1;"
																				"varying vec2 vTex0;"
																				"varying vec2 vTex1;"
																				"void main(void) "
																				"{ vTex0 = vTexCoord0;"
																				" vTex1 = vTexCoord1;"
																				" gl_Position = mvpMatrix * vVertex; "
																				"}";
										static const char *szReflectionFP =
											"	uniform sampler2D textureUnit0;"  //ori_pic
										    "		uniform sampler2D   textureUnit1;" //alpha_pic
												"varying vec2 vTex0;"
												"varying vec2 vTex1;"
												"const mat4 colorMatrix = mat4(1, 1, 1, 0,"
																	"0, -0.344, 1.773, 0,"
												                 				" 1.403, -0.714, 0, 0,"
												                  				"0, 0, 0, 1)"
												           					" * mat4(1, 0, 0, 0,"
												                  				" 0, 1, 0, 0,"
												                  				" 0, 0, 1, 0,"
												                  				" 0, -0.5, -0.5, 1);"
																	"void main(void) "
																	"{ vec4 tpic = texture2D(textureUnit0, vTex0.st);"
																	"  vec4 tpicOut = clamp(colorMatrix* vec4("
																					"tpic.g,"
																					" tpic.b,"
																					" tpic.r,"
																					" 1)"
																					" , 0.0, 1.0); "
												" vec4 tgain = texture2D(textureUnit1, vTex1.st);"
													" tgain = tgain*10;"

												  " tpicOut=(pow(tpicOut,  1/tgain));"

													"gl_FragColor= clamp(tpicOut,0.0,1.0);"
												//	"gl_FragColor= tgain;"
										                                "}";



// GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF
// Modulate texture with diffuse point light

///////////////////////////////////////////////////////////////////////////////
// Constructor, just zero out everything

#if USE_GAIN
GLShaderManager::GLShaderManager(int cam_count):
		m_cam_count(cam_count)
#else
GLShaderManager::GLShaderManager(int cam_count)
#endif
	{
	// Set stock shader handles to 0... uninitialized
	for(unsigned int i = 0; i < GLT_SHADER_LAST; i++)
		uiStockShaders[i] = 0;
#if USE_GAIN
	if(cam_count>0){
	gain_ = new M3DVector3f[cam_count];
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<cam_count;j++)
		{
			gain_[j][i]=1.0f;
		}
	}
	vanColor=new M3DVector4f[cam_count];
	ptrimColor = new float[cam_count * 3];
	for(int i=0;i<3;i++)
		{
			for(int j=0;j<cam_count;j++)
			{
				vanColor[j][i]=1.0f;
				ptrimColor[i*cam_count+j]=1.0f;
			}
		}
	}
#endif
	}

	
///////////////////////////////////////////////////////////////////////////////
// Destructor, turn loose of everything

GLShaderManager::~GLShaderManager(void)
	{
	// Stock shaders is the general (are you initialized test)
	if(uiStockShaders[0] != 0) {
		unsigned int i;
		for(i = 0; i < GLT_SHADER_LAST; i++)
			glDeleteProgram(uiStockShaders[i]);
			
		// Free shader table too
//		for(i = 0; i < shaderTable.size(); i++)
//			glDeleteProgram(shaderTable[i].uiShaderID);
		}
#if USE_GAIN
	delete []gain_;
	delete []vanColor;
	delete []ptrimColor;
#endif
	}


#if USE_GAIN
void GLShaderManager::set_gain_(int index,float x,float y,float z)
{
	 this->gain_[index][0]=x;
	 this->gain_[index][1]=y;
	 this->gain_[index][2]=z;
}

#endif
///////////////////////////////////////////////////////////////////////////////
// Initialize and load the stock shaders
bool GLShaderManager::InitializeStockShaders(void)
	{
	// Be warned, going over 128 shaders may cause a
	// hickup for a reallocation.
//	shaderTable.reserve(128);
	
	uiStockShaders[GLT_SHADER_IDENTITY]			= gltLoadShaderPairSrcWithAttributes(szIdentityShaderVP, szIdentityShaderFP, 1, GLT_ATTRIBUTE_VERTEX, "vVertex");
	uiStockShaders[GLT_SHADER_FLAT]				= gltLoadShaderPairSrcWithAttributes(szFlatShaderVP, szFlatShaderFP, 1, GLT_ATTRIBUTE_VERTEX, "vVertex");
	uiStockShaders[GLT_SHADER_SHADED]			= gltLoadShaderPairSrcWithAttributes(szShadedVP, szShadedFP, 2,
																								GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_COLOR, "vColor");


	uiStockShaders[GLT_SHADER_DEFAULT_LIGHT]	= gltLoadShaderPairSrcWithAttributes(szDefaultLightVP, szDefaultLightFP, 2,
																								GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_NORMAL, "vNormal");
	
	uiStockShaders[GLT_SHADER_POINT_LIGHT_DIFF] = gltLoadShaderPairSrcWithAttributes(szPointLightDiffVP, szPointLightDiffFP, 2,
																								GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_NORMAL, "vNormal");

	uiStockShaders[GLT_SHADER_TEXTURE_REPLACE]  = gltLoadShaderPairSrcWithAttributes(szTextureReplaceVP, szTextureReplaceFP, 2, 
																								GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_TEXTURE0, "vTexCoord0");

	uiStockShaders[GLT_SHADER_TEXTURE_SDI]  = gltLoadShaderPairSrcWithAttributes(szTextureSDIVP, szTextureSDIFP, 2,
																									GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_TEXTURE0, "vTexCoord0");



	uiStockShaders[GLT_SHADER_TEXTURE_BRIGHT]  = gltLoadShaderPairSrcWithAttributes(szTextureReplaceVP, szTextureReplaceFP_bright, 2,
																									GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_TEXTURE0, "vTexCoord0");

	uiStockShaders[GLT_SHADER_ORI]  = gltLoadShaderPairSrcWithAttributes(szTextureORIVP, szTextureORIFP, 2,
																										GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_TEXTURE0, "vTexCoord0");

	uiStockShaders[GLT_SHADER_ENHANCE]  = gltLoadShaderPairSrcWithAttributes(szTextureENHANCEVP, szTextureENHANCEFP, 2,
																											GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_TEXTURE0, "vTexCoord0");

	uiStockShaders[GLT_SHADER_TEXTURE_MODULATE] = gltLoadShaderPairSrcWithAttributes(szTextureModulateVP, szTextureModulateFP, 2,
														GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_TEXTURE0, "vTexCoord0");

	uiStockShaders[GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF] = gltLoadShaderPairSrcWithAttributes(szTexturePointLightDiffVP, szTexturePointLightDiffFP, 3,
															GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_NORMAL, "vNormal", GLT_ATTRIBUTE_TEXTURE0, "vTexCoord0");

	
    uiStockShaders[GLT_SHADER_TEXTURE_RECT_REPLACE] = gltLoadShaderPairSrcWithAttributes(szTextureRectReplaceVP, szTextureRectReplaceFP, 2, 
                                                                                             GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_TEXTURE0, "vTexCoord0");

    uiStockShaders[GLT_SHADER_TEXTURE_BLENDING] = gltLoadShaderPairSrcWithAttributes(szTextureBlendingVP, szTextureBlendingFP, 4,
                                                                                                 GLT_ATTRIBUTE_VERTEX, "vVertex",
                                                                                                 GLT_ATTRIBUTE_TEXTURE0, "vTexCoord0",
                                                                                                 GLT_ATTRIBUTE_TEXTURE1, "vTexCoord1",
                                                                                                 GLT_ATTRIBUTE_TEXTURE2, "vTexCoord2");

    uiStockShaders[GLT_SHADER_TEXTURE_ENHANCE_BLENDING] = gltLoadShaderPairSrcWithAttributes(szTextureBlendingENHANCEVP, szTextureBlendingENHANCEFP, 4,
                                                                                                    GLT_ATTRIBUTE_VERTEX, "vVertex",
                                                                                                    GLT_ATTRIBUTE_TEXTURE0, "vTexCoord0",
                                                                                                    GLT_ATTRIBUTE_TEXTURE1, "vTexCoord1",
                                                                                                    GLT_ATTRIBUTE_TEXTURE2, "vTexCoord2");


    uiStockShaders[GLT_SHADER_TEXTURE_LINE_BLENDING] = gltLoadShaderPairSrcWithAttributes(szTextureLineBlendingVP, szTextureLineBlendingFP, 6,
                                                                                                 GLT_ATTRIBUTE_VERTEX, "vVertex",
                                                                                                 GLT_ATTRIBUTE_TEXTURE0, "vTexCoord0",
                                                                                                 GLT_ATTRIBUTE_TEXTURE1, "vTexCoord1",
                                                                                                 GLT_ATTRIBUTE_TEXTURE2, "vTexCoord2",
                                                                                                 GLT_ATTRIBUTE_TEXTURE3, "vTexCoord3",
                                                                                                 GLT_ATTRIBUTE_TEXTURE4, "vTexCoord4");
    uiStockShaders[GLT_SHADER_LINE_BRIGHT] = gltLoadShaderPairSrcWithAttributes(szReflectionVP, szReflectionFP, 3,
                                                                                                   GLT_ATTRIBUTE_VERTEX, "vVertex",
                                                                                                   GLT_ATTRIBUTE_TEXTURE0, "vTexCoord0",
                                                                                                   GLT_ATTRIBUTE_TEXTURE1, "vTexCoord1");

    if(uiStockShaders[0] != 0)
		return true;
		
	return false;
	}
	

///////////////////////////////////////////////////////////////////////
// Use a specific stock shader, and set the appropriate uniforms
GLint GLShaderManager::UseStockShader(GLT_STOCK_SHADER nShaderID, ...)
	{
	// Check for out of bounds
	if(nShaderID >= GLT_SHADER_LAST)
		return -1;

	// List of uniforms
	va_list uniformList;
	va_start(uniformList, nShaderID);

	// Bind to the correct shader
	glUseProgram(uiStockShaders[nShaderID]);

	// Set up the uniforms
#if USE_GAIN
	GLint  alpha_gain_A,alpha_gain_B,locAmbient,iTransform, iModelMatrix, iProjMatrix, iColor, iLight, iTextureUnit,iTextureUnit0, iTextureUnit1,iTextureUnit2, iTextureUnit3,iTextureUnit4;
	int				iInteger, iInteger0, iInteger1,iInteger2,iInteger3,iInteger4,van_num;
#else
	GLint iTransform, iModelMatrix, iProjMatrix, iColor, iLight, iTextureUnit,iTextureUnit0, iTextureUnit1,iTextureUnit2;
	int				iInteger, iInteger0, iInteger1,iInteger2;
#endif
	M3DMatrix44f* mvpMatrix;
	M3DMatrix44f*  pMatrix;
	M3DMatrix44f*  mvMatrix;
	M3DVector4f*  vColor;
	M3DVector3f*  vLightPos;



#if USE_GAIN
	if(!overLapRegion::GetoverLapRegion()->get_change_gain())
	{

		for(int i=1;i<4;i++)
		{
			vanColor[i][0]=gain_[i][0];
			vanColor[i][1]=gain_[i][1];
			vanColor[i][2]=gain_[i][2];
			vanColor[i][3]=1.0f;
	/*		printf("i=%d  vanColor[0]=%f vanColor[1]=%f  vanColor[2]=%f\n",
					i,
					vanColor[i][0],
					vanColor[i][1],
					vanColor[i][2]);*/
		}
	/*	for(int i=0;i<m_cam_count;i++)
		{
			for(int j=0;j<1;j++){
				vanColor[i][j]=ptrimColor[3*i];
			}
		}*/
	}
	else
	{
		for(int i=0;i<m_cam_count;i++)
			{
				for(int j=0;j<3;j++){
					vanColor[i][j]=ptrimColor[j*m_cam_count+i];
				}
			}
	}
#endif




	switch(nShaderID)
		{
		case GLT_SHADER_FLAT:			// Just the modelview projection matrix and the color
			iTransform = glGetUniformLocation(uiStockShaders[nShaderID], "mvpMatrix");
		    mvpMatrix = va_arg(uniformList, M3DMatrix44f*);
			glUniformMatrix4fv(iTransform, 1, GL_FALSE, *mvpMatrix);

			iColor = glGetUniformLocation(uiStockShaders[nShaderID], "vColor");
			vColor = va_arg(uniformList, M3DVector4f*);
			glUniform4fv(iColor, 1, *vColor);
			break;
		case	GLT_SHADER_ENHANCE:
			iTransform = glGetUniformLocation(uiStockShaders[nShaderID], "mvpMatrix");
						mvpMatrix = va_arg(uniformList, M3DMatrix44f*);
						glUniformMatrix4fv(iTransform, 1, GL_FALSE, *mvpMatrix);

						iTextureUnit = glGetUniformLocation(uiStockShaders[nShaderID], "textureUnit0");
						iInteger = va_arg(uniformList, int);
						glUniform1i(iTextureUnit, iInteger);
						break;
		case GLT_SHADER_ORI:
			iTransform = glGetUniformLocation(uiStockShaders[nShaderID], "mvpMatrix");
			mvpMatrix = va_arg(uniformList, M3DMatrix44f*);
			glUniformMatrix4fv(iTransform, 1, GL_FALSE, *mvpMatrix);

			iTextureUnit = glGetUniformLocation(uiStockShaders[nShaderID], "textureUnit0");
			iInteger = va_arg(uniformList, int);
			glUniform1i(iTextureUnit, iInteger);
			break;
        case GLT_SHADER_TEXTURE_RECT_REPLACE:
		case GLT_SHADER_TEXTURE_BRIGHT:	// Just the texture place
			iTransform = glGetUniformLocation(uiStockShaders[nShaderID], "mvpMatrix");
		    mvpMatrix = va_arg(uniformList, M3DMatrix44f*);
			glUniformMatrix4fv(iTransform, 1, GL_FALSE, *mvpMatrix);

			iTextureUnit = glGetUniformLocation(uiStockShaders[nShaderID], "textureUnit0");
			iInteger = va_arg(uniformList, int);
			glUniform1i(iTextureUnit, iInteger);

#if USE_GAIN
			van_num = va_arg(uniformList, int);
			locAmbient = glGetUniformLocation(uiStockShaders[nShaderID], "vanColor");
			glUniform4fv(locAmbient, 1,vanColor[van_num]);
#endif
			break;

		case GLT_SHADER_TEXTURE_REPLACE:	// Just the texture place
					iTransform = glGetUniformLocation(uiStockShaders[nShaderID], "mvpMatrix");
				    mvpMatrix = va_arg(uniformList, M3DMatrix44f*);
					glUniformMatrix4fv(iTransform, 1, GL_FALSE, *mvpMatrix);

					iTextureUnit = glGetUniformLocation(uiStockShaders[nShaderID], "textureUnit0");
					iInteger = va_arg(uniformList, int);
					glUniform1i(iTextureUnit, iInteger);
					break;


		case		GLT_SHADER_TEXTURE_SDI:
			iTransform = glGetUniformLocation(uiStockShaders[nShaderID], "mvpMatrix");
			mvpMatrix = va_arg(uniformList, M3DMatrix44f*);
			glUniformMatrix4fv(iTransform, 1, GL_FALSE, *mvpMatrix);
			iTextureUnit = glGetUniformLocation(uiStockShaders[nShaderID], "textureUnit0");
			iInteger = va_arg(uniformList, int);
			glUniform1i(iTextureUnit, iInteger);
			break;


		case GLT_SHADER_TEXTURE_MODULATE: // Multiply the texture by the geometry color
			iTransform = glGetUniformLocation(uiStockShaders[nShaderID], "mvpMatrix");
		    mvpMatrix = va_arg(uniformList, M3DMatrix44f*);
			glUniformMatrix4fv(iTransform, 1, GL_FALSE, *mvpMatrix);

			iColor = glGetUniformLocation(uiStockShaders[nShaderID], "vColor");
			vColor = va_arg(uniformList, M3DVector4f*);
			glUniform4fv(iColor, 1, *vColor);			

			iTextureUnit = glGetUniformLocation(uiStockShaders[nShaderID], "textureUnit0");
			iInteger = va_arg(uniformList, int);
			glUniform1i(iTextureUnit, iInteger);
			break;


		case GLT_SHADER_DEFAULT_LIGHT:
			iModelMatrix = glGetUniformLocation(uiStockShaders[nShaderID], "mvMatrix");
		    mvMatrix = va_arg(uniformList, M3DMatrix44f*);
			glUniformMatrix4fv(iModelMatrix, 1, GL_FALSE, *mvMatrix);

			iProjMatrix = glGetUniformLocation(uiStockShaders[nShaderID], "pMatrix");
		    pMatrix = va_arg(uniformList, M3DMatrix44f*);
			glUniformMatrix4fv(iProjMatrix, 1, GL_FALSE, *pMatrix);

			iColor = glGetUniformLocation(uiStockShaders[nShaderID], "vColor");
			vColor = va_arg(uniformList, M3DVector4f*);
			glUniform4fv(iColor, 1, *vColor);
			break;

		case GLT_SHADER_POINT_LIGHT_DIFF:
			iModelMatrix = glGetUniformLocation(uiStockShaders[nShaderID], "mvMatrix");
		    mvMatrix = va_arg(uniformList, M3DMatrix44f*);
			glUniformMatrix4fv(iModelMatrix, 1, GL_FALSE, *mvMatrix);

			iProjMatrix = glGetUniformLocation(uiStockShaders[nShaderID], "pMatrix");
		    pMatrix = va_arg(uniformList, M3DMatrix44f*);
			glUniformMatrix4fv(iProjMatrix, 1, GL_FALSE, *pMatrix);

			iLight = glGetUniformLocation(uiStockShaders[nShaderID], "vLightPos");
			vLightPos = va_arg(uniformList, M3DVector3f*);
			glUniform3fv(iLight, 1, *vLightPos);

			iColor = glGetUniformLocation(uiStockShaders[nShaderID], "vColor");
			vColor = va_arg(uniformList, M3DVector4f*);
			glUniform4fv(iColor, 1, *vColor);
			break;			

		case GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF:
			iModelMatrix = glGetUniformLocation(uiStockShaders[nShaderID], "mvMatrix");
		    mvMatrix = va_arg(uniformList, M3DMatrix44f*);
			glUniformMatrix4fv(iModelMatrix, 1, GL_FALSE, *mvMatrix);

			iProjMatrix = glGetUniformLocation(uiStockShaders[nShaderID], "pMatrix");
		    pMatrix = va_arg(uniformList, M3DMatrix44f*);
			glUniformMatrix4fv(iProjMatrix, 1, GL_FALSE, *pMatrix);

			iLight = glGetUniformLocation(uiStockShaders[nShaderID], "vLightPos");
			vLightPos = va_arg(uniformList, M3DVector3f*);
			glUniform3fv(iLight, 1, *vLightPos);

			iColor = glGetUniformLocation(uiStockShaders[nShaderID], "vColor");
			vColor = va_arg(uniformList, M3DVector4f*);
			glUniform4fv(iColor, 1, *vColor);

			iTextureUnit = glGetUniformLocation(uiStockShaders[nShaderID], "textureUnit0");
			iInteger = va_arg(uniformList, int);
			glUniform1i(iTextureUnit, iInteger);
			break;


		case GLT_SHADER_SHADED:		// Just the modelview projection matrix. Color is an attribute
			iTransform = glGetUniformLocation(uiStockShaders[nShaderID], "mvpMatrix");
		    pMatrix = va_arg(uniformList, M3DMatrix44f*);
			glUniformMatrix4fv(iTransform, 1, GL_FALSE, *pMatrix);
			break;

		case GLT_SHADER_IDENTITY:	// Just the Color
			iColor = glGetUniformLocation(uiStockShaders[nShaderID], "vColor");
			vColor = va_arg(uniformList, M3DVector4f*);
			glUniform4fv(iColor, 1, *vColor);
			break;

		case	GLT_SHADER_TEXTURE_ENHANCE_BLENDING:
		iTransform = glGetUniformLocation(uiStockShaders[nShaderID], "mvpMatrix");
				mvpMatrix = va_arg(uniformList, M3DMatrix44f*);
				glUniformMatrix4fv(iTransform, 1, GL_FALSE, *mvpMatrix);

				iTextureUnit0 = glGetUniformLocation(uiStockShaders[nShaderID], "textureUnit0");
				iInteger0 = va_arg(uniformList, int);
				glUniform1i(iTextureUnit0, iInteger0);

				iTextureUnit1 = glGetUniformLocation(uiStockShaders[nShaderID], "textureUnit1");
				iInteger1 = va_arg(uniformList, int);
				glUniform1i(iTextureUnit1, iInteger1);

				iTextureUnit2 = glGetUniformLocation(uiStockShaders[nShaderID], "textureUnit2");
				iInteger2 = va_arg(uniformList, int);
				glUniform1i(iTextureUnit2, iInteger2);

	#if USE_GAIN
					van_num = va_arg(uniformList, int);
					alpha_gain_A = glGetUniformLocation(uiStockShaders[nShaderID], "van_gain_Color");
					glUniform4fv(alpha_gain_A, 1,vanColor[van_num]);
						//	glUniform4fv(alpha_gain_A, 1,van_gain_Color[van_num]);
					alpha_gain_B = glGetUniformLocation(uiStockShaders[nShaderID], "van_gain_Color2");
					if(van_num+1==m_cam_count)//m_cam_count)
						glUniform4fv(alpha_gain_B, 1,vanColor[0]);
					else
					glUniform4fv(alpha_gain_B, 1,vanColor[van_num+1]);
						//	glUniform4fv(alpha_gain_B, 1,van_gain_Color[van_num+1]);

	#endif
					break;

		case GLT_SHADER_TEXTURE_BLENDING:	// Just the texture blending
			iTransform = glGetUniformLocation(uiStockShaders[nShaderID], "mvpMatrix");
			mvpMatrix = va_arg(uniformList, M3DMatrix44f*);
			glUniformMatrix4fv(iTransform, 1, GL_FALSE, *mvpMatrix);

			iTextureUnit0 = glGetUniformLocation(uiStockShaders[nShaderID], "textureUnit0");
			iInteger0 = va_arg(uniformList, int);
			glUniform1i(iTextureUnit0, iInteger0);

			iTextureUnit1 = glGetUniformLocation(uiStockShaders[nShaderID], "textureUnit1");
			iInteger1 = va_arg(uniformList, int);
			glUniform1i(iTextureUnit1, iInteger1);

			iTextureUnit2 = glGetUniformLocation(uiStockShaders[nShaderID], "textureUnit2");
			iInteger2 = va_arg(uniformList, int);
			glUniform1i(iTextureUnit2, iInteger2);

#if USE_GAIN
				van_num = va_arg(uniformList, int);
				alpha_gain_A = glGetUniformLocation(uiStockShaders[nShaderID], "van_gain_Color");
				glUniform4fv(alpha_gain_A, 1,vanColor[van_num]);
					//	glUniform4fv(alpha_gain_A, 1,van_gain_Color[van_num]);
				alpha_gain_B = glGetUniformLocation(uiStockShaders[nShaderID], "van_gain_Color2");
				if(van_num+1==m_cam_count)//m_cam_count)
					glUniform4fv(alpha_gain_B, 1,vanColor[0]);
				else
				glUniform4fv(alpha_gain_B, 1,vanColor[van_num+1]);
					//	glUniform4fv(alpha_gain_B, 1,van_gain_Color[van_num+1]);

#endif
				break;


		case GLT_SHADER_TEXTURE_LINE_BLENDING:
					iTransform = glGetUniformLocation(uiStockShaders[nShaderID], "mvpMatrix");
					mvpMatrix = va_arg(uniformList, M3DMatrix44f*);
					glUniformMatrix4fv(iTransform, 1, GL_FALSE, *mvpMatrix);

					iTextureUnit0 = glGetUniformLocation(uiStockShaders[nShaderID], "textureUnit0");
					iInteger0 = va_arg(uniformList, int);
					glUniform1i(iTextureUnit0, iInteger0);

					iTextureUnit1 = glGetUniformLocation(uiStockShaders[nShaderID], "textureUnit1");
					iInteger1 = va_arg(uniformList, int);
					glUniform1i(iTextureUnit1, iInteger1);

					iTextureUnit2 = glGetUniformLocation(uiStockShaders[nShaderID], "textureUnit2");
					iInteger2 = va_arg(uniformList, int);
					glUniform1i(iTextureUnit2, iInteger2);

					iTextureUnit3 = glGetUniformLocation(uiStockShaders[nShaderID], "textureUnit3");
					iInteger3 = va_arg(uniformList, int);
					glUniform1i(iTextureUnit3, iInteger3);

					iTextureUnit4 = glGetUniformLocation(uiStockShaders[nShaderID], "textureUnit4");
					iInteger4 = va_arg(uniformList, int);
					glUniform1i(iTextureUnit4, iInteger4);
				break;
		case GLT_SHADER_LINE_BRIGHT:
							iTransform = glGetUniformLocation(uiStockShaders[nShaderID], "mvpMatrix");
							mvpMatrix = va_arg(uniformList, M3DMatrix44f*);
							glUniformMatrix4fv(iTransform, 1, GL_FALSE, *mvpMatrix);

							iTextureUnit0 = glGetUniformLocation(uiStockShaders[nShaderID], "textureUnit0");
							iInteger0 = va_arg(uniformList, int);
							glUniform1i(iTextureUnit0, iInteger0);

							iTextureUnit1 = glGetUniformLocation(uiStockShaders[nShaderID], "textureUnit1");
							iInteger1 = va_arg(uniformList, int);
							glUniform1i(iTextureUnit1, iInteger1);
							break;
		default:
			break;
		}
	va_end(uniformList);

	return uiStockShaders[nShaderID];
	}


///////////////////////////////////////////////////////////////////////////////
// Lookup a stock shader
GLuint GLShaderManager::GetStockShader(GLT_STOCK_SHADER nShaderID)
	{
	if(nShaderID >= GLT_SHADER_LAST)
		return 0;
	
	return uiStockShaders[nShaderID];
	}


///////////////////////////////////////////////////////////////////////////////
// Lookup a previously loaded shader. If szFragProg == NULL, it is assumed to be
// the same name as szVertexProg
GLuint GLShaderManager::LookupShader(const char *szVertexProg, const char *szFragProg)
	{
	// Linear Search... this isn't supposed to be relied on all the time
/*	for(unsigned int i = 0; i < shaderTable.size(); i++)
		if((strncmp(szVertexProg, shaderTable[i].szVertexShaderName, MAX_SHADER_NAME_LENGTH) == 0) && 
			(strncmp(szFragProg, shaderTable[i].szFragShaderName, MAX_SHADER_NAME_LENGTH) == 0))
			return shaderTable[i].uiShaderID;
*/	
	// Failed
	return 0;
	}



///////////////////////////////////////////////////////////////////////////////
// Load a shader pair from file. The shader pair root is added to the shader
// lookup table and can be found again if necessary with LookupShader.
GLuint GLShaderManager::LoadShaderPair(const char *szVertexProgFileName, const char *szFragProgFileName)
	{
	SHADERLOOKUPETRY shaderEntry;

	// Make sure it's not already loaded
	GLuint uiReturn = LookupShader(szVertexProgFileName, szFragProgFileName);
	if(uiReturn != 0)
		return uiReturn;

	// Load shader and test for fail
	shaderEntry.uiShaderID = gltLoadShaderPair(szVertexProgFileName, szFragProgFileName);
	if(shaderEntry.uiShaderID == 0)
		return 0;
		
	// Add to the table
	strncpy(shaderEntry.szVertexShaderName, szVertexProgFileName, MAX_SHADER_NAME_LENGTH);
	strncpy(shaderEntry.szFragShaderName, szFragProgFileName, MAX_SHADER_NAME_LENGTH);
//	shaderTable.push_back(shaderEntry);	
	return shaderEntry.uiShaderID;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Load shaders from source text. If the szName is NULL, just make it and return the handle
// (useful for stock shaders). Otherwize, make sure it's not already there, then add to list
GLuint GLShaderManager::LoadShaderPairSrc(const char *szName, const char *szVertexSrc, const char *szFragSrc)
	{
	// Just make it and return
	if(szName == NULL)
		return gltLoadShaderPairSrc(szVertexSrc, szFragSrc);
		
	// It has a name, check for duplicate
	GLuint uiShader = LookupShader(szName, szName);
	if(uiShader != 0)
		return uiShader;
			
	// Ok, make it and add to table
	SHADERLOOKUPETRY shaderEntry;
	shaderEntry.uiShaderID = gltLoadShaderPairSrc(szVertexSrc, szFragSrc);
	if(shaderEntry.uiShaderID == 0)
		return 0;	// Game over, won't compile

	// Add it...
	strncpy(shaderEntry.szVertexShaderName, szName, MAX_SHADER_NAME_LENGTH);
	strncpy(shaderEntry.szFragShaderName, szName, MAX_SHADER_NAME_LENGTH);
//	shaderTable.push_back(shaderEntry);	
	return shaderEntry.uiShaderID;		
	}

	
///////////////////////////////////////////////////////////////////////////////////////////////
// Load the shader file, with the supplied named attributes
GLuint GLShaderManager::LoadShaderPairWithAttributes(const char *szVertexProgFileName, const char *szFragmentProgFileName, ...)
	{
	// Check for duplicate
	GLuint uiShader = LookupShader(szVertexProgFileName, szFragmentProgFileName);
	if(uiShader != 0)
		return uiShader;

	SHADERLOOKUPETRY shaderEntry;

    // Temporary Shader objects
    GLuint hVertexShader;
    GLuint hFragmentShader;   
    GLint testVal;
	
    // Create shader objects
    hVertexShader = glCreateShader(GL_VERTEX_SHADER);
    hFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	
    // Load them. If fail clean up and return null
    if(gltLoadShaderFile(szVertexProgFileName, hVertexShader) == false)
		{
        glDeleteShader(hVertexShader);
        glDeleteShader(hFragmentShader);
        return 0;
		}
	
    if(gltLoadShaderFile(szFragmentProgFileName, hFragmentShader) == false)
		{
        glDeleteShader(hVertexShader);
        glDeleteShader(hFragmentShader);
        return 0;
		}
    
    // Compile them
    glCompileShader(hVertexShader);
    glCompileShader(hFragmentShader);
    
    // Check for errors
    glGetShaderiv(hVertexShader, GL_COMPILE_STATUS, &testVal);
    if(testVal == GL_FALSE)
		{
        glDeleteShader(hVertexShader);
        glDeleteShader(hFragmentShader);
        return 0;
		}
    
    glGetShaderiv(hFragmentShader, GL_COMPILE_STATUS, &testVal);
    if(testVal == GL_FALSE)
		{
        glDeleteShader(hVertexShader);
        glDeleteShader(hFragmentShader);
        return 0;
		}
    
    // Link them - assuming it works...
	shaderEntry.uiShaderID = glCreateProgram();
    glAttachShader(shaderEntry.uiShaderID, hVertexShader);
    glAttachShader(shaderEntry.uiShaderID, hFragmentShader);


	// List of attributes
	va_list attributeList;
	va_start(attributeList, szFragmentProgFileName);

	char *szNextArg;
	int iArgCount = va_arg(attributeList, int);	// Number of attributes
	for(int i = 0; i < iArgCount; i++)
		{
		int index = va_arg(attributeList, int);
		szNextArg = va_arg(attributeList, char*);
		glBindAttribLocation(shaderEntry.uiShaderID, index, szNextArg);
		}

	va_end(attributeList);

    glLinkProgram(shaderEntry.uiShaderID);
	
    // These are no longer needed
    glDeleteShader(hVertexShader);
    glDeleteShader(hFragmentShader);  
    
    // Make sure link worked too
    glGetProgramiv(shaderEntry.uiShaderID, GL_LINK_STATUS, &testVal);
    if(testVal == GL_FALSE)
		{
		glDeleteProgram(shaderEntry.uiShaderID);
		return 0;
		}
    

	// Add it...
	strncpy(shaderEntry.szVertexShaderName, szVertexProgFileName, MAX_SHADER_NAME_LENGTH);
	strncpy(shaderEntry.szFragShaderName, szFragmentProgFileName, MAX_SHADER_NAME_LENGTH);
//	shaderTable.push_back(shaderEntry);	
	return shaderEntry.uiShaderID;		
	}


///////////////////////////////////////////////////////////////////////////////////////////////
// Load the shader from source, with the supplied named attributes
GLuint GLShaderManager::LoadShaderPairSrcWithAttributes(const char *szName, const char *szVertexProg, const char *szFragmentProg, ...)
	{
	// Check for duplicate
	GLuint uiShader = LookupShader(szName, szName);
	if(uiShader != 0)
		return uiShader;

	SHADERLOOKUPETRY shaderEntry;

    // Temporary Shader objects
    GLuint hVertexShader;
    GLuint hFragmentShader;  
    GLint testVal;
	
    // Create shader objects
    hVertexShader = glCreateShader(GL_VERTEX_SHADER);
    hFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	
    // Load them. 
    gltLoadShaderSrc(szVertexProg, hVertexShader);
    gltLoadShaderSrc(szFragmentProg, hFragmentShader);
   
    // Compile them
    glCompileShader(hVertexShader);
    glCompileShader(hFragmentShader);
    
    // Check for errors
    glGetShaderiv(hVertexShader, GL_COMPILE_STATUS, &testVal);
    if(testVal == GL_FALSE)
		{
        glDeleteShader(hVertexShader);
        glDeleteShader(hFragmentShader);
        return 0;
		}
    
    glGetShaderiv(hFragmentShader, GL_COMPILE_STATUS, &testVal);
    if(testVal == GL_FALSE)
		{
        glDeleteShader(hVertexShader);
        glDeleteShader(hFragmentShader);
        return 0;
		}
    
    // Link them - assuming it works...
	shaderEntry.uiShaderID = glCreateProgram();
    glAttachShader(shaderEntry.uiShaderID, hVertexShader);
    glAttachShader(shaderEntry.uiShaderID, hFragmentShader);

	// List of attributes
	va_list attributeList;
	va_start(attributeList, szFragmentProg);

	char *szNextArg;
	int iArgCount = va_arg(attributeList, int);	// Number of attributes
	for(int i = 0; i < iArgCount; i++)
		{
		int index = va_arg(attributeList, int);
		szNextArg = va_arg(attributeList, char*);
		glBindAttribLocation(shaderEntry.uiShaderID, index, szNextArg);
		}
	va_end(attributeList);


    glLinkProgram(shaderEntry.uiShaderID);
	
    // These are no longer needed
    glDeleteShader(hVertexShader);
    glDeleteShader(hFragmentShader);  
    
    // Make sure link worked too
    glGetProgramiv(shaderEntry.uiShaderID, GL_LINK_STATUS, &testVal);
    if(testVal == GL_FALSE)
		{
		glDeleteProgram(shaderEntry.uiShaderID);
		return 0;
		}
     
	// Add it...
	strncpy(shaderEntry.szVertexShaderName, szName, MAX_SHADER_NAME_LENGTH);
	strncpy(shaderEntry.szFragShaderName, szName, MAX_SHADER_NAME_LENGTH);
//	shaderTable.push_back(shaderEntry);	
	return shaderEntry.uiShaderID;		
	}
