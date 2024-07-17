#include "Image.h"
#include <cstddef>
#include <cstring>
#include "core/Debug.h"
#include "graphics/Context.h"
#include "Common.h"

// Currently STB_IMAGE_IMPLEMENTATION is defined in ModelLoading.cpp since tinygltf requires stb_image
//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


namespace pk
{
    ImageData::ImageData(const std::string& filepath) :
        Resource(ResourceType::RESOURCE_IMAGE, filepath)
    {}

    ImageData::ImageData(
        unsigned char* pixels,
        int width,
        int height,
        int channels
    ) :
        Resource(ResourceType::RESOURCE_IMAGE),
        _width(width),
        _height(height),
        _channels(channels)
    {
        _hasAlpha = channels >= 4;

        size_t dataSize = _width * _height * _channels;
        _pData = new unsigned char[dataSize];
        memset(_pData, 0, dataSize);
        memcpy(_pData, pixels, dataSize);
    }

    ImageData::~ImageData()
    {
        delete[] _pData;
    }

    void ImageData::load()
    {
        if (_loaded)
        {
            Debug::log(
                "@ImageData::load "
                "Resource was already loaded. Requires freeing before new load can occur",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return;
        }
        int width = 0;
        int height = 0;
        int channels = 0;
        bool flipVertically = Context::get_api_type() == GRAPHICS_API_WEBGL;

        stbi_set_flip_vertically_on_load(flipVertically);
        // TODO: Test if below works, may fuck up due to MAX_FILEPATH_SIZE?
        unsigned char* stbImageData = stbi_load(_filepath, &width, &height, &channels, 0);

        if (!stbImageData)
        {
            Debug::log(
                "@ImageData::load "
                "Failed to load image from location : " + getFilepathStr(),
                Debug::MessageType::PK_FATAL_ERROR
            );
            stbi_image_free(stbImageData);
        }

        _width = width;
        _height = height;
        _channels = channels;
        _hasAlpha = channels >= 4;

        size_t dataSize = width * height * channels;
        _pData = new unsigned char[dataSize];
        memset(_pData, 0, dataSize);
        memcpy(_pData, stbImageData, dataSize);

        stbi_image_free(stbImageData); // NOTE: Not sure if required?
        _loaded = true;
    }

    void ImageData::save()
    {
        Debug::notify_unimplemented("ImageData::save");
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
