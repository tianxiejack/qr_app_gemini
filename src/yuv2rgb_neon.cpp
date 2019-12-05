/*
 * Copyright (C) 2012 Andre Chen and contributors.

 * andre.hl.chen@gmail.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include "yuv2rgb.h"
#include <omp.h>

#define ROUND(a,c)   uint16x8_t t##a;\
            uint8x8x2_t twin##a ;\
        	twin##a = vld2_u8(yuyv##a);\
            t##a = vmovl_u8(vqsub_u8(twin##a .val[0], Yshift));\
            int32x4_t const Y00##a = (int32x4_t)vmulq_n_u32(vmovl_u16(vget_low_u16(t##a)), 298);\
            int32x4_t const Y01##a = (int32x4_t)vmulq_n_u32(vmovl_u16(vget_high_u16(t##a)), 298);\
            t##a = (uint16x8_t)vsubq_s16((int16x8_t)vmovl_u8(twin##a.val[1]), half);\
            int16x4_t h##a=vget_high_s16((int16x8_t)t##a);\
            int16x4_t l##a=vget_low_s16((int16x8_t)t##a);\
            int16x4_t const V##a = vuzp1_s16(l##a,h##a);\
            int16x4_t const U##a = vuzp2_s16(l##a,h##a);\
            int32x4_t const tR##a = vmlal_n_s16(rounding, V##a, 409);\
            int32x4_t const tG##a = vmlal_n_s16(vmlal_n_s16(rounding, V##a, -208), U##a, -100);\
            int32x4_t const tB##a = vmlal_n_s16(rounding, U##a, 516);\
            int32x4x2_t const R##a = vzipq_s32(tR##a, tR##a); \
            int32x4x2_t const G##a = vzipq_s32(tG##a, tG##a); \
            int32x4x2_t const B##a = vzipq_s32(tB##a, tB##a); \
            trait::store_pixel_block(dst##a, pblock,\
                    vshrn_n_u16(vcombine_u16(vqmovun_s32(vaddq_s32(R##a.val[0], Y00##a)), vqmovun_s32(vaddq_s32(R##a.val[1], Y01##a))), 8),\
                    vshrn_n_u16(vcombine_u16(vqmovun_s32(vaddq_s32(G##a.val[0], Y00##a)), vqmovun_s32(vaddq_s32(G##a.val[1], Y01##a))), 8),\
                    vshrn_n_u16(vcombine_u16(vqmovun_s32(vaddq_s32(B##a.val[0], Y00##a)), vqmovun_s32(vaddq_s32(B##a.val[1], Y01##a))), 8));\
            yuyv##a+=c*16;\
            dst##a+=(c*8*trait::bytes_per_pixel);


void split(unsigned char const *src,unsigned char *dstY,unsigned char *dstUV,int srcYcount)
{
	for(int i=0;i<srcYcount;i++)
	{
    *(dstY++) =*(src++) ;
    *(dstUV++)= *(src++);
	}
}
//-----------------------------------------------------------------------------
#if ARM_NEON_ENABLE
#include <arm_neon.h>
template<typename trait>
bool decode_yuv_neon(unsigned char* out, unsigned char const* y, unsigned char const* uv, int width, int height, unsigned char fill_alpha=0xff)
{
    // pre-condition : width, height must be even
    if (0!=(width&1) || width<2 || 0!=(height&1) || height<2 || !out || !y || !uv)
        return false;

    // in & out pointers
    unsigned char* dst = out;

    // constants
    int const stride = width*trait::bytes_per_pixel;
    int const itHeight = height>>1;
    int const itWidth = width>>4;

    uint8x8_t const Yshift = vdup_n_u8(16);
    int16x8_t const half = (int16x8_t)vdupq_n_u16(128);
    int32x4_t const rounding = vdupq_n_s32(128);

    // tmp variable
    uint16x8_t t;

    // pixel block to temporary store 8 pixels
    typename trait::PixelBlock pblock = trait::init_pixelblock(fill_alpha);    

    for (int j=0; j<itHeight; j++, y+=width, dst+=stride) {

        for (int i=0; i<itWidth; i++, y+=8, uv+=8, dst+=(8*trait::bytes_per_pixel)) {
            t = vmovl_u8(vqsub_u8(vld1_u8(y), Yshift));
            int32x4_t const Y00 = (int32x4_t)vmulq_n_u32(vmovl_u16(vget_low_u16(t)), 298);
            int32x4_t const Y01 = (int32x4_t)vmulq_n_u32(vmovl_u16(vget_high_u16(t)), 298);

            t = vmovl_u8(vqsub_u8(vld1_u8(y+width), Yshift));
            int32x4_t const Y10 = (int32x4_t)vmulq_n_u32(vmovl_u16(vget_low_u16(t)), 298);
            int32x4_t const Y11 = (int32x4_t)vmulq_n_u32(vmovl_u16(vget_high_u16(t)), 298);

            // trait::loadvu pack 4 sets of uv into a uint8x8_t, layout : { v0,u0, v1,u1, v2,u2, v3,u3 }
            t = (uint16x8_t)vsubq_s16((int16x8_t)vmovl_u8(trait::loadvu(uv)), half);

            // UV.val[0] : v0, v1, v2, v3
            // UV.val[1] : u0, u1, u2, u3
int16x4_t h=vget_high_s16((int16x8_t)t);
int16x4_t l=vget_low_s16((int16x8_t)t);
            int16x4_t const V = vuzp1_s16(l,h);
 	int16x4_t const U = vuzp2_s16(l,h);
            // tR : 128+409V
            // tG : 128-100U-208V
            // tB : 128+516U
            int32x4_t const tR = vmlal_n_s16(rounding, V, 409);
            int32x4_t const tG = vmlal_n_s16(vmlal_n_s16(rounding, V, -208), U, -100);
            int32x4_t const tB = vmlal_n_s16(rounding, U, 516);

            int32x4x2_t const R = vzipq_s32(tR, tR); // [tR0, tR0, tR1, tR1] [ tR2, tR2, tR3, tR3]
            int32x4x2_t const G = vzipq_s32(tG, tG); // [tG0, tG0, tG1, tG1] [ tG2, tG2, tG3, tG3]
            int32x4x2_t const B = vzipq_s32(tB, tB); // [tB0, tB0, tB1, tB1] [ tB2, tB2, tB3, tB3]

            // upper 8 pixels
            trait::store_pixel_block(dst, pblock,
                    vshrn_n_u16(vcombine_u16(vqmovun_s32(vaddq_s32(R.val[0], Y00)), vqmovun_s32(vaddq_s32(R.val[1], Y01))), 8),
                    vshrn_n_u16(vcombine_u16(vqmovun_s32(vaddq_s32(G.val[0], Y00)), vqmovun_s32(vaddq_s32(G.val[1], Y01))), 8),
                    vshrn_n_u16(vcombine_u16(vqmovun_s32(vaddq_s32(B.val[0], Y00)), vqmovun_s32(vaddq_s32(B.val[1], Y01))), 8));

            // lower 8 pixels
            trait::store_pixel_block(dst+stride, pblock,
                    vshrn_n_u16(vcombine_u16(vqmovun_s32(vaddq_s32(R.val[0], Y10)), vqmovun_s32(vaddq_s32(R.val[1], Y11))), 8),
                    vshrn_n_u16(vcombine_u16(vqmovun_s32(vaddq_s32(G.val[0], Y10)), vqmovun_s32(vaddq_s32(G.val[1], Y11))), 8),
                    vshrn_n_u16(vcombine_u16(vqmovun_s32(vaddq_s32(B.val[0], Y10)), vqmovun_s32(vaddq_s32(B.val[1], Y11))), 8));
        }
    }
    return true;
}



template<typename trait>
bool decode_yuyv_neon( unsigned char* out, unsigned char const* __restrict__  yuyv,int width, int height, unsigned char fill_alpha=0)
{
    // pre-condition : width, height must be even
    if (0!=(width&1) || width<2 || 0!=(height&1) || height<2 || !out || !yuyv )
        return false;

    // in & out pointers
    unsigned char* __restrict__ dst = out;

    // constants
    int const stride = width*trait::bytes_per_pixel;
    int const itHeight = height;
    int const itWidth = width>>4;

    uint8x8_t const Yshift = vdup_n_u8(16);
    int16x8_t const half = (int16x8_t)vdupq_n_u16(128);
    int32x4_t const rounding = vdupq_n_s32(128);

    unsigned char const* __restrict__  yuyv1=yuyv;
    unsigned char const* __restrict__  yuyv2=yuyv+16;
    unsigned char const* __restrict__  yuyv3=yuyv+32;
    unsigned char const* __restrict__  yuyv4=yuyv+48;

    unsigned char* __restrict__ dst1 = out;
    unsigned char* __restrict__ dst2 = out+(8*trait::bytes_per_pixel);
    unsigned char* __restrict__ dst3 = out+(16*trait::bytes_per_pixel);
    unsigned char* __restrict__ dst4 = out+(24*trait::bytes_per_pixel);

    // pixel block to temporary store 8 pixels
    typename trait::PixelBlock pblock = trait::init_pixelblock(fill_alpha);
    for (int j=0; j<itHeight; j++) {
//#pragma omp parallel for
        for (int i=0; i<itWidth; i++) {
            ROUND(1,2)
		    ROUND(2,2)
      }
    }
    return true;
}

//------------------------------------------------------------------------------
class NV21toRGB_neon {
public:
    enum { bytes_per_pixel = 3 };
    typedef uint8x8x3_t PixelBlock;
    static PixelBlock const init_pixelblock(unsigned char /*fill_alpha*/) {
        return uint8x8x3_t();
    }
    static uint8x8_t const loadvu(unsigned char const* uv) {
        return vld1_u8(uv);
    }
    static void store_pixel_block(unsigned char* dst, PixelBlock& pblock, uint8x8_t const& r, uint8x8_t const& g, uint8x8_t const& b) {
        pblock.val[0] = r;
        pblock.val[1] = g;
        pblock.val[2] = b;
        vst3_u8(dst, pblock);
    }
};

