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
		};

		/*
		struct FontBatchData
		{
			WebVertexBuffer* vertexBuffer = nullptr;
			WebVertexBuffer* vertexBuffer_uvs = nullptr;
			WebIndexBuffer* indexBuffer = nullptr;

			// length of a single instance's data (not to be confused with "instanceCount")
			int instanceDataLen = -1; // ..kind of like "data slot" for each vertex of each instance. For example: one vec2 for each of the quad's 4 vertices -> instanceDataLength=8
			int maxTotalBatchDataLen = 0;

			int ID = -1;
			bool isFree = true;
			int currentDataPtr = 0;

			int instanceCount = 0;

			// *Ownerships gets always transferred to this GUIBatchData
			FontBatchData(int dataEntryLen, int totalDataLen, WebVertexBuffer* vb, WebVertexBuffer* vb_uvs, WebIndexBuffer* ib) :
				instanceDataLen(dataEntryLen), maxTotalBatchDataLen(totalDataLen), vertexBuffer(vb), vertexBuffer_uvs(vb_uvs), indexBuffer(ib)
			{}
			// *Ownerships gets always transferred to this GUIBatchData
			FontBatchData(const FontBatchData& other) :
				instanceDataLen(other.instanceDataLen),
				maxTotalBatchDataLen(other.maxTotalBatchDataLen),
				vertexBuffer(other.vertexBuffer),
				vertexBuffer_uvs(other.vertexBuffer_uvs),
				indexBuffer(other.indexBuffer)
			{}

			void clear()
			{
				ID = -1;
				isFree = true;
				currentDataPtr = 0;
				instanceCount = 0;
			}

			inline bool isFull() const { return currentDataPtr >= maxTotalBatchDataLen; }
		};
		*/
		
		class WebFontRenderer : public Renderer
		{
		private:

			WebShader _shader;

			WebVertexBuffer* _vb_positions = nullptr;
			WebIndexBuffer* _ib = nullptr;

			PK_int _vertexAttribLocation_pos = -1;
			PK_int _vertexAttribLocation_uv = -1;

			PK_int _uniformLocation_projMat = -1;
			PK_int _uniformLocation_texAtlasRows = -1;
			PK_int _uniformLocation_texSampler = -1;

			int _fontAtlasPixelSize = 16;

			unsigned int _fontAtlasRowCount = 1;
			unsigned int _fontAtlasMaxCellHeight = 0;

			std::unordered_map<unsigned char, CharacterData> _characterMapping;

			WebTexture* _fontTexAtlas;

			std::vector<BatchData> _batches;

		public:
			

			WebFontRenderer();
			~WebFontRenderer();

			// submit renderable component for rendering (batch preparing, before rendering)
			virtual void submit(const Component* const renderableComponent);

			virtual void render(mat4& projectionMatrix, mat4& viewMatrix);

			virtual void resize(int w, int h) {}

		private:
			std::vector<GlyphData> createGlyphs(std::string characters, std::string fontFilePath);
			WebTexture* createFontTextureAtlas(std::vector<GlyphData>& glyphs);


			void allocateBatches(int maxBatchCount, int maxBatchLength, int entryLength);
			//void occupyBatch(BatchData& batch, int batchId);
			void addToBatch(BatchData& batch, const TextRenderable * const renderable);
		};
	}

}