#pragma once
#include "../../Common/RendererAPI.h"

#include "../../CSC8503/CSC8503Common/GameWorld.h"
namespace NCL {
	class SimpleFont;
	namespace Rendering {
		class Renderer
		{
		public:
			Renderer(GameWorld& world);
			~Renderer();

			void Update(float dt);
			void Render();

			RendererAPI* GetRendererAPI() { return rendererAPI;}

			// Render commands
			void DrawString(const std::string& text, const Maths::Vector2& pos, const Maths::Vector4& colour = Vector4(0.75f, 0.75f, 0.75f, 1), float size = 20.0f);
			void DrawLine(const Maths::Vector3& start, const Maths::Vector3& end, const Maths::Vector4& colour);


			// Debug
			Maths::Matrix4 SetupDebugLineMatrix() const;
			Maths::Matrix4 SetupDebugStringMatrix() const;
			void DrawDebugData();
			void DrawDebugLines();
			void DrawDebugStrings();
		protected:
			RendererAPI* rendererAPI;
			GameWorld gameWorld;

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

			SimpleFont* font;
			std::vector<DebugString>	debugStrings;
			std::vector<DebugLine>		debugLines;

			ShaderBase* debugShader;

			MeshGeometry* debugTextMesh;
			MeshGeometry* debugLinesMesh;

		};
	}
}