bool nv21_to_rgb(unsigned char* rgb, unsigned char const* nv21, int width, int height) {
    return decode_yuv_neon<NV21toRGB_neon>(rgb, nv21, nv21+(width*height), width, height);
}

bool nv21_to_rgb(unsigned char* rgb, int width, int height,unsigned char  const*src) {

	return decode_yuyv_neon<NV21toRGB_neon>(rgb,src,width, height);
}

//------------------------------------------------------------------------------
class NV21toRGBA_neon {
public:
    enum { bytes_per_pixel = 4 };
    typedef uint8x8x4_t PixelBlock;
    static PixelBlock const init_pixelblock(unsigned char fill_alpha) {
        PixelBlock block;
        block.val[3] = vdup_n_u8(fill_alpha); // alpha channel in the last
        return block;
    }
    static uint8x8_t const loadvu(unsigned char const* uv) {
        return vld1_u8(uv);
    }
    static void store_pixel_block(unsigned char* dst, PixelBlock& pblock, uint8x8_t const& r, uint8x8_t const& g, uint8x8_t const& b) {
        pblock.val[0] = r;
        pblock.val[1] = g;
        pblock.val[2] = b;
        vst4_u8(dst, pblock);
    }
};
bool nv21_to_rgba(unsigned char* rgba, unsigned char alpha, unsigned char const* nv21, int width, int height) {
    return decode_yuv_neon<NV21toRGBA_neon>(rgba, nv21, nv21+(width*height), width, height, alpha);
}

