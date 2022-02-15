#include "Renderer.h"
#include "OGLGameRenderer.h"
#include "PS4GameRenderer.h"
using namespace NCL;
using namespace Rendering;

Renderer::Renderer(GameWorld& world) {
#ifdef _WIN64
	rendererAPI = new OGLGameRenderer(world);
#endif
#ifdef _ORBIS
	rendererAPI = new PS4::PS4GameRenderer(world);
#endif
}

Renderer::~Renderer() {
	delete rendererAPI;
}

void Renderer::Update(float dt) {

}

void Renderer::Render() {
	rendererAPI->Render();
}

void Renderer::DrawString(const std::string& text, const Vector2& pos, const Vector4& colour, float size) {
	DebugString s;
	s.colour = colour;
	s.pos = pos;
	s.size = size;
	s.text = text;
	debugStrings.emplace_back(s);
}

void Renderer::DrawLine(const Vector3& start, const Vector3& end, const Vector4& colour) {
	DebugLine l;
	l.start = start;
	l.end = end;
	l.colour = colour;
	debugLines.emplace_back(l);
}

void Renderer::DrawDebugData() {
	/*if (debugStrings.empty() && debugLines.empty()) {
		return; //don't mess with OGL state if there's no point!
	}
	BindShader(debugShader);

	if (forceValidDebugState) {
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	int matLocation = glGetUniformLocation(debugShader->GetProgramID(), "viewProjMatrix");
	Matrix4 pMat;

	BindTextureToShader(font->GetTexture(), "mainTex", 0);

	GLuint texSlot = glGetUniformLocation(boundShader->programID, "useTexture");

	if (debugLines.size() > 0) {
		pMat = SetupDebugLineMatrix();
		glUniformMatrix4fv(matLocation, 1, false, pMat.array);
		glUniform1i(texSlot, 0);
		DrawDebugLines();
	}

	if (debugStrings.size() > 0) {
		pMat = SetupDebugStringMatrix();
		glUniformMatrix4fv(matLocation, 1, false, pMat.array);
		glUniform1i(texSlot, 1);
		DrawDebugStrings();
	}

	if (forceValidDebugState) {
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}*/
}

void Renderer::DrawDebugStrings() {
	/*vector<Vector3> vertPos;
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

	BindMesh(debugTextMesh);
	DrawBoundMesh();

	debugStrings.clear();*/
}

void Renderer::DrawDebugLines() {
	/*vector<Vector3> vertPos;
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

	BindMesh(debugLinesMesh);
	DrawBoundMesh();

	debugLines.clear();*/
}