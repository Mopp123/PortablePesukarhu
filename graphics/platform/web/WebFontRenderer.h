#pragma once

#include "../../Renderer.h"
#include "WebTexture.h"

#include "../../../ecs/components/renderable/TextRenderable.h"
#include "WebBuffers.h"
#include "shaders/WebShader.h"

#include "../../../core/Debug.h"

#include <vector>
#include <unordered_map>

namespace pk
{
	namespace web
	{
		struct GlyphData
		{
		private:

			static const unsigned int s_bitmapColorChannels = 4;

		public:

			unsigned char character;
			unsigned char* bitmap = nullptr;
			unsigned int width, height;
			int bearingX, bearingY;
			unsigned int advance;
			float texOffsetX, texOffsetY;
			unsigned int visualWidth, visualHeight;

			// *EXPLANATION! Bitmap ownership ambiguity between constructor and copyconstructor:
			// ---------------------------------------------------------------------------------
			//		When passing bitmap as raw pointer it was constructed "the raw way" 
			//			-> so its ownership gets transferred here, and the "raw bitmap's" lifetime is tied to this GlyphData object.
			//		If GlyphData is copied, we can safely assume that the one we copy into this will eventually go out of scope -> destroying its bitmap data
			//			-> so we create new bitmapdata in this Glyph and copy the other Glyph's bitmapdata into it(Not changing ownerships!)

			// *NOTE! Ownership of the bitmap gets transferred here!
			GlyphData(
				unsigned char character,
				unsigned char* bitmap,
				unsigned int width, unsigned int height,
				int bearingX, int bearingY,
				unsigned int advance,
				float texOffsetX, float texOffsetY,
				unsigned int visualWidth, unsigned int visualHeight
			) :
				character(character),
				bitmap(bitmap),
				width(width), height(height),
				bearingX(bearingX), bearingY(bearingY),
				advance(advance),
				visualWidth(visualWidth), visualHeight(visualHeight)
			{}

			// *NOTE! Bitmap gets copied here! (Ownership of the bitmap DOES NOT get transferred using this copy constructor!!!)
			GlyphData(const GlyphData& other) :
				character(other.character),
				width(other.width), height(other.height),
				bearingX(other.bearingX), bearingY(other.bearingY),
				advance(other.advance),
				texOffsetX(other.texOffsetX), texOffsetY(other.texOffsetY),
				visualWidth(other.visualWidth), visualHeight(other.visualHeight)
			{
				const unsigned int bitmapDataLength = width * height * s_bitmapColorChannels;
				bitmap = new unsigned char[bitmapDataLength];
				memcpy(bitmap, other.bitmap, sizeof(unsigned char) * bitmapDataLength);
			}

			~GlyphData()
			{
				delete[] bitmap;
			}
		};

		struct CharacterData
		{
			unsigned int width, height;
			int bearingX, bearingY;
			unsigned int advance;
			float texOffsetX, texOffsetY;
			unsigned int actualHeight;
		};
		
		class WebFontRenderer : public Renderer
		{
		private:

			WebShader _shader;

			PK_int _vertexAttribLocation_pos = -1;
			PK_int _vertexAttribLocation_uv = -1;
			PK_int _vertexAttribLocation_color = -1;
			PK_int _vertexAttribLocation_thickness = -1;

			PK_int _uniformLocation_projMat = -1;
			PK_int _uniformLocation_texAtlasRows = -1;
			PK_int _uniformLocation_texSampler = -1;

			int _fontAtlasPixelSize = 32;

			unsigned int _fontAtlasRowCount = 1;
			unsigned int _fontAtlasMaxCellHeight = 0;
			int _fontMaxBearingY = 0;

			std::unordered_map<unsigned char, CharacterData> _characterMapping;

			WebTexture* _fontTexAtlas;

			std::vector<BatchData> _batches;

		public:
			

			WebFontRenderer();
			~WebFontRenderer();

			// submit renderable component for rendering (batch preparing, before rendering)
			virtual void submit(const Component* const renderableComponent, const mat4& transformation);

			virtual void render(const Camera& cam);

			virtual void resize(int w, int h) {}

		private:
			std::vector<GlyphData> createGlyphs(std::string characters, std::string fontFilePath);
			WebTexture* createFontTextureAtlas(std::vector<GlyphData>& glyphs);

			void allocateBatches(int maxBatchCount, int maxBatchLength, int entryLength);
			void addToBatch(BatchData& batch, const TextRenderable * const renderable, const mat4& transform);
		};
	}

}
