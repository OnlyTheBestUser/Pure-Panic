#ifdef _ORBIS
#include "PS4RendererAPI.h"
#include "PS4Window.h"
#include <video_out.h>	//Video System
#include "PS4Shader.h"
#include "PS4Mesh.h"
#include <gnmx\basegfxcontext.h>

#include <.\graphics\api_gnm\toolkit\allocators.h>
#include <.\graphics\api_gnm\toolkit\stack_allocator.h>

#include "../../Common/Matrix4.h"

using namespace sce::Vectormath::Scalar::Aos;

namespace SonyMath = sce::Vectormath::Scalar::Aos;

#include <iostream>
#include "PS4FrameBuffer.h"

using namespace NCL;
using namespace NCL::PS4;
using namespace NCL::Maths;

sce::Gnmx::Toolkit::IAllocator	oAllocator;
sce::Gnmx::Toolkit::IAllocator	gAllocator;

PS4RendererAPI::PS4RendererAPI(Window& window)
	: RendererAPI(window),
	_MaxCMDBufferCount(3),
	  _bufferCount(3),
	  _GarlicMemory(1024 * 1024 * 512),
	  _OnionMemory( 1024 * 1024 * 256)
{
	framesSubmitted		 = 0;
	currentGPUBuffer	 = 0;
	currentScreenBuffer	 = 0;
	prevScreenBuffer	 = 0;

	std::cerr << "Starting Rich Code!" << std::endl;

	currentGFXContext	 = nullptr;

	InitialiseMemoryAllocators();

	InitialiseGCMRendering();
	InitialiseVideoSystem();
	window.SetRenderer(this);
	SwapScreenBuffer();
	SwapCommandBuffer();//always swap at least once...

}

PS4RendererAPI::~PS4RendererAPI()	{
	DestroyGCMRendering();
	DestroyVideoSystem();
	DestroyMemoryAllocators();
}

void	PS4RendererAPI::InitialiseVideoSystem() {
	screenBuffers = new PS4ScreenBuffer*[_bufferCount];

	for (int i = 0; i < _bufferCount; ++i) {
		screenBuffers[i] = GenerateScreenBuffer(1920, 1080);
	}

	//Now we can open up the video port
	videoHandle		= sceVideoOutOpen(0, SCE_VIDEO_OUT_BUS_TYPE_MAIN, 0, NULL);

	SceVideoOutBufferAttribute attribute;
	sceVideoOutSetBufferAttribute(&attribute,
		SCE_VIDEO_OUT_PIXEL_FORMAT_B8_G8_R8_A8_SRGB,
		SCE_VIDEO_OUT_TILING_MODE_TILE,
		SCE_VIDEO_OUT_ASPECT_RATIO_16_9,
		screenBuffers[0]->colourTarget.getWidth(),
		screenBuffers[0]->colourTarget.getHeight(),
		screenBuffers[0]->colourTarget.getPitch()
	);

	void* bufferAddresses[_bufferCount];

	for (int i = 0; i < _bufferCount; ++i) {
		bufferAddresses[i] = screenBuffers[i]->colourTarget.getBaseAddress();
	}

	sceVideoOutRegisterBuffers(videoHandle, 0, bufferAddresses, _bufferCount, &attribute);
}

void	PS4RendererAPI::InitialiseGCMRendering() {
	frames = (PS4Frame*)onionAllocator->allocate(sizeof(PS4Frame) * _MaxCMDBufferCount, alignof(PS4Frame));

	for (int i = 0; i < _MaxCMDBufferCount; ++i) {
		new (&frames[i])PS4Frame();
	}

	sce::Gnmx::Toolkit::Allocators allocators = sce::Gnmx::Toolkit::Allocators(oAllocator, gAllocator, ownerHandle);
	Gnmx::Toolkit::initializeWithAllocators(&allocators);
}

void	PS4RendererAPI::InitialiseMemoryAllocators() {
	stackAllocators[MEMORY_GARLIC].init(SCE_KERNEL_WC_GARLIC, _GarlicMemory);
	stackAllocators[MEMORY_ONION ].init(SCE_KERNEL_WB_ONION , _OnionMemory);

	oAllocator = Gnmx::Toolkit::GetInterface(&stackAllocators[MEMORY_ONION]);
	gAllocator = Gnmx::Toolkit::GetInterface(&stackAllocators[MEMORY_GARLIC]);

	this->garlicAllocator   = &gAllocator;
	this->onionAllocator	= &oAllocator;
	Gnm::registerOwner(&ownerHandle, "PS4RendererAPI");
}

