#pragma once
#include <string>
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"

using std::string;

namespace NCL {
	namespace Rendering {
		enum class ShaderStages {
			SHADER_VERTEX,
			SHADER_FRAGMENT,
			SHADER_GEOMETRY,
			SHADER_DOMAIN,
			SHADER_HULL,
			SHADER_MAX
		};

		class ShaderBase	{
		public:
			ShaderBase() {
			}
			ShaderBase(const string& vertex, const string& fragment, const string& geometry = "", const string& domain = "", const string& hull = "");
			virtual ~ShaderBase();

			virtual void BindShader() = 0;
			virtual void UpdateUniformInt(std::string uniform, const int i) = 0;
			virtual void UpdateUniformFloat(std::string uniform, const float f) = 0;
			virtual void UpdateUniformVector3(std::string uniform, const Maths::Vector3 vec) = 0;
			virtual void UpdateUniformVector4(std::string uniform, const Maths::Vector4 vec) = 0;
			virtual void UpdateUniformMatrix4(std::string uniform, const Maths::Matrix4 matrix) = 0;

			virtual void ReloadShader() = 0;
		protected:

			string shaderFiles[(int)ShaderStages::SHADER_MAX];
		};
	}
}

