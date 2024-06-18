#pragma once

#include <string>


namespace pk
{
    class ImageData
    {
    private:
        unsigned char* _pData = nullptr;
        int _width = -1;
        int _height = -1;
        int _channels = -1;

        bool _hasAlpha = false;

    public:
        ImageData() {}
        // NOTE: Pixels gets copied here, ownership doesnt transfer!
        ImageData(
            unsigned char* pixels,
            int width,
            int height,
            int channels
        );

        ~ImageData();
        void clearData();

        float getBrightnessAt(int x, int y);
        void setColorAt(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
        void setColorAt_UNSAFE(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a);

        inline unsigned char* getData() { return _pData; }
        inline int getWidth() const { return _width; }
        inline int getHeight() const { return _height; }
        inline int getChannels() const { return _channels; }

        inline bool hasAlpha() const { return _hasAlpha; }
    };

    ImageData* load_image(const std::string filePath);
}
