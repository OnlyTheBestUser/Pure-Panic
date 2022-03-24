/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#include "OGLRendererAPI.h"
#include "OGLShader.h"
#include "OGLMesh.h"
#include "OGLTexture.h"
#include "OGLFrameBuffer.h"

#include "../../Common/SimpleFont.h"
#include "../../Common/TextureLoader.h"

#include "../../Common/Vector2.h"
#include "../../Common/Vector3.h"
#include "../../Common/Matrix4.h"

#include "../../Common/MeshGeometry.h"

#ifdef _WIN32
#include "../../Common/Win32Window.h"

#include "KHR\khrplatform.h"
#include "glad\glad.h"

#include "GL/GL.h"
#include "KHR/WGLext.h"

PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;
#endif

using namespace NCL;
using namespace NCL::Rendering;

#ifdef OPENGL_DEBUGGING
static void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);
#endif;

OGLRendererAPI::OGLRendererAPI(Window& w) : RendererAPI(w)	{
	initState = false;
#ifdef _WIN32
	InitWithWin32(w);
#endif
	boundMesh	= nullptr;

	currentWidth	= (int)w.GetScreenSize().x;
	currentHeight	= (int)w.GetScreenSize().y;
}

OGLRendererAPI::~OGLRendererAPI()	{

#ifdef _WIN32
	DestroyWithWin32();
#endif
}

void OGLRendererAPI::OnWindowResize(int w, int h)	 {
	currentWidth	= w;
	currentHeight	= h;
	glViewport(0, 0, currentWidth, currentHeight);
}

void OGLRendererAPI::BeginFrame()		{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(0);
	//BindShader(nullptr);
	//BindMesh(nullptr);
}

void OGLRendererAPI::RenderFrame()		{

}

void OGLRendererAPI::EndFrame()		{
	
}

void OGLRendererAPI::SwapBuffers()   {
	::SwapBuffers(deviceContext);
}

void OGLRendererAPI::BindMesh(MeshGeometry*m) {
	if (!m) {
		glBindVertexArray(0);
		boundMesh = nullptr;
	}
	else if (OGLMesh* oglMesh = dynamic_cast<OGLMesh*>(m)) {
		if (oglMesh->GetVAO() == 0) {
			std::cout << __FUNCTION__ << " has received invalid mesh?!" << std::endl;
		}
		glBindVertexArray(oglMesh->GetVAO());
		boundMesh = oglMesh;
	}
	else {
		std::cout << __FUNCTION__ << " has received invalid mesh?!" << std::endl;
		boundMesh = nullptr;
	}
}

void OGLRendererAPI::DrawBoundMesh(int subLayer, int numInstances) {
	if (!boundMesh) {
		std::cout << __FUNCTION__ << " has been called without a bound mesh!" << std::endl;
		return;
	}
	GLuint	mode	= 0;
	int		count	= 0;
	int		offset	= 0;

	if (boundMesh->GetSubMeshCount() == 0) {
		if (boundMesh->GetIndexCount() > 0) {
			count = boundMesh->GetIndexCount();
		}
		else{
			count = boundMesh->GetVertexCount();
		}
	}
	else {
		const SubMesh* m = boundMesh->GetSubMesh(subLayer);
		offset = m->start;
		count  = m->count;
	}

	switch (boundMesh->GetPrimitiveType()) {
		case GeometryPrimitive::Triangles:		mode = GL_TRIANGLES;		break;
		case GeometryPrimitive::Points:			mode = GL_POINTS;			break;
		case GeometryPrimitive::Lines:			mode = GL_LINES;			break;
		case GeometryPrimitive::TriangleFan:	mode = GL_TRIANGLE_FAN;		break;
		case GeometryPrimitive::TriangleStrip:	mode = GL_TRIANGLE_STRIP;	break;
		case GeometryPrimitive::Patches:		mode = GL_PATCHES;			break;
	}

	if (boundMesh->GetIndexCount() > 0) {
		glDrawElements(mode, count, GL_UNSIGNED_INT, (const GLvoid*)(offset * sizeof(unsigned int)));
	}
	else {
		glDrawArrays(mode, 0, count);
	}
}


void OGLRendererAPI::DrawMesh(MeshGeometry* mesh) {
	BindMesh(mesh);
	DrawBoundMesh();
}

void OGLRendererAPI::DrawMeshAndSubMesh(MeshGeometry* mesh) {
	BindMesh(mesh);
	int layerCount = mesh->GetSubMeshCount();
	for (int i = 0; i < layerCount; ++i) {
		DrawBoundMesh(i);
	}
}

void OGLRendererAPI::BindFrameBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OGLRendererAPI::BindFrameBuffer(const FrameBufferBase* fbo) {
	const OGLFrameBuffer* oglFbo = dynamic_cast<const OGLFrameBuffer*>(fbo);
	if (!oglFbo) {
		return;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, oglFbo->GetBufferObject());
}

