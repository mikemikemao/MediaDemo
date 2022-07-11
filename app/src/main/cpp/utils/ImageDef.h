//
// Created by pirate on 2022/3/8.
//

#ifndef FFMPEGDEMO_IMAGEDEF_H
#define FFMPEGDEMO_IMAGEDEF_H
#include <stdint.h>
#include "LogUtil.h"
#include <malloc.h>
#include <string.h>
#include <unistd.h>

#define IMAGE_FORMAT_RGBA           0x01
#define IMAGE_FORMAT_NV21           0x02
#define IMAGE_FORMAT_NV12           0x03
#define IMAGE_FORMAT_I420           0x04
#define IMAGE_FORMAT_YUYV           0x05
#define IMAGE_FORMAT_GRAY           0x06
#define IMAGE_FORMAT_I444           0x07
#define IMAGE_FORMAT_P010           0x08

#define IMAGE_FORMAT_RGBA_EXT       "RGB32"
#define IMAGE_FORMAT_NV21_EXT       "NV21"
#define IMAGE_FORMAT_NV12_EXT       "NV12"
#define IMAGE_FORMAT_I420_EXT       "I420"
#define IMAGE_FORMAT_YUYV_EXT       "YUYV"
#define IMAGE_FORMAT_GRAY_EXT       "GRAY"
#define IMAGE_FORMAT_I444_EXT       "I444"
#define IMAGE_FORMAT_P010_EXT       "P010" //16bit NV21

#define IMAGE_FORMAT_RGBA           0x01
#define IMAGE_FORMAT_NV21           0x02
#define IMAGE_FORMAT_NV12           0x03
#define IMAGE_FORMAT_I420           0x04

typedef struct _tag_NativeImage
{
    int width;
    int height;
    int format;
    uint8_t *ppPlane[3];
    int pLineSize[3];
    _tag_NativeImage()
    {
        width = 0;
        height = 0;
        format = 0;
        ppPlane[0] = nullptr;
        ppPlane[1] = nullptr;
        ppPlane[2] = nullptr;
        pLineSize[0] = 0;
        pLineSize[1] = 0;
        pLineSize[2] = 0;
    }
} NativeImage;

class NativeImageUtil {
public:
    static void FreeNativeImage(NativeImage *pImage) {
        if (pImage == nullptr || pImage->ppPlane[0] == nullptr) return;

        free(pImage->ppPlane[0]);
        pImage->ppPlane[0] = nullptr;
        pImage->ppPlane[1] = nullptr;
        pImage->ppPlane[2] = nullptr;
    }

    static void AllocNativeImage(NativeImage *pImage)
    {
        if (pImage->height == 0 || pImage->width == 0) return;

        switch (pImage->format)
        {
            case IMAGE_FORMAT_RGBA:
            {
                pImage->ppPlane[0] = static_cast<uint8_t *>(malloc(pImage->width * pImage->height * 4));
            }
                break;
            case IMAGE_FORMAT_YUYV:
            {
                pImage->ppPlane[0] = static_cast<uint8_t *>(malloc(pImage->width * pImage->height * 2));
            }
                break;
            case IMAGE_FORMAT_NV12:
            case IMAGE_FORMAT_NV21:
            {
                pImage->ppPlane[0] = static_cast<uint8_t *>(malloc(pImage->width * pImage->height * 1.5));
                pImage->ppPlane[1] = pImage->ppPlane[0] + pImage->width * pImage->height;
            }
                break;
            case IMAGE_FORMAT_I420:
            {
                pImage->ppPlane[0] = static_cast<uint8_t *>(malloc(pImage->width * pImage->height * 1.5));
                pImage->ppPlane[1] = pImage->ppPlane[0] + pImage->width * pImage->height;
                pImage->ppPlane[2] = pImage->ppPlane[1] + pImage->width * (pImage->height >> 2);
            }
                break;
            case IMAGE_FORMAT_GRAY:
            {
                pImage->ppPlane[0] = static_cast<uint8_t *>(malloc(pImage->width * pImage->height));
            }
                break;
            case IMAGE_FORMAT_I444:
            {
                pImage->ppPlane[0] = static_cast<uint8_t *>(malloc(pImage->width * pImage->height * 3));
            }
                break;
            case IMAGE_FORMAT_P010:
            {
                pImage->ppPlane[0] = static_cast<uint8_t *>(malloc(pImage->width * pImage->height * 3));
                pImage->ppPlane[1] = pImage->ppPlane[0] + pImage->width * pImage->height * 2;
            }
                break;
            default:
                LOGCATE("NativeImageUtil::AllocNativeImage do not support the format. Format = %d", pImage->format);
                break;
        }
    }

    static void CopyNativeImage(NativeImage *pSrcImg, NativeImage *pDstImg)
    {
        if(pSrcImg == nullptr || pSrcImg->ppPlane[0] == nullptr) return;

        if(pSrcImg->format != pDstImg->format ||
           pSrcImg->width != pDstImg->width ||
           pSrcImg->height != pDstImg->height) return;

        if(pDstImg->ppPlane[0] == nullptr) AllocNativeImage(pDstImg);

        switch (pSrcImg->format)
        {
            case IMAGE_FORMAT_I420:
            case IMAGE_FORMAT_NV21:
            case IMAGE_FORMAT_NV12:
            {
                memcpy(pDstImg->ppPlane[0], pSrcImg->ppPlane[0], pSrcImg->width * pSrcImg->height * 1.5);
            }
                break;
            case IMAGE_FORMAT_YUYV:
            {
                memcpy(pDstImg->ppPlane[0], pSrcImg->ppPlane[0], pSrcImg->width * pSrcImg->height * 2);
            }
                break;
            case IMAGE_FORMAT_RGBA:
            {
                memcpy(pDstImg->ppPlane[0], pSrcImg->ppPlane[0], pSrcImg->width * pSrcImg->height * 4);
            }
                break;
            case IMAGE_FORMAT_GRAY:
            {
                memcpy(pDstImg->ppPlane[0], pSrcImg->ppPlane[0], pSrcImg->width * pSrcImg->height);
            }
                break;
            case IMAGE_FORMAT_P010:
            case IMAGE_FORMAT_I444:
            {
                memcpy(pDstImg->ppPlane[0], pSrcImg->ppPlane[0], pSrcImg->width * pSrcImg->height * 3);
            }
                break;
            default:
            {
                LOGCATE("NativeImageUtil::CopyNativeImage do not support the format. Format = %d", pSrcImg->format);
            }
                break;
        }

    }


};



#endif //FFMPEGDEMO_IMAGEDEF_H
