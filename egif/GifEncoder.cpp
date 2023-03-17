//
// Created by xiaozhuai on 2020/12/20.
//

#include <string>
#include <vector>
#include <exception>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include "GifEncoder.h"
#include "giflib/gif_lib.h"
#include "algorithm/NeuQuant.h"

#define m_gifFile ((GifFileType *) m_gifFileHandler)
#define GifAddExtensionBlockFor(a, func, len, data) GifAddExtensionBlock(       \
        &((a)->ExtensionBlockCount),                                            \
        &((a)->ExtensionBlocks),                                                \
        func,                                                                   \
        len,                                                                    \
        data                                                                    \
    )

// MICHEL: changed to 4 byte RGBA pixels
static void getColorMap(uint8_t *colorMap, const uint8_t *pixels, int nPixels, int quality) {
    initnet(pixels, nPixels * 4, quality);
    learn();
    unbiasnet();
    inxbuild();
    getcolourmap(colorMap);
}

// MICHEL: changed to 4 byte RGBA pixels
static void getRasterBits(uint8_t *rasterBits, const uint8_t *pixels, int nPixels) {
    const int indexBlack = inxsearch(0, 0, 0);

    for (int i = 0; i < nPixels; ++i) {
        const int r = pixels[i * 4];
        const int g = pixels[i * 4 + 1];
        const int b = pixels[i * 4 + 2];
        rasterBits[i] = (b == 0 && g == 0 && r == 0) ? indexBlack : inxsearch(b, g, r);
    }
}

inline void RGB2BGR(uint8_t *dst, const uint8_t *src, int width, int height) {
    for (const uint8_t *dstEnd = dst + width * height * 3; dst < dstEnd; src += 3) {
        *(dst++) = *(src + 2);
        *(dst++) = *(src + 1);
        *(dst++) = *(src);
    }
}

inline void BGRA2BGR(uint8_t *dst, const uint8_t *src, int width, int height) {
    for (const uint8_t *dstEnd = dst + width * height * 3; dst < dstEnd; src += 4) {
        *(dst++) = *(src);
        *(dst++) = *(src + 1);
        *(dst++) = *(src + 2);
    }
}

inline void RGBA2BGR(uint8_t *dst, const uint8_t *src, int width, int height) {
    for (const uint8_t *dstEnd = dst + width * height * 3; dst < dstEnd; src += 4) {
        *(dst++) = *(src + 2);
        *(dst++) = *(src + 1);
        *(dst++) = *(src);
    }
}

static bool convertToBGR(GifEncoder::PixelFormat format, uint8_t *dst, const uint8_t *src, int width, int height) {
    switch (format) {
        case GifEncoder::PIXEL_FORMAT_BGR:
            memcpy(dst, src, width * height * 3);
            break;
        case GifEncoder::PIXEL_FORMAT_RGB:
            RGB2BGR(dst, src, width, height);
            break;
        case GifEncoder::PIXEL_FORMAT_BGRA:
            BGRA2BGR(dst, src, width, height);
            break;
        case GifEncoder::PIXEL_FORMAT_RGBA:
            RGBA2BGR(dst, src, width, height);
            break;
        default:
            return false;
    }
    return true;
}

bool GifEncoder::open(const std::string &file, int width, int height, int quality, int16_t loop) {
    if (m_gifFile != nullptr) {
        return false;
    }

    int error;
    m_gifFileHandler = EGifOpenFileName(file.c_str(), false, &error);
    if (!m_gifFile) {
        return false;
    }

    m_quality = quality;

    reset();

    m_gifFile->SWidth = width;
    m_gifFile->SHeight = height;
    m_gifFile->SColorResolution = 8;
    m_gifFile->SBackGroundColor = 0;
    m_gifFile->SColorMap = nullptr;

    uint8_t appExt[11] = {
            'N', 'E', 'T', 'S', 'C', 'A', 'P', 'E',
            '2', '.', '0'
    };
    uint8_t appExtSubBlock[3] = {
            0x01,       // hex 0x01
            0x00, 0x00  // little-endian short. The number of times the loop should be executed.
    };
    memcpy(appExtSubBlock + 1, &loop, sizeof(loop));

    GifAddExtensionBlockFor(m_gifFile, APPLICATION_EXT_FUNC_CODE, sizeof(appExt), appExt);
    GifAddExtensionBlockFor(m_gifFile, CONTINUE_EXT_FUNC_CODE, sizeof(appExtSubBlock), appExtSubBlock);

    if (EGifWriteHeader(m_gifFile) == GIF_ERROR) {
        EGifCloseFile(m_gifFile, &error);
        m_gifFileHandler = nullptr;
        return false;
    }

    if (EGifWriteExtBlocks(m_gifFile) == GIF_ERROR) {
        EGifCloseFile(m_gifFile, nullptr);
        m_gifFileHandler = nullptr;
        return false;
    }

    return true;
}

bool GifEncoder::push(const uint8_t *frame, int x, int y, int width, int height, int delay) {
    if (m_gifFile == nullptr) {
        return false;
    }

    if (frame == nullptr) {
        return false;
    }

    auto *colorMap = GifMakeMapObject(256, nullptr);
    getColorMap((uint8_t *) colorMap->Colors, frame, width * height, m_quality);

    auto *rasterBits = (GifByteType *) malloc(width * height);
    getRasterBits((uint8_t *) rasterBits, frame, width * height);

    if (!encodeFrame(x, y, width, height, delay, colorMap, rasterBits))
        return false;

    return true;
}

bool GifEncoder::close() {
    if (m_gifFile == nullptr) {
        return false;
    }

    int extCount = m_gifFile->ExtensionBlockCount;
    auto *extBlocks = m_gifFile->ExtensionBlocks;
    GifFreeExtensions(&extCount, &extBlocks);

    EGifCloseFile(m_gifFile, nullptr);
    m_gifFileHandler = nullptr;
    reset();

    return true;
}

bool GifEncoder::encodeFrame(int x, int y, int width, int height, int delay, void *colorMap, void *rasterBits) {
    auto *gifImage = GifMakeSavedImage(m_gifFile, nullptr);

    gifImage->ImageDesc.Left = x;
    gifImage->ImageDesc.Top = y;
    gifImage->ImageDesc.Width = width;
    gifImage->ImageDesc.Height = height;
    gifImage->ImageDesc.Interlace = false;
    gifImage->ImageDesc.ColorMap = (ColorMapObject *) colorMap;
    gifImage->RasterBits = (GifByteType *) rasterBits;
    gifImage->ExtensionBlockCount = 0;
    gifImage->ExtensionBlocks = nullptr;

    GraphicsControlBlock gcb;
    gcb.DisposalMode = DISPOSE_DO_NOT;
    gcb.UserInputFlag = false;
    gcb.DelayTime = delay;
    gcb.TransparentColor = NO_TRANSPARENT_COLOR;
    uint8_t gcbBytes[4];
    EGifGCBToExtension(&gcb, gcbBytes);
    GifAddExtensionBlockFor(gifImage, GRAPHICS_EXT_FUNC_CODE, sizeof(gcbBytes), gcbBytes);

    if (EGifWritePictures(m_gifFile) == GIF_ERROR) {
        EGifCloseFile(m_gifFile, nullptr);
        m_gifFileHandler = nullptr;
        return false;
    }

    GifFreeSavedImages(m_gifFile);
    return true;
}