//------------------------------------------------------------------------------
class NV21toBGRA_neon {
public:
    enum { bytes_per_pixel = 4 };
    typedef uint8x8x4_t PixelBlock;
    static PixelBlock const init_pixelblock(unsigned char fill_alpha) {
        PixelBlock block;
        block.val[3] = vdup_n_u8(fill_alpha); // alpha channel in the last
        return block;
    }
    static uint8x8_t const loadvu(unsigned char const* uv) {
        return vld1_u8(uv);
    }
    static void store_pixel_block(unsigned char* dst, PixelBlock& pblock, uint8x8_t const& r, uint8x8_t const& g, uint8x8_t const& b) {
        pblock.val[0] = b;
        pblock.val[1] = g;
        pblock.val[2] = r;
        vst4_u8(dst, pblock);
    }
};
bool nv21_to_bgra(unsigned char* rgba, unsigned char alpha, unsigned char const* nv21, int width, int height) {
    return decode_yuv_neon<NV21toBGRA_neon>(rgba, nv21, nv21+(width*height), width, height, alpha);
}

//------------------------------------------------------------------------------
class NV21toBGR_neon {
public:
    enum { bytes_per_pixel = 3 };
    typedef uint8x8x3_t PixelBlock;
    static PixelBlock const init_pixelblock(unsigned char /*fill_alpha*/) {
        return uint8x8x3_t();
    }
    static uint8x8_t const loadvu(unsigned char const* uv) {
        return vld1_u8(uv);
    }
    static void store_pixel_block(unsigned char* dst, PixelBlock& pblock, uint8x8_t const& r, uint8x8_t const& g, uint8x8_t const& b) {
        pblock.val[0] = b;
        pblock.val[1] = g;
        pblock.val[2] = r;
        vst3_u8(dst, pblock);
    }
};
bool nv21_to_bgr(unsigned char* bgr, unsigned char const* nv21, int width, int height) {
    return decode_yuv_neon<NV21toBGR_neon>(bgr, nv21, nv21+(width*height), width, height);
}