void PS4RendererAPI::DestroyMemoryAllocators() {
	stackAllocators[MEMORY_GARLIC].deinit();
	stackAllocators[MEMORY_ONION ].deinit();
}

PS4ScreenBuffer*	PS4RendererAPI::GenerateScreenBuffer(uint width, uint height, bool colour, bool depth, bool stencil) {
	PS4ScreenBuffer* buffer = new PS4ScreenBuffer();

	if (colour) {	
		Gnm::RenderTargetSpec spec;
		spec.init();
		spec.m_width		= width;
		spec.m_height		= height;
		spec.m_numSamples	= Gnm::kNumSamples1;
		spec.m_numFragments = Gnm::kNumFragments1;
		spec.m_colorFormat	= Gnm::kDataFormatB8G8R8A8UnormSrgb;	

		GpuAddress::computeSurfaceTileMode(Gnm::GpuMode::kGpuModeBase, &spec.m_colorTileModeHint, GpuAddress::kSurfaceTypeColorTargetDisplayable, spec.m_colorFormat, 1);

		int32_t success = buffer->colourTarget.init(&spec);

		if (success != SCE_GNM_OK) {
			//bool a = true;
		}

		const Gnm::SizeAlign colourAlign = buffer->colourTarget.getColorSizeAlign();

		void *colourMemory = stackAllocators[MEMORY_GARLIC].allocate(colourAlign);

		Gnm::registerResource(nullptr, ownerHandle, colourMemory, colourAlign.m_size,
			"Colour", Gnm::kResourceTypeDepthRenderTargetBaseAddress, 0);

		buffer->colourTarget.setAddresses(colourMemory, NULL, NULL);
	}

	if (depth) {
		Gnm::DepthRenderTargetSpec spec;
		spec.init();
		spec.m_width			= width;
		spec.m_height			= height;
		spec.m_numFragments		= Gnm::kNumFragments1;
		spec.m_zFormat			= Gnm::ZFormat::kZFormat32Float;
		spec.m_stencilFormat	= (stencil ? Gnm::kStencil8 : Gnm::kStencilInvalid);

		GpuAddress::computeSurfaceTileMode(Gnm::GpuMode::kGpuModeBase, &spec.m_tileModeHint, GpuAddress::kSurfaceTypeDepthTarget, Gnm::DataFormat::build(spec.m_zFormat), 1);

		void* stencilMemory = 0;

		int32_t success = buffer->depthTarget.init(&spec);

		if (success != SCE_GNM_OK) {
			//bool a = true;
		}

		void *depthMemory = stackAllocators[MEMORY_GARLIC].allocate(buffer->depthTarget.getZSizeAlign());

		Gnm::registerResource(nullptr, ownerHandle, depthMemory, buffer->depthTarget.getZSizeAlign().m_size,
			"Depth", Gnm::kResourceTypeDepthRenderTargetBaseAddress, 0);

		if (stencil) {
			stencilMemory = stackAllocators[MEMORY_GARLIC].allocate(buffer->depthTarget.getStencilSizeAlign());

			Gnm::registerResource(nullptr, ownerHandle, stencilMemory, buffer->depthTarget.getStencilSizeAlign().m_size,
				"Stencil", Gnm::kResourceTypeDepthRenderTargetBaseAddress, 0);
		}

		buffer->depthTarget.setAddresses(depthMemory, stencilMemory);
	}

	return buffer;
}

void	PS4RendererAPI::DestroyGCMRendering() {
	onionAllocator->release(frames);
}

void	PS4RendererAPI::DestroyVideoSystem() {
	for (int i = 0; i < _bufferCount; ++i) {
		delete screenBuffers[i];
	}
	delete[] screenBuffers;

	sceVideoOutClose(videoHandle);
}

void	PS4RendererAPI::OnWindowResize(int w, int h)  {
	currentWidth	= w;
	currentHeight	= h;
}

void	PS4RendererAPI::BeginFrame()   {
	currentFrame->StartFrame();

	currentGFXContext->waitUntilSafeForRendering(videoHandle, currentGPUBuffer);

	SetRenderBuffer(currentPS4Buffer, true, true, true);

	//Primitive Setup State
	Gnm::PrimitiveSetup primitiveSetup;
	primitiveSetup.init();
	primitiveSetup.setCullFace(Gnm::kPrimitiveSetupCullFaceBack);
	primitiveSetup.setFrontFace(Gnm::kPrimitiveSetupFrontFaceCcw);
	//primitiveSetup.setPolygonMode()
	currentGFXContext->setPrimitiveSetup(primitiveSetup);

	////Screen Access State
	Gnm::DepthStencilControl dsc;
	dsc.init();
	dsc.setDepthControl(Gnm::kDepthControlZWriteEnable, Gnm::kCompareFuncLessEqual);
	dsc.setDepthEnable(false);
	currentGFXContext->setDepthStencilControl(dsc);
}

