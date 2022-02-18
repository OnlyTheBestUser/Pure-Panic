#include "PS4GameRenderer.h"
#include "RendererBase.h"
#include "../../Plugins/OpenGLRendering/OGLRendererAPI.h"

#include "../../Common/SimpleFont.h"
#include "../../Common/TextureLoader.h"

#include "../../Common/MeshGeometry.h"
#include "../../Plugins/PlayStation4/PS4Mesh.h"
using namespace NCL;
using namespace Rendering;

RendererBase::RendererBase() {
	
}

RendererBase::~RendererBase() {

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
		rendererAPI->UpdateUniformFloat(debugShader, "useTexture", 0);
		DrawDebugLines();
	}

	if (debugStrings.size() > 0) {
		pMat = SetupDebugStringMatrix();
		rendererAPI->UpdateUniformMatrix4(debugShader, "viewProjMatrix", pMat);
		rendererAPI->UpdateUniformFloat(debugShader, "useTexture", 1);
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

	rendererAPI->DrawMesh(debugTextMesh);

	debugLines.clear();
}

Maths::Matrix4 RendererBase::SetupDebugLineMatrix()	const {
	return Matrix4();
}

Maths::Matrix4 RendererBase::SetupDebugStringMatrix()	const {
	return Matrix4();
}