#else // !neon

#ifdef __ANDROID__
#warning "use SLOW YUV(nv21) decoder : Try #define ARM_NEON_ENABLE if target architecture is ARMv7a"
#endif

//------------------------------------------------------------------------------
template<typename trait>
bool decode_yuv(unsigned char* out, unsigned char const* yuv, int width, int height, unsigned char alpha=0xff)
{
    // pre-condition : width and height must be even
    if (0!=(width&1) || width<2 || 0!=(height&1) || height<2 || !out || !yuv)
        return false;

    unsigned char* dst0 = out;

    unsigned char const* y0 = yuv;
    unsigned char const* uv = yuv + (width*height);
    int const halfHeight = height>>1;
    int const halfWidth = width>>1;

    int Y00, Y01, Y10, Y11;
    int V, U;
    int tR, tG, tB;
    for (int h=0; h<halfHeight; ++h) {
        unsigned char const* y1 = y0+width;
        unsigned char* dst1 = dst0 + width*trait::bytes_per_pixel;
        for (int w=0; w<halfWidth; ++w) {
            // shift
            Y00 = (*y0++) - 16;  Y01 = (*y0++) - 16;
            Y10 = (*y1++) - 16;  Y11 = (*y1++) - 16;

            // U,V or V,U? our trait will make the right call
            trait::loadvu(U, V, uv);

            // temps
            Y00 = (Y00>0) ? (298*Y00):0;
            Y01 = (Y01>0) ? (298*Y01):0;
            Y10 = (Y10>0) ? (298*Y10):0;
            Y11 = (Y11>0) ? (298*Y11):0;
            tR = 128 + 409*V;
            tG = 128 - 100*U - 208*V;
            tB = 128 + 516*U;

            // 2x2 pixels result
            trait::store_pixel(dst0, Y00+tR, Y00+tG, Y00+tB, alpha);
            trait::store_pixel(dst0, Y01+tR, Y01+tG, Y01+tB, alpha);
            trait::store_pixel(dst1, Y10+tR, Y10+tG, Y10+tB, alpha);
            trait::store_pixel(dst1, Y11+tR, Y11+tG, Y11+tB, alpha);
        }
        y0 = y1;
        dst0 = dst1;
    }
    return true;
}

