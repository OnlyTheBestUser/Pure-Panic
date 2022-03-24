#include "RendererAPI.h"
#include "../Plugins/PlayStation4/PS4RendererAPI.h"
#include "../Plugins/OpenGLRendering/OGLRendererAPI.h"""
using namespace NCL;
using namespace Rendering;

RendererAPI* RendererAPI::activeAPI = nullptr;

RendererAPI::RendererAPI(Window& window) : hostWindow(window)	{

}


RendererAPI* NCL::Rendering::RendererAPI::GetInstance()
{
	if (activeAPI == nullptr) {
#ifdef _WIN64
		activeAPI = new OGLRendererAPI(*Window::GetWindow());
#endif
#ifdef _ORBIS
		activeAPI = new PS4::PS4RendererAPI(*Window::GetWindow());
#endif
	}
	return activeAPI;
}

RendererAPI::~RendererAPI()
{
	delete activeAPI;
}
