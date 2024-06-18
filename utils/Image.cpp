#include "Image.h"
#include <cstddef>
#include <cstring>
#include "core/Debug.h"
#include "graphics/Context.h"
#include "Common.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


namespace pk
{
    ImageData* load_image(const std::string filePath)
    {
        int width = 0;
        int height = 0;
        int channels = 0;
        bool flipVertically = Context::get_api_type() == GRAPHICS_API_WEBGL;

        stbi_set_flip_vertically_on_load(flipVertically);
        unsigned char* stbImageData = stbi_load(filePath.c_str(), &width, &height, &channels, 0);

        if (!stbImageData)
        {
            Debug::log(
                "@load_image "
                "Failed to load image from location : " + filePath,
                Debug::MessageType::PK_FATAL_ERROR
            );
            stbi_image_free(stbImageData);
            return nullptr;
        }

        ImageData* imgData = new ImageData(stbImageData, width, height, channels);
        stbi_image_free(stbImageData);
        return imgData;
    }


    ImageData::ImageData(unsigned char* pixels, int width, int height, int channels) :
        _width(width), _height(height), _channels(channels)
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