void OGLRendererAPI::SetDepth(bool d) {
	d ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
}

void OGLRendererAPI::SetBlend(bool b, RendererAPI::BlendType srcFunc, BlendType dstFunc) {
	b ? glEnable(GL_BLEND) : glDisable(GL_BLEND);

	auto toGLenum = [](const BlendType func)->GLenum {
		switch (func)
		{
		case BlendType::NONE:
			return GL_NONE;
			break;
		case BlendType::ONE:
			return GL_ONE;
			break;
		case BlendType::ALPHA:
			return GL_SRC_ALPHA;
			break;
		case BlendType::ONE_MINUS_ALPHA:
			return GL_ONE_MINUS_SRC_ALPHA;
			break;
		case BlendType::SRC_COLOR:
			return GL_SRC_COLOR;
			break;
		case BlendType::ONE_MINUS_SRC_COLOR:
			return GL_ONE_MINUS_SRC_COLOR;
			break;
		default:
			break;
		}
	};

	glBlendFunc(toGLenum(srcFunc), toGLenum(dstFunc));
}

void OGLRendererAPI::SetCullFace(bool b) {
	switch (b)
	{
	default:
		break;
	case true:
		glEnable(GL_CULL_FACE);
		break;
	case false:
		glDisable(GL_CULL_FACE);
		break;
	}
}

void OGLRendererAPI::SetCullType(CULL_TYPE type) {
	switch (type)
	{
	case NCL::Rendering::RendererAPI::FRONT:
		glCullFace(GL_FRONT);
		break;
	case NCL::Rendering::RendererAPI::BACK:
		glCullFace(GL_BACK);
		break;
	default:
		break;
	}
}

void OGLRendererAPI::ClearBuffer(bool color, bool depth, bool stencil) {
	if (color) {
		glClear(GL_COLOR_BUFFER_BIT);
	}
	if (depth) {
		glClear(GL_DEPTH_BUFFER_BIT);
	}
	if (stencil) {
		glClear(GL_STENCIL_BUFFER_BIT);
	}
}

void OGLRendererAPI::SetClearColour(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
}

void OGLRendererAPI::SetColourMask(bool r, bool g, bool b, bool a) {
	glColorMask(r, g, b, a);
}

void OGLRendererAPI::SetViewportSize(int x, int y) {
	glViewport(0, 0, x, y);
}


