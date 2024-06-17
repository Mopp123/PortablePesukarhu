#pragma once

#include "graphics/Texture.h"
#include <unordered_map>

#include "utils/pkmath.h"

#include <ft2build.h>
#include FT_FREETYPE_H


/*
   Possible problems:
 * When creating new Font, we init freetype and also "clear"/"deallocate"(FT_Done_FreeType) freetype. This may be problem,
 when creating and deleting multiple fonts? (Not tested!)
 */

namespace pk
{
    struct FontGlyphData
    {
        // Offset in font texture atlas, where we get this glyph's visual
        int32_t texOffsetX = 0;
        int32_t texOffsetY = 0;

        // Scale of this glyph
        int32_t width = 0;
        int32_t height = 0;

        // Offset from baseline to left/top of glyph
        int32_t bearingX = 0;
        int32_t bearingY = 0;

        // Offset to advance to next glyph
        uint32_t advance = 0;
    };


    class Font
    {
    private:
        TextureAtlas* _textureAtlas = nullptr;
        unsigned int _pixelSize = 1;
        int _textureAtlasTileWidth = 1; // Width in pixels of a single tile inside the font's texture atlas.

        std::unordered_map<char, FontGlyphData> _glyphMapping;

        vec4 _color;

    public:
        // NOTE: Some "pixelSize" values doesn't work on some fonts!
        //  -> If text looks funky try some other pixelSize values.
        // TODO: Figure out way of knowing available sizes for fonts..
        Font(const std::string& fontFilePath, unsigned int pixelSize, vec4 color = vec4(1,1,1,1));
        ~Font();

        inline TextureAtlas* getTextureAtlas() const { return _textureAtlas; }
        inline const unsigned int getPixelSize() const { return _pixelSize; }
        inline std::unordered_map<char, FontGlyphData>& getGlyphMapping() { return _glyphMapping; }
        inline int getTilePixelWidth() const { return _textureAtlasTileWidth; }

        inline const vec4& getColor() const { return _color; }

    private:
        void createFont(const std::string& charsToLoad, const std::string& fontFilePath, unsigned int pixelSize);
    };
}
