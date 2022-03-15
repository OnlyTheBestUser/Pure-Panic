#pragma once
#ifdef _ORBIS
#include "../../Common/MeshGeometry.h"
#include "PS4MemoryAware.h"

#include <gnm.h>

#include <gnmx\context.h>
#include <..\samples\sample_code\graphics\api_gnm\toolkit\allocators.h>
#include <..\samples\sample_code\graphics\api_gnm\toolkit\stack_allocator.h>
#include <gnm\dataformats.h>


namespace NCL::PS4 {
	using namespace sce;

	class PS4Mesh :
		public NCL::MeshGeometry, public PS4MemoryAware
	{
		friend class PS4RendererAPI;
	public:		
		PS4Mesh();
		PS4Mesh(const std::string& filename);
		~PS4Mesh();

		static PS4Mesh* GenerateTriangle();
		static PS4Mesh* GenerateQuad();
		static PS4Mesh* GenerateSinglePoint();

		void	UploadToGPU(Rendering::RendererAPI* renderer) override;
		void UpdateGPUBuffers(unsigned int startVertex, unsigned int vertexCount) override;
		void	SubmitDraw(Gnmx::GnmxGfxContext& cmdList, Gnm::ShaderStage stage);

		void	InitAttributeBuffer(sce::Gnm::Buffer &buffer, Gnm::DataFormat format, void*offset);

	protected:

		const int MAX_DEBUG_VERTEX_LIMIT = 5000;
		//Gpu simply has a 4 byte alignment!
		struct MeshVertex
		{
			float position[3];
			float textureCoord[2];
			float normal[3];
			float tangent[4];
			float colour[4];
		};

		sce::Gnm::IndexSize		indexType;

		int*		indexBuffer;
		sce::Gnm::ResourceHandle indexHandle;
		MeshVertex*	vertexBuffer;
		sce::Gnm::ResourceHandle vertexHandle;

		int	vertexDataSize;
		int indexDataSize;

		sce::Gnm::Buffer*	attributeBuffers;
		int					attributeCount;
	};
}
#endif
