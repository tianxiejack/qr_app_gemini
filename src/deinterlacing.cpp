#include "v4l2camera.h"
#include <stdlib.h>
#include <string.h>
#include<opencv2/opencv.hpp>
#include <omp.h>
#if  !NO_ARM_NEON
#include <arm_neon.h>
#endif
using namespace std;
#define _abs abs
#define _min2 MIN
#define _max2 MAX

inline void v4l2_camera::Deinterlace(unsigned char *pTmpBuf, unsigned char *lpYFrame, int ImgWidth, int ImgHeight, int ImgStride)
{
	int x,	xx, y;
	register int thred = 50;
	memcpy(pTmpBuf, lpYFrame, ImgStride*ImgHeight*2);

#pragma omp parallel for
	for(y = 0; y < (ImgHeight-2); y+=2)
	{
		register unsigned char *pSrc0,  *pSrc2, *pOdd, *pDst;
		pSrc0 = pTmpBuf + y*ImgStride*2;
		pSrc2 = pTmpBuf + (y+2)*ImgStride*2;
		pOdd = pTmpBuf + (y+1)*ImgStride*2;
		pDst = lpYFrame + (y+1)*ImgStride*2;
		for(xx = 1; xx < ImgWidth-1; xx++)
		{
			x = 2*xx+1;
			register int a_1 = pSrc0[x-2];
			register int a = pSrc0[x];
			register int a1 = pSrc0[x+2];
			register int d = pOdd[x];
			register int b_1 = pSrc2[x-2];
			register int b = pSrc2[x];
			register int b1 = pSrc2[x+2];
			register int grd = _abs(a_1 - d) + _abs(a - d) + _abs(a1 - d) + _abs(b_1 - d) + _abs(b - d) + _abs(b1 - d);

			if(grd > thred)
			{
				register int grda = _abs(a_1 - b1);
				register int grdb = _abs(a   - b);
				register int grdc = _abs(a1 - b_1);

				if( (grda < grdb) && (grda < grdc) )
				{
					pDst[x] = a_1 + b1 + d- _min2(_min2(a_1, b1), d) - _max2(_max2(a_1, b1), d);//medthr(a_1, b1, d);
				}
				else if( (grdc < grda) && (grdc < grdb) )
				{
					pDst[x] = a1 + b_1 + d- _min2(_min2(a1, b_1), d) - _max2(_max2(a1, b_1), d);//medthr(a1, b_1, d);
				}
				else
				{
					pDst[x] = a + b + d- _min2(_min2(a, b), d) - _max2(_max2(a, b), d);//medthr(a, b, d);
				}
			}
			else
			{
				pDst[x] = pOdd[x];
			}
		}
	}
}

void v4l2_camera::DeinterlaceYUV(unsigned char *pTmpBuf, unsigned char *lpYUVFrame, int ImgWidth, int ImgHeight, int ImgStride)
{
#if !NO_ARM_NEON
	int	y;
	int stride8x8 = ImgWidth*2/8;
#pragma omp parallel for
	for(y = 0; y < (ImgHeight-2); y+=2)
	{
		unsigned char *pSrc0,  *pSrc2, *pOdd, *pDst;
		uint8x8_t * __restrict__ pSrc08x8_t, * __restrict__ pSrc28x8_t, * __restrict__ pOdd8x8_t, * __restrict__ pDst8x8_t;
		int x,i;

		pSrc08x8_t = (uint8x8_t *)(lpYUVFrame + y*ImgStride*2);
		pSrc28x8_t = (uint8x8_t *)(lpYUVFrame + (y+2)*ImgStride*2);
		pOdd8x8_t = (uint8x8_t *)(lpYUVFrame + (y+1)*ImgStride*2);
		pDst8x8_t = (uint8x8_t *)(lpYUVFrame + (y+1)*ImgStride*2);
		for(i=0; i < stride8x8;  i++)
		{
			uint8x8_t a = pSrc08x8_t[i];
			uint8x8_t d = pOdd8x8_t[i];
			uint8x8_t b = pSrc28x8_t[i];
			pDst8x8_t[i]= vsub_u8(vsub_u8(vadd_u8(vadd_u8(a,b), d), vmin_u8( vmin_u8(a,b),d)), vmax_u8( vmax_u8(a,b),d));

		}
	}
#endif
}

void DeinterlaceYUV_Neon(unsigned char *lpYUVFrame, int ImgWidth, int ImgHeight, int ImgStride)
{
	int	y;
#ifndef DISABLE_NEON_DEI
	int stride8x8 = ImgWidth*2/8;
#pragma omp parallel for
	for(y = 0; y < (ImgHeight-2); y+=2)
	{
		uint8x8_t * __restrict__ pSrc08x8_t, * __restrict__ pSrc28x8_t, * __restrict__ pOdd8x8_t, * __restrict__ pDst8x8_t;
		int i;

		pSrc08x8_t = (uint8x8_t *)(lpYUVFrame + y*ImgStride*2);
		pSrc28x8_t = (uint8x8_t *)(lpYUVFrame + (y+2)*ImgStride*2);
		pOdd8x8_t = (uint8x8_t *)(lpYUVFrame + (y+1)*ImgStride*2);
		pDst8x8_t = (uint8x8_t *)(lpYUVFrame + (y+1)*ImgStride*2);
		for(i=0; i < stride8x8;  i++)
		{
			uint8x8_t a = pSrc08x8_t[i];
			uint8x8_t d = pOdd8x8_t[i];
			uint8x8_t b = pSrc28x8_t[i];
			pDst8x8_t[i]= vsub_u8(vsub_u8(vadd_u8(vadd_u8(a,b), d), vmin_u8( vmin_u8(a,b),d)), vmax_u8( vmax_u8(a,b),d));
		}
	}
#else
#pragma omp parallel for
	for(y = 0; y < (ImgHeight-2); y+=2)
	{
		register unsigned char *pSrc0,  *pSrc2, *pOdd, *pDst;
		int x;

		pSrc0 = lpYUVFrame + y*ImgStride*2;
		pSrc2 = lpYUVFrame + (y+2)*ImgStride*2;
		pOdd = lpYUVFrame + (y+1)*ImgStride*2;
		pDst = lpYUVFrame + (y+1)*ImgStride*2;
		for(x = 0; x < ImgWidth*2; x++)
		{
			register int a = pSrc0[x];
			register int d = pOdd[x];
			register int b = pSrc2[x];

			pDst[x] = a + b + d- _min2(_min2(a, b), d) - _max2(_max2(a, b), d);//medthr(a, b, d);
		}
	}
#endif
}
