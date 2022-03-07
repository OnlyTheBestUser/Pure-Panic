/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include "../../Common/RendererAPI.h"


#ifdef _WIN32
#include "windows.h"
#endif

#ifdef _DEBUG
#define OPENGL_DEBUGGING
#endif


#include <string>
#include <vector>

namespace NCL {
	class MeshGeometry;

	namespace Maths {
		class Matrix4;
	}

	namespace Rendering {
		class ShaderBase;
		class TextureBase;

		class OGLMesh;
		class OGLShader;

		class SimpleFont;
		
		class OGLRendererAPI : public RendererAPI
		{
		public:
			friend class OGLRendererAPI;
			OGLRendererAPI(Window& w);
			~OGLRendererAPI();

			void OnWindowResize(int w, int h)	override;
			bool HasInitialised()				const override {
				return initState;
			}

			virtual bool SetVerticalSync(VerticalSyncState s);

			void BeginFrame()	override;
			void RenderFrame()	override;
			void EndFrame()		override;
			void SwapBuffers()  override;

		protected:			

			void BindTextureToShader(TextureType type,const TextureBase*t, const std::string& uniform, int texUnit) const;
			void BindMesh(MeshGeometry*m);
			void DrawBoundMesh(int subLayer = 0, int numInstances = 1);
#ifdef _WIN32
			void InitWithWin32(Window& w);
			void DestroyWithWin32();
			HDC		deviceContext;		//...Device context?
			HGLRC	renderContext;		//Permanent Rendering Context		
#endif


			void DrawMesh(MeshGeometry* mesh) override;
			void DrawMeshAndSubMesh(MeshGeometry* mesh) override;

			void BindFrameBuffer() override;
			void BindFrameBuffer(const FrameBufferBase* fbo) override;

			void SetDepth(bool depth) override;
			void SetBlend(bool blend) override;

			void SetCullFace(bool cull) override;
			void SetCullType(CULL_TYPE type) override;

			void SetViewportSize(int x, int y) override;

			void ClearBuffer(bool color, bool depth, bool stencil) override;
			void SetClearColour(float r, float g, float b, float a) override;
			void SetColourMask(bool r, bool g, bool b, bool a) override;
		private:
			struct DebugString {
				Maths::Vector4 colour;
				Maths::Vector2	pos;
				float			size;
				std::string		text;
			};

			struct DebugLine {
				Maths::Vector3 start;
				Maths::Vector3 end;
				Maths::Vector4 colour;
			};

			OGLMesh*	boundMesh;
			OGLShader*	boundShader;
		};
	}
}