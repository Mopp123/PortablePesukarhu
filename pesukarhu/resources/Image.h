#pragma once

#include "Resource.h"
#include <string>


namespace pk
{
    class ImageData : public Resource
    {
    private:
        unsigned char* _pData = nullptr;
        int _width = -1;
        int _height = -1;
        int _channels = -1;

        bool _hasAlpha = false;
        bool _flip = false;

    public:
        ImageData(const std::string& filepath, bool flip = false);
        ImageData(
            unsigned char* pixels,
            int width,
            int height,
            int channels
        );
        ~ImageData();

        virtual bool load();
        virtual bool save();

        void clearData();

        float getBrightnessAt(int x, int y) const;
        void setColorAt(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
        void setColorAt_UNSAFE(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
        void setColorAt_UNSAFE(int pixelIndex, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
        // For single channel images
        // NOTE: No range checking! USE AT YOUR OWN RISK!
        void setColorAt_UNSAFE(int x, int y, unsigned char val);

        inline const unsigned char* getData() const { return _pData; }
        inline unsigned char* getData() { return _pData; }
        inline int getWidth() const { return _width; }
        inline int getHeight() const { return _height; }
        inline int getChannels() const { return _channels; }
        // Returns total size in bytes
        inline size_t getSize() const { return _width * _height * _channels; }

        inline bool hasAlpha() const { return _hasAlpha; }
    };
}
