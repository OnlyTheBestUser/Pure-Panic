#pragma once
#ifdef _ORBIS
#include "../../Common/RendererAPI.h"
#include "PS4MemoryAware.h"

#include <gnm.h>
#include <gnmx\fetchshaderhelper.h>

#include <..\samples\sample_code\graphics\api_gnm\toolkit\allocators.h>
#include <..\samples\sample_code\graphics\api_gnm\toolkit\stack_allocator.h>
#include <..\samples\sample_code\graphics\api_gnm\toolkit\toolkit.h>
#include "PS4Frame.h"
#include "PS4Texture.h"
#include "../../Common/Window.h"
#include "../../CSC8503/GameTech/Renderer.h"

using namespace sce;
using namespace Gnmx;
using namespace Gnmx::Toolkit;



namespace NCL::CSC8503 {
	class RenderObject;
}

namespace NCL::Maths {
	class Matrix4;
}

namespace NCL::PS4 {
	enum MemoryLocation {
		MEMORY_GARLIC,
		MEMORY_ONION,
		MEMORY_MAX
	};

	struct PS4ScreenBuffer {
		sce::Gnm::RenderTarget		colourTarget;
		sce::Gnm::DepthRenderTarget depthTarget;
	};

	class PS4Window;
	class PS4Shader;
	class PS4Mesh;


	class PS4RendererAPI :
		public RendererAPI, public PS4MemoryAware
	{
	friend class PS4UniformBuffer;
	friend class PS4Shader;
	friend class PS4Texture;
	friend class PS4FrameBuffer;
	friend class NCL::Rendering::Renderer;
	public:
		PS4RendererAPI(Window& window);
		~PS4RendererAPI();

	protected:
		void	OnWindowResize(int w, int h) override;
		void	BeginFrame()    override;
		void	EndFrame()		override;
		void RenderFrame() override;
		void	SwapBuffers()	override;

		void	SwapScreenBuffer();
		void	SwapCommandBuffer();
		void	SetRenderBuffer(PS4ScreenBuffer*buffer, bool clearColour, bool clearDepth, bool clearStencil);
		PS4ScreenBuffer* GenerateScreenBuffer(uint width, uint height, bool colour = true, bool depth = true, bool stencil = false);
		void SetPaintBuffer(Gnm::RenderTarget target);

		//Render commands
		void DrawMesh(MeshGeometry* mesh) override;
		void DrawMeshAndSubMesh(MeshGeometry* mesh) override;

		void BindFrameBuffer() override;
		void BindFrameBuffer(const FrameBufferBase* fbo) override;

		void SetDepth(bool depth) override;
		void SetBlend(bool blend, BlendType srcFunc = BlendType::ONE, BlendType dstFunc = BlendType::ONE) override;

		void SetCullFace(bool cull) override;
		void SetCullType(CULL_TYPE type) override;

		void SetViewportSize(int x, int y) override;

		void ClearBuffer(bool color, bool depth, bool stencil) override;
		void SetClearColour(float r, float g, float b, float a) override;
		void SetColourMask(bool r, bool g, bool b, bool a) override;

	private:
		void	InitialiseMemoryAllocators();
		void	InitialiseVideoSystem();
		void	InitialiseGCMRendering();

		void	DestroyMemoryAllocators();
		void	DestroyVideoSystem();
		void	DestroyGCMRendering();

	protected:
		int currentGPUBuffer;

		const int _MaxCMDBufferCount;

		//VIDEO SYSTEM VARIABLES
		int videoHandle;		//Handle to video system

	//SCREEN BUFFER VARIABLES
		const int			_bufferCount;	//How many screen buffers should we have
		int					currentScreenBuffer;
		int					prevScreenBuffer;
		PS4ScreenBuffer**	screenBuffers;	//Pointer to our screen buffers
	//Memory Allocation
		const int _GarlicMemory;
		const int _OnionMemory;

		sce::Gnmx::Toolkit::StackAllocator	stackAllocators[MEMORY_MAX];

		//Individual Frames
		PS4Frame*	frames;

		int framesSubmitted;

		//Per frame pointers...
		PS4ScreenBuffer*		currentPS4Buffer;  //Pointer to whichever buffer we're currently using...
		Gnmx::GnmxGfxContext*	currentGFXContext;
		PS4Frame*				currentFrame;
	};
}
#endif
