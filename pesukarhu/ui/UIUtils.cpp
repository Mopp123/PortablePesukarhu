#include "UIUtils.h"
#include "pesukarhu/core/Application.h"
#include "pesukarhu/core/Debug.h"


namespace pk
{
    namespace ui
    {
        float get_text_visual_width(const std::string& txt, const Font* pFont)
        {
            float txtWidth = 0.0f;
            float pos = 0.0f;
            for (char c : txt)
            {
                const FontGlyphData * const glyph = pFont->getGlyph(c);
                if (glyph)
                {
                    txtWidth = pos + glyph->bearingX;
                    pos += ((float)(glyph->advance >> 6));
                }
            }
            return txtWidth;
        }
    }
}
