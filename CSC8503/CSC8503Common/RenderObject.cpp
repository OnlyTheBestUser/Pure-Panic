#include "RenderObject.h"
#include "../../Common/MeshGeometry.h"

using namespace NCL::CSC8503;
using namespace NCL;

RenderObject::RenderObject(Transform* parentTransform, MeshGeometry* mesh, TextureBase* tex, ShaderBase* shader, TextureBase* paintMask, TextureBase* normal)
	: transform(parentTransform), mesh(mesh), shader(shader), paintMask(paintMask), colour({1.0f,1.0f,1.0f,1.0f}) {
	this->texture[0]	= tex;
	this->texture[1] = normal;
}

RenderObject::~RenderObject() {

}