#ifdef _WIN32
void OGLRendererAPI::InitWithWin32(Window& w) {
	Win32Code::Win32Window* realWindow = (Win32Code::Win32Window*)&w;

	if (!(deviceContext = GetDC(realWindow->GetHandle()))) {
		std::cout << __FUNCTION__ << " Failed to create window!" << std::endl;
		return;
	}

	//A pixel format descriptor is a struct that tells the Windows OS what type of front / back buffers we want to create etc
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize		= sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion	= 1;
	pfd.dwFlags		= PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;   //It must be double buffered, it must support OGL(!), and it must allow us to draw to it...
	pfd.iPixelType	= PFD_TYPE_RGBA;	//We want our front / back buffer to have 4 channels!
	pfd.cColorBits	= 32;				//4 channels of 8 bits each!
	pfd.cDepthBits	= 24;				//24 bit depth buffer
	pfd.cStencilBits = 8;				//plus an 8 bit stencil buffer
	pfd.iLayerType	= PFD_MAIN_PLANE;

	GLuint		PixelFormat;
	if (!(PixelFormat = ChoosePixelFormat(deviceContext, &pfd))) {	// Did Windows Find A Matching Pixel Format for our PFD?
		std::cout << __FUNCTION__ << " Failed to choose a pixel format!" << std::endl;
		return;
	}

	if (!SetPixelFormat(deviceContext, PixelFormat, &pfd)) {		// Are We Able To Set The Pixel Format?
		std::cout << __FUNCTION__ << " Failed to set a pixel format!" << std::endl;
		return;
	}

	HGLRC		tempContext;		//We need a temporary OpenGL context to check for OpenGL 3.2 compatibility...stupid!!!
	if (!(tempContext = wglCreateContext(deviceContext))) {	// Are We Able To get the temporary context?
		std::cout << __FUNCTION__ <<"  Cannot create a temporary context!" << std::endl;
		wglDeleteContext(tempContext);
		return;
	}

	if (!wglMakeCurrent(deviceContext, tempContext)) {	// Try To Activate The Rendering Context
		std::cout << __FUNCTION__ << " Cannot set temporary context!" << std::endl;
		wglDeleteContext(tempContext);
		return;
	}
	if (!gladLoadGL()) {
		std::cout << __FUNCTION__ << " Cannot initialise GLAD!" << std::endl;	//It's all gone wrong!
		return;
	}
	//Now we have a temporary context, we can find out if we support OGL 4.x
	char* ver = (char*)glGetString(GL_VERSION); // ver must equal "4.1.0" (or greater!)
	int major = ver[0] - '0';		//casts the 'correct' major version integer from our version string
	int minor = ver[2] - '0';		//casts the 'correct' minor version integer from our version string

	if (major < 3) {					//Graphics hardware does not support OGL 4! Erk...
		std::cout << __FUNCTION__ << " Device does not support OpenGL 4.x!" << std::endl;
		wglDeleteContext(tempContext);
		return;
	}

	if (major == 4 && minor < 1) {	//Graphics hardware does not support ENOUGH of OGL 4! Erk...
		std::cout << __FUNCTION__ << " Device does not support OpenGL 4.1!" << std::endl;
		wglDeleteContext(tempContext);
		return;
	}
	//We do support OGL 4! Let's set it up...

	int attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, major,
		WGL_CONTEXT_MINOR_VERSION_ARB, minor,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
#ifdef OPENGL_DEBUGGING 
		| WGL_CONTEXT_DEBUG_BIT_ARB
#endif		//No deprecated stuff!! DIE DIE DIE glBegin!!!!
		,WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0					//That's enough attributes...
	};

	//Everywhere else in the Renderers, we use function pointers provided by GLEW...but we can't initialise GLEW yet! So we have to use the 'Wiggle' API
	//to get a pointer to the function that will create our OpenGL 3.2 context...
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	renderContext = wglCreateContextAttribsARB(deviceContext, 0, attribs);

	// Check for the context, and try to make it the current rendering context
	if (!renderContext || !wglMakeCurrent(deviceContext, renderContext)) {
		std::cout << __FUNCTION__ <<" Cannot set OpenGL 3 context!" << std::endl;	//It's all gone wrong!
		wglDeleteContext(renderContext);
		wglDeleteContext(tempContext);
		return;
	}

	wglDeleteContext(tempContext);	//We don't need the temporary context any more!

	std::cout << __FUNCTION__ << " Initialised OpenGL " << major << "." << minor << " rendering context" << std::endl;	//It's all gone wrong!

	glEnable(GL_FRAMEBUFFER_SRGB);

#ifdef OPENGL_DEBUGGING
	glDebugMessageCallback(DebugCallback, NULL);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
#endif

	//If we get this far, everything's going well!
	initState = true; 

	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

	w.SetRenderer(this);
}

void OGLRendererAPI::DestroyWithWin32() {
	wglDeleteContext(renderContext);
}

bool OGLRendererAPI::SetVerticalSync(VerticalSyncState s) {
	if (!wglSwapIntervalEXT) {
		return false;
	}
	GLuint state;

	switch (s) {
		case VerticalSyncState::VSync_OFF:		state =  0; break;
		case VerticalSyncState::VSync_ON:		state =  1; break;
		case VerticalSyncState::VSync_ADAPTIVE:	state = -1; break;
	}

	return wglSwapIntervalEXT(state);
}
#endif

#ifdef OPENGL_DEBUGGING
static void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
	string sourceName;
	string typeName;
	string severityName;

	switch (source) {
	case GL_DEBUG_SOURCE_API_ARB: sourceName = "Source(OpenGL)"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB: sourceName = "Source(Window System)"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB: sourceName = "Source(Shader Compiler)"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY_ARB: sourceName = "Source(Third Party)"; break;
	case GL_DEBUG_SOURCE_APPLICATION_ARB: sourceName = "Source(Application)"; break;
	case GL_DEBUG_SOURCE_OTHER_ARB: sourceName = "Source(Other)"; break;
	}

	switch (type) {
	case GL_DEBUG_TYPE_ERROR_ARB: typeName = "Type(Error)"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB: typeName = "Type(Deprecated Behaviour)"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB: typeName = "Type(Undefined Behaviour)"; break;
	case GL_DEBUG_TYPE_PORTABILITY_ARB: typeName = "Type(Portability)"; break;
	case GL_DEBUG_TYPE_PERFORMANCE_ARB: typeName = "Type(Performance)"; break;
	case GL_DEBUG_TYPE_OTHER_ARB: typeName = "Type(Other)"; break;
	}

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH_ARB: severityName = "Priority(High)"; break;
	case GL_DEBUG_SEVERITY_MEDIUM_ARB: severityName = "Priority(Medium)"; break;
	case GL_DEBUG_SEVERITY_LOW_ARB: severityName = "Priority(Low)"; break;
	}

	std::cout << "OpenGL Debug Output: " + sourceName + ", " + typeName + ", " + severityName + ", " + string(message) << std::endl;
}
#endif