void PS4RendererAPI::EndFrame()			{
	currentFrame->EndFrame();
	framesSubmitted++;
}

void NCL::PS4::PS4RendererAPI::RenderFrame()
{

}

void	PS4RendererAPI::SwapBuffers() {
	SwapScreenBuffer();
	SwapCommandBuffer();
}

void	PS4RendererAPI::SwapScreenBuffer() {
	prevScreenBuffer	= currentScreenBuffer;
	currentScreenBuffer = (currentScreenBuffer + 1) % _bufferCount;
	sceVideoOutSubmitFlip(videoHandle, prevScreenBuffer, SCE_VIDEO_OUT_FLIP_MODE_VSYNC, 0);

	currentPS4Buffer = screenBuffers[currentScreenBuffer];
}

void	PS4RendererAPI::SwapCommandBuffer() {
	if (currentGFXContext) {	
		if (currentGFXContext->submit() != sce::Gnm::kSubmissionSuccess) {
			std::cerr << "Graphics queue submission failed?" << std::endl;
		}
		Gnm::submitDone();
	}

	currentGPUBuffer	= (currentGPUBuffer + 1) % _MaxCMDBufferCount;
	 
	currentFrame		= &frames[currentGPUBuffer]; 
	currentGFXContext	= &currentFrame->GetCommandBuffer();
}
 
void NCL::PS4::PS4RendererAPI::SetRenderBuffer(PS4ScreenBuffer*buffer, bool clearColour, bool clearDepth, bool clearStencil) {
	currentPS4Buffer = buffer;
	currentGFXContext->setRenderTargetMask(0xF);
	currentGFXContext->setRenderTarget(0, &currentPS4Buffer->colourTarget);
	currentGFXContext->setDepthRenderTarget(&currentPS4Buffer->depthTarget);

	currentGFXContext->setupScreenViewport(0, 0, currentPS4Buffer->colourTarget.getWidth(), currentPS4Buffer->colourTarget.getHeight(), 0.5f, 0.5f);
	currentGFXContext->setScreenScissor(0, 0, currentPS4Buffer->colourTarget.getWidth(), currentPS4Buffer->colourTarget.getHeight());
	currentGFXContext->setWindowScissor(0, 0, currentPS4Buffer->colourTarget.getWidth(), currentPS4Buffer->colourTarget.getHeight(), sce::Gnm::WindowOffsetMode::kWindowOffsetDisable);
	currentGFXContext->setGenericScissor(0, 0, currentPS4Buffer->colourTarget.getWidth(), currentPS4Buffer->colourTarget.getHeight(), sce::Gnm::WindowOffsetMode::kWindowOffsetDisable);

	ClearBuffer(clearColour, clearDepth, clearStencil);
}

void NCL::PS4::PS4RendererAPI::SetPaintBuffer(Gnm::RenderTarget target) {
	currentGFXContext->setRenderTargetMask(0xF);
	currentGFXContext->setRenderTarget(0, &target);
	currentGFXContext->setDepthRenderTarget(NULL);

	currentGFXContext->setupScreenViewport(0, 0, target.getWidth(), target.getHeight(), 0.5f, 0.5f);
	currentGFXContext->setScreenScissor(0, 0, target.getWidth(), target.getHeight());
	currentGFXContext->setWindowScissor(0, 0, target.getWidth(), target.getHeight(), sce::Gnm::WindowOffsetMode::kWindowOffsetDisable);
	currentGFXContext->setGenericScissor(0, 0, target.getWidth(), target.getHeight(), sce::Gnm::WindowOffsetMode::kWindowOffsetDisable);
}

