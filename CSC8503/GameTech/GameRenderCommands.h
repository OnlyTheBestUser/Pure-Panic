#include "../CSC8503Common/GameWorld.h"

namespace NCL {
	class Maths::Vector3;
	class Maths::Vector4;
	namespace CSC8503 {
		class GameRenderCommands
		{
		public:
			GameRenderCommands(GameWorld& world);
			virtual ~GameRenderCommands() {}
		protected:
			GameWorld& gameWorld;

			void BuildObjectList();
			void SortObjectList();

			virtual void RenderShadowMap() = 0;
			virtual void RenderCamera() = 0;
			virtual void RenderSkybox() = 0;
			virtual void LoadSkybox() = 0;

			virtual void RenderScene() = 0;

			vector<const RenderObject*> activeObjects;
		};
	}
}

