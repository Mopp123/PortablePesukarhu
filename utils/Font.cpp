#include "Font.h"
#include "core/Debug.h"
#include "core/Application.h"
#include <cmath>
#include <vector>


namespace pk
{
    Font::Font(const std::string& filepath, unsigned int pixelSize) :
        Resource(ResourceType::RESOURCE_FONT, filepath),
        _pixelSize(pixelSize)
    {
    }

    Font::~Font()
    {}

    void Font::load()
    {
        createFont(
            "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890.,:;?!&_'+-*^/()[]{}äåöÄÅÖ"
        );
    }

    // TODO: Put into constructor -> doesnt need to be own func anymore!
    void Font::createFont(const std::string& charsToLoad)
    {
        std::string filepathStr = std::string(_filepath);
        FT_Library freetypeLib;
        if (FT_Init_FreeType(&freetypeLib))
        {
            Debug::log(
                "Location: void Font::createFont(const std::string& charsToLoad, const std::string& fontFilePath, unsigned int pixelSize)\n"
                "Failed to init Freetype library!",
                Debug::MessageType::PK_FATAL_ERROR
            );
            return;
        }

        // Load font file
        FT_Face fontFace;
        if (FT_New_Face(freetypeLib, filepathStr.c_str(), 0, &fontFace))
        {
            Debug::log(
                "@Font::createFont "
                "Failed to load font file from location: " + filepathStr,
                Debug::MessageType::PK_FATAL_ERROR
            );
            return;
        }

        if (FT_Set_Pixel_Sizes(fontFace, 0, _pixelSize))
        {
            Debug::log(
                "@Font::createFont Failed to set pixel sizes to: " + std::to_string(_pixelSize),
                Debug::MessageType::PK_FATAL_ERROR
            );
        }

        // Load data from each glyph(contained in "charsToLoad") of the font
        float charArraysqrt = std::sqrt((float)charsToLoad.size());
        int textureAtlasRowCount = (int)std::ceil(charArraysqrt);
        _textureAtlasTileWidth = 1; // width in pixels of a single tile inside the font texture atlas

        int texOffsetX = 0;
        int texOffsetY = 0;

        int maxGlyphWidth = 0;
        int maxGlyphHeight = 0;

        std::vector<std::pair<unsigned char*, FontGlyphData>> loadedGlyphs;

        for (int i = 0; i < charsToLoad.size(); i++)
        {
            char c = charsToLoad[i];

            if (FT_Load_Char(fontFace, c, FT_LOAD_RENDER))
            {
                Debug::log(
                    "@Font::createFont "
                    "Failed to load character '" + std::to_string(c) + "' from a font. Font file path was " + filepathStr,
                    Debug::MessageType::PK_FATAL_ERROR
                );
                continue;
            }

            FT_GlyphSlot ftGlyph = fontFace->glyph;
            int currentGlyphWidth = ftGlyph->bitmap.width;
            int currentGlyphHeight = ftGlyph->bitmap.rows;

            FontGlyphData gd =
            {
                texOffsetX,
                texOffsetY,

                currentGlyphWidth,
                currentGlyphHeight,

                ftGlyph->bitmap_left,
                ftGlyph->bitmap_top,

                (uint32_t)ftGlyph->advance.x
            };

            maxGlyphWidth = std::max(maxGlyphWidth, currentGlyphWidth);
            maxGlyphHeight = std::max(maxGlyphHeight, currentGlyphHeight);

            // If we have space -char -> don't store any bitmap data about it..
            if (c != ' ')
            {
                const int glyphBitmapSize = currentGlyphWidth * currentGlyphHeight;
                unsigned char* bitmap = new unsigned char[glyphBitmapSize];
                memcpy(bitmap, ftGlyph->bitmap.buffer, sizeof(unsigned char) * glyphBitmapSize);
                loadedGlyphs.push_back(std::make_pair(bitmap, gd));

                texOffsetX++;
                if (texOffsetX >= textureAtlasRowCount)
                {
                    texOffsetX = 0;
                    texOffsetY++;
                }
            }
            _glyphMapping.insert(std::make_pair(c, gd));
        }
        // We want each glyph in the texture atlas to have perfect square space, for simplicity's sake..
        // (This results in many unused pixels tho..)
        _textureAtlasTileWidth = std::max(maxGlyphWidth, maxGlyphHeight);

        // Combine all the loaded glyphs bitmaps into a single large texture atlas
        const unsigned int combinedGlyphBitmapWidth = textureAtlasRowCount * _textureAtlasTileWidth;
        const unsigned int combinedGlyphBitmapSize = combinedGlyphBitmapWidth * combinedGlyphBitmapWidth; // font texture atlas size in bytes

        unsigned char* combinedGlyphBitmap = new unsigned char[combinedGlyphBitmapSize];
        memset(combinedGlyphBitmap, 0, combinedGlyphBitmapSize);

        for (unsigned int apy = 0; apy < combinedGlyphBitmapWidth; ++apy)
        {
            for (unsigned int apx = 0; apx < combinedGlyphBitmapWidth; ++apx)
            {
                unsigned int glyphIndexX = std::floor(apx / _textureAtlasTileWidth);
                unsigned int glyphIndexY = std::floor(apy / _textureAtlasTileWidth);

                if (glyphIndexX + glyphIndexY * textureAtlasRowCount < (unsigned int)loadedGlyphs.size())
                {
                    std::pair<unsigned char*, FontGlyphData>& g = loadedGlyphs[glyphIndexX + glyphIndexY * textureAtlasRowCount];
                    if (g.first == nullptr)
                        continue;

                    unsigned int glyphPixelX = apx % _textureAtlasTileWidth;
                    unsigned int glyphPixelY = apy % _textureAtlasTileWidth;

                    if (glyphPixelX < g.second.width && glyphPixelY < g.second.height)
                        combinedGlyphBitmap[apx + apy * combinedGlyphBitmapWidth] = g.first[glyphPixelX + glyphPixelY * g.second.width];
                }
            }
        }
        // delete all heap allocated glyph bitmaps
        for (std::pair<unsigned char*, FontGlyphData>& p : loadedGlyphs)
        {
            delete p.first;
            p.first = nullptr;
        }

        // Create resources through ResourceManager
        Application* pApp = Application::get();
        if (!pApp)
        {
            Debug::log(
                "@Font::createFont Application was nullptr!",
                Debug::MessageType::PK_FATAL_ERROR
            );
        }
        ResourceManager& resourceManager = pApp->getResourceManager();

        ImageData* pFontImgData = resourceManager.createImage(
            combinedGlyphBitmap,
            combinedGlyphBitmapWidth,
            combinedGlyphBitmapWidth,
            1
        );
        _imgDataResourceID = pFontImgData->getResourceID();

        Texture_new* pTexture = resourceManager.createTexture(
            _imgDataResourceID,
            TextureSampler(
                TextureSamplerFilterMode::PK_SAMPLER_FILTER_MODE_LINEAR,
                TextureSamplerAddressMode::PK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
            )
        );
        _textureResourceID = pTexture->getResourceID();

        _textureAtlasRowCount = textureAtlasRowCount;

        FT_Done_Face(fontFace);
        FT_Done_FreeType(freetypeLib);
    }

    const Texture_new* Font::getTexture() const
    {
        return (Texture_new*)Application::get()->getResourceManager().getResource(_textureResourceID);
    }
}
