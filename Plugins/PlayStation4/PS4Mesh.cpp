#ifdef _ORBIS
#include "PS4Mesh.h"
#include "../../Common/Vector2.h"
#include "../../Common/Vector3.h"

#include <.\graphics\api_gnm\toolkit\allocators.h>
#include <.\graphics\api_gnm\toolkit\stack_allocator.h>

using namespace NCL::Maths;
using namespace NCL::PS4;

PS4Mesh::PS4Mesh()	{
	indexBuffer		= 0;
	vertexBuffer	= 0;
	attributeCount	= 0;
}

PS4Mesh::PS4Mesh(const std::string& filename) :MeshGeometry(filename){
	indexBuffer = 0;
	vertexBuffer = 0;

	attributeCount = 0;
	indexType = sce::Gnm::IndexSize::kIndexSize32;
	primitiveType = sce::Gnm::PrimitiveType::kPrimitiveTypeTriList;
}

PS4Mesh::~PS4Mesh()	{
	delete[] attributeBuffers;
}

PS4Mesh* PS4Mesh::GenerateQuad() {
	PS4Mesh* mesh = new PS4Mesh();

	mesh->indexType		= sce::Gnm::IndexSize::kIndexSize32;
	mesh->primitiveType = sce::Gnm::PrimitiveType::kPrimitiveTypeTriStrip;

	mesh->SetVertexPositions({ Vector3(-1.0f, -1.0f, 0.0f), Vector3(-1.0f,  1.0f, 0.0f),Vector3(1.0f, -1.0f, 0.0f), Vector3(1.0f,  1.0f, 0.0f) });
	mesh->SetVertexTextureCoords({ Vector2(0.0f, 0.0f) , Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f), Vector2(0.0f, 1.0f) });
	std::vector<Vector3> normals;
	std::vector<Vector4> tangents;
	std::vector<unsigned int> indices;

	for (int i = 0; i < 4; ++i) {
		normals.emplace_back(Vector3(0, 0, 1));
		tangents.emplace_back(Vector4(1, 0, 0, 0));

		indices.emplace_back(i);
	}

	mesh->SetVertexNormals(normals);
	mesh->SetVertexTangents(tangents);
	mesh->SetVertexIndices(indices);

	return mesh;
}

PS4Mesh* PS4Mesh::GenerateSinglePoint() {
	PS4Mesh* mesh = new PS4Mesh();

	mesh->indexType		= sce::Gnm::IndexSize::kIndexSize32;
	mesh->primitiveType = sce::Gnm::PrimitiveType::kPrimitiveTypePointList;

	mesh->SetVertexPositions({ Vector3(0.0f, 0.0f, 0.0f) });
	mesh->SetVertexNormals({ Vector3(0, 0, 1) });
	mesh->SetVertexTangents({ Vector3(1, 0, 0) });
	mesh->SetVertexIndices({ 0 });

	return mesh;
}

PS4Mesh* PS4Mesh::GenerateTriangle() {
	PS4Mesh* mesh = new PS4Mesh();

	mesh->indexType		= sce::Gnm::IndexSize::kIndexSize32;
	mesh->primitiveType = sce::Gnm::PrimitiveType::kPrimitiveTypeTriList;

	mesh->SetVertexPositions({ Vector3(0.0f, 0.5f, 0.0f), Vector3(0.5f, -0.5f, 0.0f), Vector3(-0.5f, -0.5f, 0.0f) });
	mesh->SetVertexTextureCoords({ Vector2(0.5f, 0.0f) , Vector2(1.0f, 1.0f), Vector2(0.0f, 1.0f) });
	mesh->SetVertexNormals({ Vector3(0, 0, 1),Vector3(0, 0, 1), Vector3(0, 0, 1) });
	mesh->SetVertexTangents({ Vector4(1, 0, 0, 0), Vector4(1, 0, 0,0), Vector4(1, 0, 0,0) });
	mesh->SetVertexIndices({ 0, 1, 2 });

	return mesh;
}