//------------------------------------------------------------------------------
class NV21toRGB {
public:
    enum { bytes_per_pixel = 3 };
    static void loadvu(int& U, int& V, unsigned char const* &uv) {
        V = (*uv++) - 128;
        U = (*uv++) - 128;
    }
    static void store_pixel(unsigned char* &dst, int iR, int iG, int iB, unsigned char/*alpha*/) {
        *dst++ = (iR>0) ? (iR<65535 ? (unsigned char)(iR>>8):0xff):0;
        *dst++ = (iG>0) ? (iG<65535 ? (unsigned char)(iG>>8):0xff):0;
        *dst++ = (iB>0) ? (iB<65535 ? (unsigned char)(iB>>8):0xff):0;
    }
};
bool nv21_to_rgb(unsigned char* rgb, unsigned char const* nv21, int width, int height) {
	return decode_yuv<NV21toRGB>(rgb, nv21, width, height);
}
bool nv21_to_rgb(unsigned char* rgb, int width, int height,unsigned char const* nv21)
{
	return true;
}
//------------------------------------------------------------------------------
class NV21toRGBA {
public:
    enum { bytes_per_pixel = 4 };
    static void loadvu(int& U, int& V, unsigned char const* &uv) {
        V = (*uv++) - 128;
        U = (*uv++) - 128;
    }
    static void store_pixel(unsigned char* &dst, int iR, int iG, int iB, unsigned char alpha) {
        *dst++ = (iR>0) ? (iR<65535 ? (unsigned char)(iR>>8):0xff):0;
        *dst++ = (iG>0) ? (iG<65535 ? (unsigned char)(iG>>8):0xff):0;
        *dst++ = (iB>0) ? (iB<65535 ? (unsigned char)(iB>>8):0xff):0;
        *dst++ = alpha;
    }
};

bool nv21_to_rgba(unsigned char* rgba, unsigned char alpha, unsigned char const* nv21, int width, int height) {
    return decode_yuv<NV21toRGBA>(rgba, nv21, width, height, alpha);
}

//------------------------------------------------------------------------------
class NV21toBGR {
public:
    enum { bytes_per_pixel = 3 };
    static void loadvu(int& U, int& V, unsigned char const* &uv) {
        V = (*uv++) - 128;
        U = (*uv++) - 128;
    }
    static void store_pixel(unsigned char* &dst, int iR, int iG, int iB, unsigned char/*alpha*/) {
        *dst++ = (iB>0) ? (iB<65535 ? (unsigned char)(iB>>8):0xff):0;
        *dst++ = (iG>0) ? (iG<65535 ? (unsigned char)(iG>>8):0xff):0;
        *dst++ = (iR>0) ? (iR<65535 ? (unsigned char)(iR>>8):0xff):0;
    }
};
bool nv21_to_bgr(unsigned char* bgr, unsigned char const* nv21, int width, int height) {
    return decode_yuv<NV21toBGR>(bgr, nv21, width, height);
}

//------------------------------------------------------------------------------
class NV21toBGRA {
public:
    enum { bytes_per_pixel = 4 };
    static void loadvu(int& U, int& V, unsigned char const* &uv) {
        V = (*uv++) - 128;
        U = (*uv++) - 128;
    }
    static void store_pixel(unsigned char* &dst, int iR, int iG, int iB, unsigned char alpha) {
        *dst++ = (iB>0) ? (iB<65535 ? (unsigned char)(iB>>8):0xff):0;		
        *dst++ = (iG>0) ? (iG<65535 ? (unsigned char)(iG>>8):0xff):0;
        *dst++ = (iR>0) ? (iR<65535 ? (unsigned char)(iR>>8):0xff):0;
        *dst++ = alpha;
    }
};

bool nv21_to_bgra(unsigned char* rgba, unsigned char alpha, unsigned char const* nv21, int width, int height) {
    return decode_yuv<NV21toBGRA>(rgba, nv21, width, height, alpha);
}
#endif
