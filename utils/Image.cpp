#include "Image.h"
#include <cstddef>
#include <cstring>
#include "core/Debug.h"
#include "Common.h"

#ifdef PK_PLATFORM_WEB
    #include <SDL_image.h>
#endif

namespace pk
{
    #ifdef PK_PLATFORM_WEB
    ImageData* load_image(const std::string filePath)
    {
        SDL_Surface* surface = IMG_Load(filePath.c_str());
        if (surface == NULL)
        {
            Debug::log(
                "Failed to create SDL surface from: " + filePath + " SDL_image error: " + IMG_GetError(),
                Debug::MessageType::PK_FATAL_ERROR
            );
            return nullptr;
        }
        // NOTE: Not tested if this is actually correct!
        int channels = (int)surface->format->BytesPerPixel;
        ImageData* imgData = new ImageData(
            (unsigned PK_byte*)surface->pixels,
            surface->w,
            surface->h,
            channels,
            false
        );

        SDL_FreeSurface(surface);
    }
    #else
    ImageData* load_image(const std::string filePath)
    {
        Debug::log(
            "Failed to load image due to invalid platform definition",
            Debug::MessageType::PK_FATAL_ERROR
        );
        return nullptr;
    }
    #endif


    ImageData::ImageData(unsigned char* pixels, int width, int height, int channels, bool isFlipped) :
        _width(width), _height(height), _channels(channels),
        _isFlipped(isFlipped)
    {
        size_t dataSize = width * height * channels;
        _pData = new unsigned char[dataSize];
        memset(_pData, 0, dataSize);
        memcpy(_pData, pixels, dataSize);

        _hasAlpha = _channels >= 4;
    }

    ImageData::~ImageData()
    {
        clearData();
    }

    void ImageData::clearData()
    {
        delete[] _pData;
        _pData = nullptr;
    }

    float ImageData::getBrightnessAt(int x, int y)
    {
        int index = (x + y * _width) * _channels;
        float b = ((float)_pData[index]) / 255.0f;
        return b;
    }

    void ImageData::setColorAt(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    {
        int index_r = (x + y * _width) * _channels;
        int index_g = (x + y * _width) * _channels + 1;
        int index_b = (x + y * _width) * _channels + 2;
        int index_a = (x + y * _width) * _channels + 3;

        size_t imgDatSize = _width * _height * _channels;

        if ((index_r < 0 || index_r >= imgDatSize) ||
                (index_g < 0 || index_g >= imgDatSize) ||
                (index_b < 0 || index_b >= imgDatSize) ||
                (index_a < 0 || index_a >= imgDatSize)
           )
        {
            Debug::log(
                "Failed to set ImageData's data at specific index!\n"
                "Img dimensions were: " + std::to_string(_width) + "x" + std::to_string(_height) + "\n"
                "Accessed coordinates were: " + std::to_string(x) + "," + std::to_string(y) + "\n"
                "At the moment all modified ImageDatas has to have 4 color components(r,g,b,a channels)",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return;
        }

        _pData[index_r] = r;
        _pData[index_g] = g;
        _pData[index_b] = b;
        _pData[index_a] = a;
    }

    void ImageData::setColorAt_UNSAFE(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    {
        int index_r = (x + y * _width) * _channels;
        int index_g = (x + y * _width) * _channels + 1;
        int index_b = (x + y * _width) * _channels + 2;
        int index_a = (x + y * _width) * _channels + 3;

        _pData[index_r] = r;
        _pData[index_g] = g;
        _pData[index_b] = b;
        _pData[index_a] = a;
    }
}
