#pragma once
#include "../../Plugins/OpenGLRendering/OGLRenderer.h"
#include <vector>
#include <string>

namespace NCL {
	class Debug
	{
	public:
		static void Print(const std::string& text, const Maths::Vector2&pos, const Maths::Vector4& colour = Vector4(1, 1, 1, 1));
		static void DrawLine(const Maths::Vector3& startpoint, const Maths::Vector3& endpoint, const Maths::Vector4& colour = Vector4(1, 1, 1, 1), float time = 0.0f);

		static void DrawAxisLines(const Maths::Matrix4 &modelMatrix, float scaleBoost = 1.0f, float time = 0.0f);

		static void SetRenderer(RendererBase* r) {
			renderer = r;
		}

		static void FlushRenderables(float dt);


		static const Maths::Vector4 RED;
		static const Maths::Vector4 GREEN;
		static const Maths::Vector4 BLUE;

		static const Maths::Vector4 BLACK;
		static const Maths::Vector4 WHITE;

		static const Maths::Vector4 YELLOW;
		static const Maths::Vector4 MAGENTA;
		static const Maths::Vector4 CYAN;

	protected:
		struct DebugStringEntry {
			std::string	data;
			Maths::Vector2 position;
			Maths::Vector4 colour;
		};

		struct DebugLineEntry {
			Maths::Vector3 start;
			Maths::Vector3 end;
			float	time;
			Maths::Vector4 colour;
		};

		Debug() {}
		~Debug() {}

		static std::vector<DebugStringEntry>	stringEntries;
		static std::vector<DebugLineEntry>	lineEntries;

		static RendererBase* renderer;
	};
}

