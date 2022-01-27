#include "GameRenderCommands.h"

GameRenderCommands::GameRenderCommands(GameWorld& world) : gameWorld(world) {

}

void GameRenderCommands::BuildObjectList() {
	activeObjects.clear();

	gameWorld.OperateOnContents(
		[&](GameObject* o) {
			if (o->IsActive()) {
				const RenderObject* g = o->GetRenderObject();
				if (g) {
					activeObjects.emplace_back(g);
				}
			}
		}
	);
}

void GameRenderCommands::SortObjectList() {
	//Who cares!
}