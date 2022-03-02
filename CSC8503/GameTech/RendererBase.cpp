#include "../../Plugins/PlayStation4/PS4RendererAPI.h"
#include "RendererBase.h"
#include "../../Plugins/OpenGLRendering/OGLRendererAPI.h"
#include "../../Plugins/OpenGLRendering/OGLFrameBuffer.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"

#include "../../Common/SimpleFont.h"
#include "../../Common/TextureLoader.h"

#include "../../Common/MeshGeometry.h"
#include "../../Plugins/PlayStation4/PS4Mesh.h"
using namespace NCL;
using namespace Rendering;

RendererBase::RendererBase() {
#ifdef _WIN64
	rendererAPI = new OGLRendererAPI(*Window::GetWindow());

	TextureLoader::RegisterAPILoadFunction(OGLTexture::RGBATextureFromFilename);

	font = new SimpleFont("PressStart2P.fnt", "PressStart2P.png");

	if (rendererAPI->HasInitialised()) {
		OGLTexture* t = (OGLTexture*)font->GetTexture();

		if (t) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, t->GetObjectID());
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glBindTexture(GL_TEXTURE_2D, 0);
			glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		}
		debugShader = new OGLShader("debugVert.glsl", "debugFrag.glsl");
	}

	debugLinesMesh = new OGLMesh();
	debugTextMesh = new OGLMesh();

	debugLinesMesh->SetVertexPositions(std::vector<Vector3>(5000, Vector3()));
	debugLinesMesh->SetVertexColours(std::vector<Vector4>(5000, Vector3()));

	debugTextMesh->SetVertexPositions(std::vector<Vector3>(5000, Vector3()));
	debugTextMesh->SetVertexColours(std::vector<Vector4>(5000, Vector3()));
	debugTextMesh->SetVertexTextureCoords(std::vector<Vector2>(5000, Vector3()));


#endif
#ifdef _ORBIS
	rendererAPI = new PS4::PS4RendererAPI(*Window::GetWindow());

	TextureLoader::RegisterAPILoadFunction(PS4::PS4Texture::LoadTextureFromFile);

	debugLinesMesh = PS4::PS4Mesh::GenerateQuad();
	debugTextMesh = PS4::PS4Mesh::GenerateQuad();


#endif

	debugTextMesh->UploadToGPU(rendererAPI);
	debugLinesMesh->UploadToGPU(rendererAPI);

	debugLinesMesh->SetPrimitiveType(GeometryPrimitive::Lines);
}

RendererBase::~RendererBase() {
	delete font;
	delete rendererAPI;

	delete debugLinesMesh;
	delete debugTextMesh;

	delete debugShader;
}


void RendererBase::DrawString(const std::string& text, const Maths::Vector2& pos, const Maths::Vector4& colour, float size) {
	DebugString s;
	s.colour = colour;
	s.pos = pos;
	s.size = size;
	s.text = text;
	debugStrings.emplace_back(s);
}

void RendererBase::DrawLine(const Maths::Vector3& start, const Maths::Vector3& end, const Maths::Vector4& colour) {
	DebugLine l;
	l.start = start;
	l.end = end;
	l.colour = colour;
	debugLines.emplace_back(l);
}

void RendererBase::DrawDebugData() {
	if (debugStrings.empty() && debugLines.empty()) {
		return; //don't mess with OGL state if there's no point!
	}
	rendererAPI->BindShader(debugShader);

	if (forceValidDebugState) {
		rendererAPI->SetBlend(true);
		rendererAPI->SetDepth(false);
	}

	Matrix4 pMat;

	rendererAPI->BindTexture(font->GetTexture(), "mainTex", 0);

	if (debugLines.size() > 0) {
		pMat = SetupDebugLineMatrix();
		rendererAPI->UpdateUniformMatrix4(debugShader, "viewProjMatrix", pMat);
		rendererAPI->UpdateUniformInt(debugShader, "useTexture", 0);
		DrawDebugLines();
	}

	if (debugStrings.size() > 0) {
		pMat = SetupDebugStringMatrix();
		rendererAPI->UpdateUniformMatrix4(debugShader, "viewProjMatrix", pMat);
		rendererAPI->UpdateUniformInt(debugShader, "useTexture", 1);
		DrawDebugStrings();
	}

	if (forceValidDebugState) {
		rendererAPI->SetBlend(false);
		rendererAPI->SetDepth(true);
	}
}

void RendererBase::DrawDebugStrings() {
	vector<Vector3> vertPos;
	vector<Vector2> vertTex;
	vector<Vector4> vertColours;

	if (debugStrings.size() > 100) {
		bool a = true;
	}

	for (DebugString& s : debugStrings) {
		font->BuildVerticesForString(s.text, s.pos, s.colour, s.size, vertPos, vertTex, vertColours);
	}

	debugTextMesh->SetVertexPositions(vertPos);
	debugTextMesh->SetVertexTextureCoords(vertTex);
	debugTextMesh->SetVertexColours(vertColours);
	debugTextMesh->UpdateGPUBuffers(0, vertPos.size());


	rendererAPI->DrawMesh(debugTextMesh);

	debugStrings.clear();
}

void RendererBase::DrawDebugLines() {
	vector<Vector3> vertPos;
	vector<Vector4> vertCol;

	for (DebugLine& s : debugLines) {
		vertPos.emplace_back(s.start);
		vertPos.emplace_back(s.end);

		vertCol.emplace_back(s.colour);
		vertCol.emplace_back(s.colour);
	}

	debugLinesMesh->SetVertexPositions(vertPos);
	debugLinesMesh->SetVertexColours(vertCol);
	debugLinesMesh->UpdateGPUBuffers(0, vertPos.size());

	rendererAPI->DrawMesh(debugLinesMesh);

	debugLines.clear();
}

Maths::Matrix4 RendererBase::SetupDebugLineMatrix()	const {
	return Matrix4();
}

Maths::Matrix4 RendererBase::SetupDebugStringMatrix()	const {
	return Matrix4();
}