void NCL::PS4::PS4RendererAPI::ClearBuffer(bool colour, bool depth, bool stencil) {
	if (colour) {
		//Vector4 defaultClearColour(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, 1.0f);
		SonyMath::Vector4 defaultClearColour(0.1f, 0.1f, 0.1f, 1.0f);
		SurfaceUtil::clearRenderTarget(*currentGFXContext, &currentPS4Buffer->colourTarget, defaultClearColour);
	}

	if (depth) {
		float defaultDepth = 1.0f;
		SurfaceUtil::clearDepthTarget(*currentGFXContext, &currentPS4Buffer->depthTarget, defaultDepth);
	}

	if (stencil && currentPS4Buffer->depthTarget.getStencilReadAddress()) {
		int defaultStencil = 0;
		SurfaceUtil::clearStencilTarget(*currentGFXContext, &currentPS4Buffer->depthTarget, defaultStencil);
	}
}

void NCL::PS4::PS4RendererAPI::SetClearColour(float r, float g, float b, float a)
{
}

void NCL::PS4::PS4RendererAPI::SetColourMask(bool r, bool g, bool b, bool a)
{
}

void NCL::PS4::PS4RendererAPI::DrawMesh(MeshGeometry* mesh) {
	PS4Mesh* ps4Mesh = static_cast<PS4Mesh*>(mesh);
	if (!ps4Mesh) {
		return;
	}
	ps4Mesh->SubmitDraw(*currentGFXContext, Gnm::ShaderStage::kShaderStageVs);
}

void NCL::PS4::PS4RendererAPI::DrawMeshAndSubMesh(MeshGeometry* mesh)
{
	PS4Mesh* ps4Mesh = static_cast<PS4Mesh*>(mesh);
	if (!ps4Mesh) {
		return;
	}
	ps4Mesh->SubmitDraw(*currentGFXContext, Gnm::ShaderStage::kShaderStageVs);
}

void NCL::PS4::PS4RendererAPI::BindFrameBuffer()
{
	SetRenderBuffer(screenBuffers[currentScreenBuffer], true, true, true);
}

void NCL::PS4::PS4RendererAPI::BindFrameBuffer(const FrameBufferBase* fbo)
{
	const ::PS4::PS4FrameBuffer* buffer = static_cast<const NCL::PS4::PS4FrameBuffer*>(fbo);
	//PS4ScreenBuffer* test = new PS4ScreenBuffer();
	//test->colourTarget = buffer->renderTarget;

	//SetRenderBuffer(test, false, false, false);
	SetPaintBuffer(buffer->renderTarget);
}

void NCL::PS4::PS4RendererAPI::SetDepth(bool d) {
	sce::Gnm::DepthStencilControl depth;
	depth.setDepthEnable(d);
	depth.setDepthControl(Gnm::DepthControlZWrite::kDepthControlZWriteEnable, Gnm::CompareFunc::kCompareFuncLess);
	currentGFXContext->setDepthStencilControl(depth);
}

void NCL::PS4::PS4RendererAPI::SetBlend(bool b, BlendType srcFunc, BlendType dstFunc) {

	auto toPSenum = [](const BlendType func)->Gnm::BlendMultiplier {
		switch (func)
		{
		case BlendType::NONE:
			return Gnm::BlendMultiplier::kBlendMultiplierZero;
			break;
		case BlendType::ONE:
			return Gnm::BlendMultiplier::kBlendMultiplierOne;
			break;
		case BlendType::ALPHA:
			return Gnm::BlendMultiplier::kBlendMultiplierSrcAlpha;
			break;
		default:
			break;
		}
	};

	sce::Gnm::BlendControl blend;
	blend.init();
	blend.setBlendEnable(b);
	blend.setColorEquation(toPSenum(srcFunc), Gnm::BlendFunc::kBlendFuncAdd, toPSenum(dstFunc));
	blend.setAlphaEquation(toPSenum(BlendType::ONE), Gnm::BlendFunc::kBlendFuncAdd, toPSenum(BlendType::NONE));
	blend.setSeparateAlphaEnable(true);
	currentGFXContext->setBlendControl(0, blend);
}

void NCL::PS4::PS4RendererAPI::SetCullFace(bool cull)
{
	Gnm::PrimitiveSetup primitiveSetup;
	primitiveSetup.init();
	primitiveSetup.setFrontFace(Gnm::kPrimitiveSetupFrontFaceCcw);
	if (cull) {
		primitiveSetup.setCullFace(Gnm::kPrimitiveSetupCullFaceBack);
	}
	else {
		primitiveSetup.setCullFace(Gnm::kPrimitiveSetupCullFaceNone);
	}
	currentGFXContext->setPrimitiveSetup(primitiveSetup);
}

void NCL::PS4::PS4RendererAPI::SetCullType(CULL_TYPE type)
{

}

void NCL::PS4::PS4RendererAPI::SetViewportSize(int x, int y)
{

}
#endif
