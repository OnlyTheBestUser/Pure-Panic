#pragma once
#include "../../Common/RendererAPI.h"
#include <vector>

namespace NCL {
	namespace Rendering {
		class SimpleFont;

		class RendererBase
		{
		public:
			RendererBase();
			~RendererBase();

			virtual void Update(float dt) = 0;
			virtual void Render() = 0;

			RendererAPI* GetRendererAPI() { return rendererAPI; }

			void ForceValidDebugState(bool newState) {
				forceValidDebugState = newState;
			}

			// Render commands
			void DrawString(const std::string& text, const Maths::Vector2& pos, const Maths::Vector4& colour = Vector4(0.75f, 0.75f, 0.75f, 1), float size = 20.0f);
			void DrawLine(const Maths::Vector3& start, const Maths::Vector3& end, const Maths::Vector4& colour);


			// Debug
			virtual Maths::Matrix4 SetupDebugLineMatrix() const;
			virtual Maths::Matrix4 SetupDebugStringMatrix() const;
			void DrawDebugData();
			void DrawDebugLines();
			void DrawDebugStrings();

		protected:
			RendererAPI* rendererAPI;
			NCL::Rendering::SimpleFont* font;
			MeshGeometry* debugTextMesh;
			MeshGeometry* debugLinesMesh;
			ShaderBase* debugShader;


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

			std::vector<DebugString>	debugStrings;
			std::vector<DebugLine>		debugLines;
			bool forceValidDebugState;

		};
	}
}

