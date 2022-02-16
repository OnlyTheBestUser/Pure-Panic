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

			void UpdateShaderMatrices(OGLShader* shader, Matrix4 proj, Matrix4 view);
			void UpdateModelShaderMatrices(OGLShader* shader, Matrix4 model);
			void UpdateModelShaderMatrices(OGLShader* shader, Matrix4 model, Matrix4 shadow);
			void UpdateLightUniforms(OGLShader* shader, Vector3 lightPos, Vector4 lightColor, float lightRadius);


			void BindTextureToShader(const TextureBase*t, const std::string& uniform, int texUnit) const;
			void BindMesh(MeshGeometry*m);
			void DrawBoundMesh(int subLayer = 0, int numInstances = 1);
#ifdef _WIN32
			void InitWithWin32(Window& w);
			void DestroyWithWin32();
			HDC		deviceContext;		//...Device context?
			HGLRC	renderContext;		//Permanent Rendering Context		
#endif


			void DrawMesh(MeshGeometry* mesh) override;
			void BindShader(ShaderBase* shader) override;
			void BindTexture(const TextureBase* tex, std::string uniform, int texSlot) override;
			void UpdateUniformFloat(ShaderBase* shader, std::string uniform, float f) override;
			void UpdateUniformMatrix4(ShaderBase* shader, std::string uniform, Maths::Matrix4 matrix) override;
			void SetDepth(bool depth) override;
			void SetBlend(bool blend) override;
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

			bool initState;
		};
	}
}