/******************************************************************************
Class:RendererAPI
Implements:
Author:Rich Davison
Description:TODO

-_-_-_-_-_-_-_,------,
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""

*//////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Window.h"
#include "../Common/Vector3.h"
#include "../Common/Vector4.h"
#include "../Common/Matrix4.h"
namespace NCL {
	class MeshGeometry;

	namespace Rendering {
		class ShaderBase;
		class TextureBase;

		enum class VerticalSyncState {
			VSync_ON,
			VSync_OFF,
			VSync_ADAPTIVE
		};
		class RendererAPI {
		public:
			friend class NCL::Window;
			RendererAPI(Window& w);
			virtual ~RendererAPI();
			virtual bool HasInitialised() const { return true; }
			virtual void Update(float dt) {}
			virtual bool SetVerticalSync(VerticalSyncState s) {
				return false;
			}

			virtual void BeginFrame() = 0;
			virtual void RenderFrame() = 0;
			virtual void EndFrame() = 0;
			virtual void SwapBuffers() = 0;

			// Renderer commands
			virtual void DrawMesh(MeshGeometry* mesh) = 0;
			virtual void BindShader(ShaderBase* shader) = 0;
			virtual void BindTexture(const TextureBase* tex, std::string uniform, int texSlot) = 0;
			virtual void UpdateUniformFloat(ShaderBase* shader, std::string uniform, float f) = 0;
			virtual void UpdateUniformMatrix4(ShaderBase* shader, std::string uniform, Maths::Matrix4 matrix) = 0;
			virtual void SetDepth(bool depth) = 0;
			virtual void SetBlend(bool blend) = 0;

			int GetCurrentWidth() { return currentWidth; }
			int GetCurrentHeight() { return currentHeight; }

		protected:
			virtual void OnWindowResize(int w, int h) = 0;
			virtual void OnWindowDetach() {}; //Most renderers won't care about this
			
			Window& hostWindow;

			int currentWidth;
			int currentHeight;
		};
	}
}
