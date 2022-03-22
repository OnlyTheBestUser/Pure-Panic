#pragma once
#include "../../Common/Matrix4.h"
#include "../../Common/TextureBase.h"
#include "../../Common/ShaderBase.h"
#include "../../Common/Vector4.h"

constexpr int TEXTURE_AMOUNT = 2;

namespace NCL {
	using namespace NCL::Rendering;

	class MeshGeometry;
	namespace CSC8503 {
		class Transform;
		using namespace Maths;

		class RenderObject
		{
		public:
			RenderObject(Transform* parentTransform, MeshGeometry* mesh, TextureBase* tex, ShaderBase* shader, TextureBase* paintMask = nullptr, TextureBase* normal = nullptr);
			virtual ~RenderObject();

			void SetDefaultTexture(TextureBase* t) {
				texture[0] = t;
			}

			void SetMaskTexture(TextureBase* t) {
				paintMask = t;
			}

			TextureBase* GetDefaultTexture() const {
				return texture[0];
			}

			TextureBase* GetNormalMap() const {
				return texture[1];
			}

			TextureBase* GetPaintMask() const {
				return paintMask;
			}

			MeshGeometry*	GetMesh() const {
				return mesh;
			}

			Transform*		GetTransform() const {
				return transform;
			}

			ShaderBase*		GetShader() const {
				return shader;
			}

			void SetColour(const Vector4& c) {
				colour = c;
			}

			Vector4 GetColour() const {
				return colour;
			}

			void SetVisibility(bool IsVisible) {
				colour.w = int(IsVisible);
			}

		protected:
			MeshGeometry*	mesh;
			TextureBase*	texture[2];
			TextureBase*	paintMask;
			ShaderBase*		shader;
			Transform*		transform;
			Vector4			colour;
		};
	}
}
