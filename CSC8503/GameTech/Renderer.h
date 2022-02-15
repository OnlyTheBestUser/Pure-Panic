#pragma once
#include "../../Common/RendererAPI.h"

#include "../../CSC8503/CSC8503Common/GameWorld.h"
namespace NCL {
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
			void DrawString(const std::string& text, const Vector2& pos, const Vector4& colour = Vector4(0.75f, 0.75f, 0.75f, 1), float size = 20.0f);
			void DrawLine(const Vector3& start, const Vector3& end, const Vector4& colour);
			void DrawDebugData();
			void DrawDebugLines();
			void DrawDebugStrings();
		protected:
			RendererAPI* rendererAPI;
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

		};
	}
}

