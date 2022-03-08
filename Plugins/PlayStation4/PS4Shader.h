#pragma once
#ifdef _ORBIS
#include "../../Common/ShaderBase.h"
#include "PS4MemoryAware.h"

#include <gnm.h>
#include <gnmx\shaderbinary.h>
#include <gnmx\context.h>

#include <..\samples\sample_code\graphics\api_gnm\toolkit\allocators.h>
#include <..\samples\sample_code\graphics\api_gnm\toolkit\stack_allocator.h>
#include <vector>
#include <shader\binary.h>

using std::vector;
using namespace sce;

namespace NCL::PS4 {
	class PS4Shader :
		public Rendering::ShaderBase, public PS4MemoryAware
	{
		friend class PS4RendererAPI;
	public:
		~PS4Shader();

		static PS4Shader* GenerateShader(const string& vertex, const string& pixel);

		int		GetConstantBufferIndex(const Gnm::ShaderStage stage, const string& name);
		void	SubmitShaderSwitch(Gnmx::GnmxGfxContext& cmdList);

		virtual void BindShader() override;
		virtual void UpdateUniformInt(std::string uniform, const int i) override;
		virtual void UpdateUniformFloat(std::string uniform, const float f) override;
		virtual void UpdateUniformVector3(std::string uniform, const Maths::Vector3 vec) override;
		virtual void UpdateUniformVector4(std::string uniform, const Maths::Vector4 vec) override;
		virtual void UpdateUniformMatrix4(std::string uniform, const Maths::Matrix4 matrix) override;

		void UpdateAllUniform(std::string uniform, Gnm::Buffer buffer);

	protected:
		PS4Shader();

		void ReloadShader() override {
			//we load binaries in directly...
		}

		void GenerateVertexShader(const string& name, bool makeFetch);

		void GeneratePixelShader(const string& name);

		void GenerateFetchShader(char* binData);

		bool LoadShaderText(const string& name, string& into);

		bool LoadShaderBinary(const string& name, char*& into, int& dataSize);

		bool ShaderIsBinary(const string& name);

		void* fetchShader;

		sce::Shader::Binary::Program	vertexBinary;
		Gnmx::VsShader* vertexShader;
		Gnmx::InputOffsetsCache			vertexCache;

		sce::Shader::Binary::Program	pixelBinary;
		Gnmx::PsShader* pixelShader;
		Gnmx::InputOffsetsCache			pixelCache;
	};
}
#endif
