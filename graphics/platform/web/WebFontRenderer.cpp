#include <ft2build.h>
#include <unordered_map>
#include FT_FREETYPE_H


#include "WebFontRenderer.h"

#include "../../../core/Timing.h"
#include "../../../utils/Algorithms.h"
#include "../../../Common.h"

#include <algorithm>
#include <iostream>

namespace pk
{
    namespace web
    {

        static std::string s_vertexSource = R"(
            precision mediump float;

            attribute vec2 position;
            attribute vec2 uv;
            attribute vec4 color;
	    attribute float thickness;

	    uniform mat4 projectionMatrix;
	    uniform float texAtlasRows;

	    varying vec2 var_uv;
	    varying vec4 var_color;
	    varying float var_thickness;

            void main()
            {
                int xPos = int(position.x);
                int yPos = int(position.y);
	        gl_Position = projectionMatrix * vec4(float(xPos), float(yPos), 0, 1.0);
	    	var_uv = uv / texAtlasRows;
	    	var_color = color;
	    	var_thickness = thickness;
	    }
        )";

        static std::string s_fragmentSource = R"(
            precision mediump float;

	    varying vec2 var_uv;
	    varying vec4 var_color;
	    varying float var_thickness;

	    uniform sampler2D textureSampler;

            void main()
            {
	        vec4 texColor = texture2D(textureSampler, var_uv);

	        vec4 finalColor = var_color * texColor.a * var_thickness;


	        if(texColor.a <= 0.0)
	        {
	            discard;
	        }

	        gl_FragColor = vec4(finalColor.rgb, 1.0);
            }
        )";

        static float s_TEST_anim = 1.0f;


        WebFontRenderer::WebFontRenderer() :
            _shader(s_vertexSource, s_fragmentSource)
        {
            _vertexAttribLocation_pos = _shader.getAttribLocation("position");
            _vertexAttribLocation_uv = _shader.getAttribLocation("uv");
            _vertexAttribLocation_color = _shader.getAttribLocation("color");
            _vertexAttribLocation_thickness = _shader.getAttribLocation("thickness");

            _uniformLocation_projMat = _shader.getUniformLocation("projectionMatrix");
            _uniformLocation_texAtlasRows = _shader.getUniformLocation("texAtlasRows");
            _uniformLocation_texSampler = _shader.getUniformLocation("textureSampler");


            std::vector<GlyphData> glyphs = createGlyphs("qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890.,:;?!&_'+-*^/()[]{}‰Âˆƒ≈÷", "assets/Ubuntu-R.ttf");

            _fontTexAtlas = createFontTextureAtlas(glyphs);

            // Create character mapping..
            for (const GlyphData& glyph : glyphs)
            {
                _characterMapping.insert(
                        std::pair<unsigned char, CharacterData>(
                            glyph.character,
                            { glyph.visualWidth, glyph.visualHeight, glyph.bearingX, glyph.bearingY, glyph.advance, glyph.texOffsetX, glyph.texOffsetY }
                            )
                        );
            }

            // TODO: Smaller batch size and automatically add to the next available batch
            // if attempted batch was full!
            const int maxBatchInstanceCount = 1000; // NOTE: This should be smaller
            const int batchInstanceDataLen = 9 * 4;

            allocateBatches(4, maxBatchInstanceCount * batchInstanceDataLen, batchInstanceDataLen);
        }

        WebFontRenderer::~WebFontRenderer()
        {
            for (BatchData& batch : _batches)
            {
                for (VertexBuffer* vb : batch.vertexBuffers)
                    delete vb;

                delete batch.indexBuffer;
            }

            delete _fontTexAtlas;
        }

        // submit renderable component for rendering (batch preparing, before rendering)
        void WebFontRenderer::submit(const Component* const renderableComponent, const mat4& transformation)
        {
            // <#M_DANGER>
            const TextRenderable* const  renderable = (const TextRenderable* const)(renderableComponent);

            const int maxBatchLen = _batches[0].maxTotalBatchDataLen;
            const int strDataLen = renderable->getStr().size() * _batches[0].instanceDataLen;
            if (strDataLen >= maxBatchLen)
            {
                Debug::log("(@WebFontRenderer::submit) Attempted to submit too long string for rendering\nCurrent limit is " + std::to_string(maxBatchLen) + "\nAttempted: " + std::to_string(strDataLen), Debug::MessageType::PK_ERROR);
                return;
            }

            int identifier = 1;

            // Find suitable batch
            for (int i = 0; i < _batches.size(); ++i)
            {
                BatchData& batch = _batches[i];

                int newSize = batch.currentDataPtr + batch.instanceDataLen * renderable->getStr().size();

                if (newSize >= maxBatchLen)
                {
                    continue;
                }

                if (batch.isFree)
                {
                    batch.occupy(identifier);// occupyBatch(batch, identifier);
                    addToBatch(batch, renderable, transformation);
                    //Debug::log("created new batch");
                    return;
                }
                else if (batch.ID == identifier)
                {
                    addToBatch(batch, renderable, transformation);
                    //Debug::log("assigned to existing");
                    return;
                }
            }

            Debug::log("(@WebFontRenderer::submit) No batches available", Debug::MessageType::PK_ERROR);
        }


        void WebFontRenderer::render(const Camera& cam)
        {
            s_TEST_anim += 0.125f * Timing::get_delta_time();

            mat4 projectionMatrix = cam.getProjMat2D();

            glUseProgram(_shader.getProgramID());

            // all common uniforms..
            _shader.setUniform(_uniformLocation_projMat, projectionMatrix);
            _shader.setUniform(_uniformLocation_texAtlasRows, _fontAtlasRowCount);

            //glDisable(GL_DEPTH_TEST);
            glDepthFunc(GL_ALWAYS);

            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, _fontTexAtlas->getID());


            for (BatchData& batch : _batches)
            {
                if (batch.isFree)
                    continue;

                WebVertexBuffer* vb = (WebVertexBuffer*)batch.vertexBuffers[0];
                //WebVertexBuffer* vb_uv = (WebVertexBuffer*)batch.vertexBuffers[1];

                WebIndexBuffer* ib = (WebIndexBuffer*)batch.indexBuffer;
                // hardcoded just as temp..
                const GLsizei instanceIndexCount = 6;

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->getID());

                int stride = sizeof(float) * 9;
                glBindBuffer(GL_ARRAY_BUFFER, vb->getID());

                // position vertex attrib
                glEnableVertexAttribArray(_vertexAttribLocation_pos);
                glVertexAttribPointer(_vertexAttribLocation_pos, 2, PK_ShaderDatatype::PK_FLOAT, GL_FALSE, stride, 0);

                // uv coord vertex attrib
                glEnableVertexAttribArray(_vertexAttribLocation_uv);
                glVertexAttribPointer(_vertexAttribLocation_uv, 2, PK_ShaderDatatype::PK_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 2));

                // color vertex attrib
                glEnableVertexAttribArray(_vertexAttribLocation_color);
                glVertexAttribPointer(_vertexAttribLocation_color, 4, PK_ShaderDatatype::PK_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 4));

                // thickness attrib
                glEnableVertexAttribArray(_vertexAttribLocation_thickness);
                glVertexAttribPointer(_vertexAttribLocation_thickness, 1, PK_ShaderDatatype::PK_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 8));


                glDrawElements(GL_TRIANGLES, instanceIndexCount * batch.getInstanceCount(), GL_UNSIGNED_SHORT, 0);


                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

                // JUST TEMPORARELY FREE HERE!!!
                batch.clear();
            }
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        std::vector<GlyphData> WebFontRenderer::createGlyphs(std::string characters, std::string fontFilePath)
        {
            std::vector<GlyphData> glyphs;
            glyphs.reserve(characters.size());

            FT_Library ftLib;
            if (FT_Init_FreeType(&ftLib))
            {
                Debug::log("Failed to init FreeType library", Debug::MessageType::PK_FATAL_ERROR);
                return glyphs;
            }

            FT_Face fontFace;
            if (FT_New_Face(ftLib, fontFilePath.c_str(), 0, &fontFace))
            {
                Debug::log("Failed to create FT_Face from: " + fontFilePath, Debug::MessageType::PK_FATAL_ERROR);
                FT_Done_FreeType(ftLib);
                return glyphs;
            }

            FT_Set_Pixel_Sizes(fontFace, _fontAtlasPixelSize, _fontAtlasPixelSize);

            std::string::const_iterator it;
            for (it = characters.begin(); it != characters.end(); it++)
            {
                unsigned char c = *it;

                FT_UInt glyphIndex = FT_Get_Char_Index(fontFace, c);
                if (FT_Load_Glyph(fontFace, glyphIndex, FT_LOAD_DEFAULT))
                {
                    Debug::log("Failed to load character from font(@FontRenderer::createGlyphs)", Debug::MessageType::PK_FATAL_ERROR);
                    continue;
                }

                FT_Error error = FT_Render_Glyph(fontFace->glyph, FT_Render_Mode::FT_RENDER_MODE_NORMAL);
                if (error)
                {
                    Debug::log("Hups... @WebFontRenderer", Debug::MessageType::PK_WARNING);
                }
                /*
                   if (FT_Load_Char(fontFace, c, FT_LOAD_RENDER))
                   {
                   Debug::log_error("Failed to load character from font(@FontRenderer::createGlyphs)");
                   continue;
                   }
                   */

                unsigned int glyphWidth = fontFace->glyph->bitmap.width;
                unsigned int glyphHeight = fontFace->glyph->bitmap.rows;


                // We need to copy the bitmap for later use when we combine all glyphs into a single texture
                // *NOTE! Atm we convert the bitmaps to 4 color channel component img data, because.. THAT Vulkan format img issue thing..
                unsigned int channels = 1;

                unsigned int bmpDataWidth = glyphWidth * channels;
                unsigned int bmpDataHeight = glyphHeight;

                unsigned char* bmp = new unsigned char[bmpDataWidth * bmpDataHeight * channels];
                memset(bmp, 0, bmpDataWidth * bmpDataHeight * channels);

                for (int py = 0; py < bmpDataHeight; ++py)
                {
                    for (int px = 0; px < bmpDataWidth; px += channels)
                    {
                        bmp[px + py * bmpDataWidth] = fontFace->glyph->bitmap.buffer[(px / channels) + py * glyphWidth];
                    }
                }

                _fontMaxBearingY = std::max(_fontMaxBearingY, (int)fontFace->glyph->bitmap_top);

                glyphs.push_back(
                        {
                        c,
                        bmp,
                        glyphWidth,
                        glyphHeight,
                        (int)fontFace->glyph->bitmap_left,
                        (int)fontFace->glyph->bitmap_top,
                        (unsigned int)fontFace->glyph->advance.x,
                        0,0,
                        0,0
                        }
                        );
            }

            FT_Done_Face(fontFace);
            FT_Done_FreeType(ftLib);
            Debug::log("Font loading finished");
            return glyphs;
        }


        WebTexture* WebFontRenderer::createFontTextureAtlas(std::vector<GlyphData>& glyphs)
        {
            int bitmapColorChannels = 1;

            unsigned int cellWidth = 0;
            _fontAtlasMaxCellHeight = 0;

            // First find the glyph that is the widest to get maxWidth and tallest to get maxHeight
            for (const GlyphData& gd : glyphs)
            {
                cellWidth = std::max(cellWidth, gd.width);
                _fontAtlasMaxCellHeight = std::max(_fontAtlasMaxCellHeight, gd.height);
            }
            // We want each cell to be perfect square
            cellWidth = get_next_pow2(std::max(cellWidth, _fontAtlasMaxCellHeight));
            _fontAtlasMaxCellHeight = get_next_pow2(std::max(cellWidth, _fontAtlasMaxCellHeight));

            // total length in pixels of the combined "final" font texture atlas
            unsigned int bitmapPixelsLength = (cellWidth * _fontAtlasMaxCellHeight) * glyphs.size();
            // *We require our textures to be always pow2s...
            unsigned int bitmapPixelsWidth = get_next_pow2((unsigned int)std::sqrt((double)bitmapPixelsLength));

            // ..Use that to get the actual final size of the texture *NOTE! we need to also add the other color channels here..
            unsigned char* bitmapFont = new unsigned char[bitmapPixelsWidth * bitmapPixelsWidth * bitmapColorChannels];
            memset(bitmapFont, 0, sizeof(unsigned char) * bitmapPixelsWidth * bitmapPixelsWidth * bitmapColorChannels);

            _fontAtlasRowCount = bitmapPixelsWidth / cellWidth;

            // Then combine all the glyphs into a single "image"
            // ..we go through each glyph .. "scanline style"
            for (int bpy = 0; bpy < bitmapPixelsWidth; bpy += cellWidth)
            {
                for (int bpx = 0; bpx < bitmapPixelsWidth; bpx += cellWidth)
                {
                    int glyphIndexX = std::floor((double)bpx / (double)cellWidth);
                    int glyphIndexY = std::floor((double)bpy / (double)cellWidth);
                    int glyphsIndex = glyphIndexX + glyphIndexY * _fontAtlasRowCount;
                    if (glyphsIndex < glyphs.size())
                    {
                        GlyphData& currentGlyph = glyphs[glyphsIndex];
                        currentGlyph.texOffsetX = (float)glyphIndexX;
                        currentGlyph.texOffsetY = (float)glyphIndexY;
                        currentGlyph.visualWidth = currentGlyph.width; //+ (cellWidth - currentGlyph.width);
                        currentGlyph.visualHeight = currentGlyph.height; //+ (_fontAtlasMaxCellHeight - currentGlyph.height);


                        // For each pixel in the glyph
                        for (int gpy = 0; gpy < currentGlyph.height; gpy++)
                        {
                            for (int gpx = 0; gpx < currentGlyph.width; gpx++)
                            {
                                bitmapFont[(bpx * bitmapColorChannels + gpx * bitmapColorChannels) + (bpy + gpy) * (bitmapPixelsWidth * bitmapColorChannels)] = currentGlyph.bitmap[gpx * bitmapColorChannels + gpy * (currentGlyph.width * bitmapColorChannels)];
                            }
                        }
                    }
                }
            }

            WebTexture* texture = new WebTexture(
                    bitmapFont, bitmapPixelsWidth, bitmapPixelsWidth, bitmapColorChannels,
                    {
                    TextureSamplerFilterMode::PK_SAMPLER_FILTER_MODE_LINEAR,
                    TextureSamplerAddressMode::PK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER
                    }
                    );
            delete[] bitmapFont;
            return texture;
        }



        void WebFontRenderer::allocateBatches(int maxBatchCount, int maxBatchLength, int entryLength)
        {
            std::vector<float> vertexData(maxBatchLength);

            const int instanceVertexCount = 4;
            std::vector<PK_ushort> indices(maxBatchLength);

            int vertexIndex = 0;
            for (int i = 0; i < indices.size(); i += 6)
            {
                if (i + 5 >= indices.size())
                    break;

                indices[i] =	 0 + vertexIndex;
                indices[i + 1] = 1 + vertexIndex;
                indices[i + 2] = 2 + vertexIndex;
                indices[i + 3] = 2 + vertexIndex;
                indices[i + 4] = 3 + vertexIndex;
                indices[i + 5] = 0 + vertexIndex;

                vertexIndex += instanceVertexCount;
            }

            for (int i = 0; i < maxBatchCount; ++i)
            {
                WebVertexBuffer* vb = new WebVertexBuffer(vertexData, VertexBufferUsage::PK_BUFFER_USAGE_DYNAMIC);
                WebIndexBuffer* ib = new WebIndexBuffer(indices);

                _batches.push_back({ entryLength, maxBatchLength, {vb}, ib });
            }
        }

        void WebFontRenderer::addToBatch(BatchData& batch, const TextRenderable* const renderable, const mat4& transform)
        {
            //memcpy((&batch.vertexBuffer->accessRawData()[0]) + batch.currentDataPtr, &data[0], sizeof(float) * batch.instanceDataLen);
            vec2 position(transform[0 + 3 * 4], transform[1 + 3 * 4]);
            const vec3& color = renderable->color;
            const float thickness = renderable->isBold() ? 3.0f : 1.0f;

            float charWidth = _fontAtlasPixelSize;
            float charHeight = _fontAtlasPixelSize;


            float scaleFactorX = 0.625f; // 0.5f + (std::sin(s_TEST_anim) + 1.0f) * 0.5f;// 1.0f; // Atm scaling is disabled with webgl...
            float scaleFactorY = 0.625f; // 0.5f + (std::sin(s_TEST_anim) + 1.0f) * 0.5f;// 1.0f;

            const float originalX = position.x - (float)_fontAtlasPixelSize * (scaleFactorX * 0.5f);
            float posX = originalX;
            float posY = (int)position.y - _fontMaxBearingY / 2;

            // NOTE:
            float cw = (float)charWidth * scaleFactorX;
            float ch = (float)charHeight * scaleFactorY;

            for (const char c : renderable->getStr())
            {
                // JUST DEBUG TESTING!
                // Make sure we are still in range...
                if (batch.currentDataPtr >= batch.maxTotalBatchDataLen)
                {
                    Debug::log("ERROR FOUND!!!");
                    break;
                }

                // Check, do we want to change line?
                if (c == '\n')
                {
                    posY -= charHeight * scaleFactorY;
                    posX = originalX;
                    continue;
                }
                // If this was just an empy space -> add to next posX and continue..
                else if (c == ' ')
                {
                    posX += (charWidth * 0.25f) * scaleFactorX;
                    continue;
                }
                // Make sure not draw nulldtm chars accidentally..
                else if (c == 0)
                {
                    break;
                }

                const CharacterData& charData = _characterMapping[c];

                float x = (posX + ((float)charData.bearingX) * 0.5f); //* scaleFactorX;
                // NOTE: For some reason on some fonts below needs to be divided by 2 or smthn
                // to make it look right.. Not sure why..
                float y = posY + (float)charData.bearingY * scaleFactorY;

                std::vector<float> vertexData = {
                    x, y - ch,		charData.texOffsetX,	 charData.texOffsetY + 1,	color.x,color.y,color.z,1.0f, thickness,
                    x, y,			charData.texOffsetX,	 charData.texOffsetY,		color.x,color.y,color.z,1.0f, thickness,
                    x + cw, y,		charData.texOffsetX + 1, charData.texOffsetY,		color.x,color.y,color.z,1.0f, thickness,
                    x + cw, y - ch, charData.texOffsetX + 1, charData.texOffsetY + 1,	color.x,color.y,color.z,1.0f, thickness
                };

                batch.insertInstanceData(0, vertexData);

                batch.addNewInstance();
                // NOTE: Don't quite understand this.. For some reason on some fonts >> 7 works better...
                posX += ((float)(charData.advance >> 6)) * scaleFactorX; // now advance cursors for next glyph (note that advance is number of 1/64 pixels). bitshift by 6 to get value in pixels (2^6 = 64) | OLD COMMENT: 2^5 = 32 (pixel size of the font..)
            }
        }

    }
}