void	PS4Mesh::UploadToGPU(Rendering::RendererAPI* renderer) {
	vertexDataSize = GetVertexCount() * sizeof(MeshVertex);
	indexDataSize  = GetIndexCount() * sizeof(int);

	if (GetIndexCount() > 0) {
		indexBuffer = static_cast<int*>			(garlicAllocator->allocate(indexDataSize, Gnm::kAlignmentOfBufferInBytes));
	}
	vertexBuffer = static_cast<MeshVertex*>	(garlicAllocator->allocate(vertexDataSize, Gnm::kAlignmentOfBufferInBytes));

	//switch (GetPrimitiveType())
	//{
	//default:
	//	break;
	//case NCL::GeometryPrimitive::Triangles:
	//	primitiveType = Gnm::kPrimitiveTypeTriList;
	//	break;
	//case NCL::GeometryPrimitive::Lines:
	//	primitiveType = Gnm::kPrimitiveTypeLineList;
	//	break;
	//}

	if (GetIndexCount() > 0) {
		Gnm::registerResource(nullptr, ownerHandle, indexBuffer, indexDataSize, "IndexData", Gnm::kResourceTypeIndexBufferBaseAddress, 0);
	}
	Gnm::registerResource(nullptr, ownerHandle, vertexBuffer, vertexDataSize, "VertexData", Gnm::kResourceTypeIndexBufferBaseAddress, 0);

	for (int i = 0; i < GetVertexCount(); ++i) {
		memcpy(&vertexBuffer[i].position,	  &positions[i], sizeof(float) * 3);
		memcpy(&vertexBuffer[i].textureCoord, &texCoords[i], sizeof(float) * 2);
		memcpy(&vertexBuffer[i].normal,		  &normals[i],   sizeof(float) * 3);
		memcpy(&vertexBuffer[i].tangent,	  &tangents[i],  sizeof(float) * 3);
		//memcpy(&vertexBuffer[i].colour,		  &colours[i], sizeof(float) * 4);
	}

	if (GetIndexCount() > 0) {
		for (int i = 0; i < GetIndexCount(); ++i) { //Our index buffer might not have the same data size as the source indices?
			indexBuffer[i] = indices[i];
		}
	}

	attributeCount		= 4;
	attributeBuffers	= new sce::Gnm::Buffer[4];

	InitAttributeBuffer(attributeBuffers[0], Gnm::kDataFormatR32G32B32Float, &(vertexBuffer[0].position));
	InitAttributeBuffer(attributeBuffers[1], Gnm::kDataFormatR32G32Float	, &(vertexBuffer[0].textureCoord));
	InitAttributeBuffer(attributeBuffers[2], Gnm::kDataFormatR32G32B32Float, &(vertexBuffer[0].normal));
	InitAttributeBuffer(attributeBuffers[3], Gnm::kDataFormatR32G32B32A32Float, &(vertexBuffer[0].tangent));
	//InitAttributeBuffer(attributeBuffers[4], Gnm::kDataFormatR32G32B32A32Float, &(vertexBuffer[0].colour));
}

void	PS4Mesh::InitAttributeBuffer(sce::Gnm::Buffer &buffer, Gnm::DataFormat format, void*offset) {
	buffer.initAsVertexBuffer(offset, format, sizeof(MeshVertex), GetVertexCount());
	buffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO);
}

void PS4Mesh::SubmitDraw(Gnmx::GnmxGfxContext& cmdList, Gnm::ShaderStage stage) {
	cmdList.setVertexBuffers(stage, 0, attributeCount, attributeBuffers);
	cmdList.setPrimitiveType(primitiveType);
	cmdList.setIndexSize(indexType);

	if (GetIndexCount() != 0) {
 		cmdList.drawIndex(GetIndexCount(), indexBuffer);
	}
	else
	{
		cmdList.drawIndexAuto(GetVertexCount());
		cmdList.setIndexSize(indexType);
	}
} 

void PS4Mesh::UpdateGPUBuffers(unsigned int startVertex, unsigned int vertexCount) {
	delete[] attributeBuffers;
	Gnm::unregisterAllResourcesForOwner(ownerHandle);

	UploadToGPU(nullptr);
}
#endif