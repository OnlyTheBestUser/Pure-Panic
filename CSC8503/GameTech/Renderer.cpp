#include "PS4GameRenderer.h"
#include "Renderer.h"
#include "OGLGameRenderer.h"

#include "../../Common/SimpleFont.h"
#include "../../Common/TextureLoader.h"

#include "../../Common/MeshGeometry.h"
#include "../../Plugins/PlayStation4/PS4Mesh.h"
using namespace NCL;
using namespace Rendering;

Renderer::Renderer(GameWorld& world) {
	gameWorld = world;
#ifdef _WIN64
	rendererAPI = new OGLGameRenderer(world);
	debugLinesMesh = new OGLMesh();
	debugTextMesh = new OGLMesh();

	debugLinesMesh->SetVertexPositions(std::vector<Vector3>(5000, Vector3()));
	debugLinesMesh->SetVertexColours(std::vector<Vector4>(5000, Vector3()));

	debugTextMesh->SetVertexPositions(std::vector<Vector3>(5000, Vector3()));
	debugTextMesh->SetVertexColours(std::vector<Vector4>(5000, Vector3()));
	debugTextMesh->SetVertexTextureCoords(std::vector<Vector2>(5000, Vector3()));

	debugTextMesh->UploadToGPU();
	debugLinesMesh->UploadToGPU();

	debugLinesMesh->SetPrimitiveType(GeometryPrimitive::Lines);
	debugShader = new OGLShader("debugVert.glsl", "debugFrag.glsl");

	TextureLoader::RegisterAPILoadFunction(OGLTexture::RGBATextureFromFilename);
#endif
#ifdef _ORBIS
	rendererAPI = new PS4::PS4GameRenderer(world);

	debugLinesMesh = PS4::PS4Mesh::GenerateQuad();
	debugTextMesh = PS4::PS4Mesh::GenerateQuad();

	debugTextMesh->UploadToGPU();
	debugLinesMesh->UploadToGPU();

	debugLinesMesh->SetPrimitiveType(GeometryPrimitive::Lines);
#endif

	font = new SimpleFont("PressStart2P.fnt", "PressStart2P.png");
}

Renderer::~Renderer() {
	delete rendererAPI;
	delete font;
}

void Renderer::Update(float dt) {

}

void Renderer::Render() {
	rendererAPI->BeginFrame();
	rendererAPI->RenderFrame();
	rendererAPI->EndFrame();
	DrawDebugData();
	rendererAPI->SwapBuffers();
}

void Renderer::DrawString(const std::string& text, const Maths::Vector2& pos, const Maths::Vector4& colour, float size) {
	DebugString s;
	s.colour = colour;
	s.pos = pos;
	s.size = size;
	s.text = text;
	debugStrings.emplace_back(s);
}

void Renderer::DrawLine(const Maths::Vector3& start, const Maths::Vector3& end, const Maths::Vector4& colour) {
	DebugLine l;
	l.start = start;
	l.end = end;
	l.colour = colour;
	debugLines.emplace_back(l);
}

void Renderer::DrawDebugData() {
	if (debugStrings.empty() && debugLines.empty()) {
		return; //don't mess with OGL state if there's no point!
	}
	//BindShader(debugShader);

	/*if (forceValidDebugState) {
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}*/

	//int matLocation = glGetUniformLocation(debugShader->GetProgramID(), "viewProjMatrix");
	Matrix4 pMat;

	//BindTextureToShader(font->GetTexture(), "mainTex", 0);

	//GLuint texSlot = glGetUniformLocation(boundShader->programID, "useTexture");

	if (debugLines.size() > 0) {
		//pMat = SetupDebugLineMatrix();
		//glUniformMatrix4fv(matLocation, 1, false, pMat.array);
		//glUniform1i(texSlot, 0);
		DrawDebugLines();
	}

	if (debugStrings.size() > 0) {
		//pMat = SetupDebugStringMatrix();
		//glUniformMatrix4fv(matLocation, 1, false, pMat.array);
		//glUniform1i(texSlot, 1);
		DrawDebugStrings();
	}

	/*if (forceValidDebugState) {
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}*/
}

void Renderer::DrawDebugStrings() {
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


	//BindMesh(debugTextMesh);
	//DrawBoundMesh();

	debugStrings.clear();
}

void Renderer::DrawDebugLines() {
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

	//BindMesh(debugLinesMesh);
	//DrawBoundMesh();

	debugLines.clear();
}


Maths::Matrix4 Renderer::SetupDebugLineMatrix()	const {
	float screenAspect = (float)rendererAPI->GetCurrentWidth() / (float)rendererAPI->GetCurrentHeight();
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

	return projMatrix * viewMatrix;
}

Maths::Matrix4 Renderer::SetupDebugStringMatrix()	const {
	return Matrix4::Orthographic(-1, 1.0f, 100, 0, 0, 100);
}