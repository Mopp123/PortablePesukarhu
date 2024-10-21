#pragma once

#include "Resource.h"
#include "Texture.h"
#include <unordered_map>

#include "utils/pkmath.h"

// For some reason this is bitching with vim+Ycm
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


    class Font : public Resource
    {
    private:
        uint32_t _imgDataResourceID = 0;
        uint32_t _textureResourceID = 0;

        unsigned int _pixelSize = 1;
        int _textureAtlasRowCount = 1;
        int _textureAtlasTileWidth = 1; // Width in pixels of a single tile inside the font's texture atlas.

        std::unordered_map<char, FontGlyphData> _glyphMapping;

    public:
        // NOTE: Some "pixelSize" values doesn't work on some fonts!
        //  -> If text looks funky try some other pixelSize values.
        // TODO: Figure out way of knowing available sizes for fonts..
        Font(const std::string& filepath, unsigned int pixelSize);
        ~Font();

        virtual void load();
        virtual void save() {}

        Texture* accessTexture();
        const Texture* getTexture() const;
        const FontGlyphData * const getGlyph(char c) const;
        inline const unsigned int getPixelSize() const { return _pixelSize; }
        inline std::unordered_map<char, FontGlyphData>& getGlyphMapping() { return _glyphMapping; }
        inline const std::unordered_map<char, FontGlyphData>& getGlyphMapping() const { return _glyphMapping; }
        inline int getTextureAtlasRowCount() const { return _textureAtlasRowCount; }
        inline int getTilePixelWidth() const { return _textureAtlasTileWidth; }

    private:
        void createFont(const std::string& charsToLoad);
    };
}
