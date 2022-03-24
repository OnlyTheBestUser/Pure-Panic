#include "TutorialGame.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Common/TextureLoader.h"
#include "../../Common/MeshGeometry.h"

#include "../../Common/Quaternion.h"
#include "../CSC8503Common/CollisionDetection.h"
#include "../CSC8503Common/Timer.h"

#include "../CSC8503Common/InputHandler.h"
#include "../CSC8503Common/GameActor.h"
#include "../CSC8503Common/Command.h"
#include "../../Common/Assets.h"

#include "../CSC8503Common/InputList.h"
#include "LoadingScreen.h"

using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame()	{
	world			= new GameWorld();
	LoadingScreen::AddProgress(15.0f);
	LoadingScreen::UpdateGame(0.0f);
	renderer		= new Renderer(*world);
	LoadingScreen::AddProgress(10.0f);
	LoadingScreen::UpdateGame(0.0f);
	physics			= new PhysicsSystem(*world);
	LoadingScreen::AddProgress(25.0f);
	LoadingScreen::UpdateGame(0.0f);
	levelLoader		= new LevelLoader(physics, renderer);
	LoadingScreen::AddProgress(50.0f);
	LoadingScreen::UpdateGame(0.0f);
	gameManager		= new GameManager(this);
	
#ifndef _ORBIS
	InitSounds();
#endif

	forceMagnitude = 30.0f;
	useGravity = true;
	physics->UseGravity(useGravity);

	inSelectionMode = false;

	state = GameState::PLAY;

	Debug::SetRenderer(renderer);

	//physics->SetGravity(Vector3(0, 9.8f, 0));
	//physics->SetLinearDamping(10.0f);

#pragma region Commands

	/*
		Command Design Pattern Explanation

		The command design pattern I've implemented comes down to three classes, all of which can be found in the Input Handling filter:
		Command.h:
			This contains all the commands you will use to affect the world (via input handling). There are a few examples
			in the class itself of how to use them.

		GameActor.h:
			This is a gameobject child that you will control using inputs, I've given the class a set of default methods, which
			can be overriden if you derive a Player class from GameActor for example. Use the commands class above to attach methods
			to commands, which can in turn be assigned to keys in the input handler.

		InputHandler.h:
			This is where keys are assigned, each key you want to assign is made as a Command* pointer variable. The handleInputs method
			is called every frame, it will loop through each of the keys, see if they are assigned to a command, if so it checks if the key is
			pressed and executes accordingly.

		Basically, all this means is that the hard coding of key checking is done in a separate file, and commands are kept in their own file,
		so everything is organised and neat.

		To use this, as shown in the example below, you need to instantiate an input handler and some commands, and bind the commands to the buttons.
		It makes it very easy and readable to change which keys do what. If there is a specific gameobject you wish to register inputs for, you need
		to instantiate a GameActor and pass it into the Commands accordingly.

	*/

	// Character movement Go to Line 395

	inputHandler = new InputHandler();

	Command* toggleDebug = new ToggleBoolCommand(&debugDraw);
	Command* togglePause = new ToggleBoolCommand(&pausePressed);
	Command* toggleMouse = new ToggleMouseCommand(&inSelectionMode);
	Command* resetWorld = new ResetWorldCommand(&state);
	Command* quitCommand = new QuitCommand(&quit, &pause);
	//Command* paintFireCommand = new PaintFireCommand(this);
	Command* startTimer = new StartTimerCommand(gameManager->GetTimer());
	
	inputHandler->BindButton(TOGGLE_DEBUG, toggleDebug);
	inputHandler->BindButton(TOGGLE_PAUSE, togglePause);
	inputHandler->BindButton(RESET_WORLD, resetWorld);
	inputHandler->BindButton(QUIT, quitCommand);
	//inputHandler->BindButton(FIRE, paintFireCommand);
	inputHandler->BindButton(TOGGLE_MOUSE, toggleMouse);
	inputHandler->BindButton(START_TIMER, startTimer);

#pragma endregion
}

void TutorialGame::InitialiseAssets() {
	InitCamera();
	InitWorld();
}


void TutorialGame::InitSounds() {
#ifndef _ORBIS
	audio = NCL::AudioManager::GetInstance();
	audio->Initialize();
	//Menu Sounds
	audio->LoadSound(Assets::AUDIODIR + "menu_music.ogg", false, true, true);
	audio->LoadSound(Assets::AUDIODIR + "menu_move.ogg", false, false, false);
	audio->LoadSound(Assets::AUDIODIR + "menu_select.ogg", false, false, false);

	//Shooting Sounds
	audio->LoadSound(Assets::AUDIODIR + "gun_fire.ogg", true, false, false);
	audio->LoadSound(Assets::AUDIODIR + "splat_neutral_01.ogg", true, false, false);
	audio->LoadSound(Assets::AUDIODIR + "splat_neutral_02.ogg", true, false, false);

	//Player Sounds
	audio->LoadSound(Assets::AUDIODIR + "boy_whoa_01.ogg", true, false, false);
	audio->LoadSound(Assets::AUDIODIR + "boy_whoa_02.ogg", true, false, false);
	audio->LoadSound(Assets::AUDIODIR + "boy_whoa_03.ogg", true, false, false);

	bgm = new BGMManager(audio);
	bgm->PlaySongFade(Assets::AUDIODIR + "menu_music.ogg", 3.0f);
#endif // !_ORBIS
}

TutorialGame::~TutorialGame() {
	delete physics;
	delete renderer;
	delete world;
	delete levelLoader;
}

void TutorialGame::UpdateGame(float dt) {
	Debug::SetRenderer(renderer);

	if (pausePressed) {
		if (pause == false) {
			pause = true;
			SetState(GameState::PAUSE);
		}
		else {
			pause = false;
			SetState(GameState::PLAY);
			
		}
		pausePressed = false;
	}

	switch (state) {
		case GameState::PLAY: {
			UpdateGameWorld(dt);
			break;
		}
		case GameState::PAUSE: {
			UpdatePauseState(dt);
			break;
		}
		case GameState::WIN: {
			UpdateWinScreen(dt);
			break;
		}
		case GameState::RESET: {
			InitCamera();
			InitWorld();
			renderer->ClearPaint();
			selectionObject = nullptr;
			quit = false;
			pause = false;
			SetState(GameState::PLAY);

			break;
		}
	}

	inputHandler->HandleInput();

	Debug::FlushRenderables(dt);

	renderer->scores = gameManager->CalcCurrentScoreRatio();
	renderer->drawGUI = (!LoadingScreen::GetCompletionState() && state == GameState::PLAY);

	renderer->Render();
}

void TutorialGame::UpdatePauseState(float dt) {
	UpdatePauseScreen(dt);
}

void TutorialGame::UpdateGameWorld(float dt)
{
#ifndef _ORBIS
	audio->Update();
	audio->UpdateAudioListener(0, player1->GetTransform().GetPosition(), player1->GetTransform().GetOrientation());
#endif // !_ORBIS

	if (!inSelectionMode) {
		world->GetMainCamera()->UpdateCamera(dt);
	}


	if (debugDraw) {
		GameObjectIterator first;
		GameObjectIterator last;
		world->GetObjectIterators(first, last);
		for (auto i = first; i != last; i++) {
			DebugDrawCollider((*i)->GetBoundingVolume(), &(*i)->GetTransform());
			if ((*i)->GetPhysicsObject() == nullptr)
				continue;
			DebugDrawVelocity((*i)->GetPhysicsObject()->GetLinearVelocity(), &(*i)->GetTransform());
		}
	}


	physics->Update(dt);
	world->UpdateWorld(dt);
	gameManager->Update(dt);

	UpdateScores(dt);
}

void TutorialGame::UpdateScores(float dt) {
	timeSinceLastScoreUpdate += dt;
	//Can change time for better performance
	if (timeSinceLastScoreUpdate > 1.0f/60.0f) {
		GameObjectIterator start;
		GameObjectIterator cur;
		GameObjectIterator end;
		world->GetPaintableObjectIterators(start, end);
		cur = start;
		for (int i = 0; i < currentObj; i++) {
			cur++;
			if (cur == end) {
				currentObj = 0;
				cur = start;
			}
		}

		currentObj++;
		if ((*cur)->GetPaintRadius() == 0 || (*cur)->GetRenderObject() == nullptr) {
			return;
		}
		// Need to score the texture here.
		Vector2 scoreDif = renderer->CountPaintMask((*cur)->GetRenderObject()->GetPaintMask(), world->GetScoreForObject((*cur)), GameManager::team1Colour, GameManager::team2Colour);
		if ((*cur)->GetPaintRadius() != 0){
			scoreDif = scoreDif / (*cur)->GetPaintRadius();
		}
		world->UpdateScore((*cur), scoreDif);
		//std::cout << (*cur)->GetName() << "\n" << "Team 1: " << scoreDif.x << "\n" << "Team 2: " << scoreDif.y << "\n\n";

		gameManager->UpdateScores(scoreDif);
		timeSinceLastScoreUpdate = 0;
	}
}

void TutorialGame::DebugDrawCollider(const CollisionVolume* c, Transform* worldTransform) {
	Vector4 col = Vector4(1, 0, 0, 1);

	if (c == nullptr)
		return;
	switch (c->type) {
	case VolumeType::AABB: Debug::DrawCube(worldTransform->GetPosition(), ((AABBVolume*)c)->GetHalfDimensions(), col); break;
	case VolumeType::OBB: Debug::DrawCube(worldTransform->GetPosition(), ((AABBVolume*)c)->GetHalfDimensions(), Vector4(0, 1, 0, 1), 0, worldTransform->GetOrientation()); break;
	case VolumeType::Sphere: Debug::DrawSphere(worldTransform->GetPosition(), ((SphereVolume*)c)->GetRadius(), col); break;
	case VolumeType::Capsule: Debug::DrawCapsule(worldTransform->GetPosition(), ((CapsuleVolume*)c)->GetRadius(), ((CapsuleVolume*)c)->GetHalfHeight(), worldTransform->GetOrientation(), col); break;
	default: break;
	}
}

void TutorialGame::DebugDrawVelocity(const Vector3& velocity, Transform* worldTransform) {
	Vector4 col = Vector4(1, 0, 1, 1);
	Debug::DrawArrow(worldTransform->GetPosition(), worldTransform->GetPosition() + velocity, col);
}

void TutorialGame::DebugDrawObjectInfo(const GameObject* obj) {
	Vector3 pos = selectionObject->GetTransform().GetPosition();
	Vector3 rot = selectionObject->GetTransform().GetOrientation().ToEuler();
	string name = obj->GetName();
	string n = "Name: " + (name == "" ? "-" : name);
	string p = "Pos: (" + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z) + ")";
	string r = "Rot: (" + std::to_string(rot.x) + ", " + std::to_string(rot.y) + ", " + std::to_string(rot.z) + ")";
	renderer->DrawString(n, Vector2(1, 3), Debug::WHITE, 15.0f);
	renderer->DrawString(p, Vector2(1, 6), Debug::WHITE, 15.0f);
	renderer->DrawString(r, Vector2(1, 9), Debug::WHITE, 15.0f);
}

void TutorialGame::UpdatePauseScreen(float dt)
{
	renderer->DrawString("PAUSED", Vector2(5, 80), Debug::MAGENTA, 30.0f);
	renderer->DrawString("Press P to Unpause.", Vector2(5, 90), Debug::WHITE, 20.0f);
	renderer->DrawString("Press Esc to return to Main Menu.", Vector2(5, 95), Debug::WHITE, 20.0f);
}

void TutorialGame::UpdateWinScreen(float dt)
{
	renderer->DrawString("YOU WIN", Vector2(5, 80), Debug::MAGENTA, 30.0f);
	renderer->DrawString("Press R to Restart.", Vector2(5, 90), Debug::WHITE, 20.0f);
	renderer->DrawString("Press Esc to return to Main Menu.", Vector2(5, 95), Debug::WHITE, 20.0f);
}

void TutorialGame::InitCamera() {
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(500.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
}

void TutorialGame::InitWorld() {
	world->ClearAndErase();
	physics->Clear();

	levelLoader->ReadInLevelFile(NCL::Assets::MAPDIR + "map1.txt");
	Player* player = levelLoader->SpawnPlayer(Vector3(50, 5, 50));
	
	AxisCommand* m = new MoveCommand(player);
	inputHandler->BindAxis(0, m);

	AxisCommand* l = new LookCommand(player);
	inputHandler->BindAxis(1, l);

	Command* toggleLockCam = new ToggleBoolCommand(player->GetCamLock());
	inputHandler->BindButton(LOCK, toggleLockCam);

	Command* j = new JumpCommand(player);
	inputHandler->BindButton(JUMP, j);

	Command* d = new DescendCommand(player);
	inputHandler->BindButton(DESCEND, d);

	Command* f = new FireCommand(player);
	inputHandler->BindButton(FIRE, f);

	/*GameObject* cap1 = LevelLoader->AddCapsuleToWorld(Vector3(15, 15, 0), 3.0f, 1.5f);
	cap1->GetPhysicsObject()->SetDynamic(true);
	cap1->SetCollisionLayers(CollisionLayer::LAYER_ONE | CollisionLayer::LAYER_TWO);*/

	player1 = player;
	renderer->playerColour = GameManager::GetColourForID(player1->GetPlayerID());

	physics->BuildStaticList();
}

/*

Every frame, this code will let you perform a raycast, to see if there's an object
underneath the cursor, and if so 'select it' into a pointer, so that it can be
manipulated later. Pressing Q will let you toggle between this behaviour and instead
letting you move the camera around.

*/
bool TutorialGame::SelectObject() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q)) {
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}
	if (inSelectionMode) {
		renderer->DrawString("Press Q to change to camera mode!", Vector2(5, 85));

		if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT)) {
			if (selectionObject) {	//set colour to deselected;
				if (selectionObject->GetRenderObject())
					selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				//selectionObject->SetLayer(0);
				selectionObject = nullptr;
			}

			Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, true)) {
				selectionObject = (GameObject*)closestCollision.node;
				if (selectionObject->GetRenderObject())
					selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
				//selectionObject->SetLayer(1);
				//Ray r(selectionObject->GetTransform().GetPosition(), Vector3(0,0,-1));
				//RayCollision col;

				// Doesn't work for cubes for some reason idk

				//if (world->Raycast(r, col, true)) {
				//	((GameObject*)col.node)->GetRenderObject()->SetColour(Vector4(0, 0, 1, 1));
				//	Debug::DrawLine(r.GetPosition(), col.collidedAt, Vector4(1, 0, 0, 1), 5.0f);
				//}

				Debug::DrawLine(ray.GetPosition(), closestCollision.collidedAt, Vector4(1, 0, 0, 1), 5.0f);
				return true;
			}
			else {
				return false;
			}
		}
	}
	else {
		renderer->DrawString("Press Q to change to select mode!", Vector2(5, 85));
	}

	return false;
}

/*
If an object has been clicked, it can be pushed with the right mouse button, by an amount
determined by the scroll wheel. In the first tutorial this won't do anything, as we haven't
added linear motion into our physics system. After the second tutorial, objects will move in a straight
line - after the third, they'll be able to twist under torque aswell.
*/
void TutorialGame::MoveSelectedObject(float dt) {
	renderer->DrawString("Click Force: " + std::to_string(forceMagnitude), Vector2(10, 20));
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 100.0f;

	if (!selectionObject)
		return;

	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::RIGHT)) {
		Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());
		RayCollision closestCollision;
		if (world->Raycast(ray, closestCollision, true)) {
			if (closestCollision.node == selectionObject) {
				selectionObject->GetPhysicsObject()->AddForceAtPosition(ray.GetDirection() * forceMagnitude, closestCollision.collidedAt);
#ifndef _ORBIS
				audio->StartPlayingSound(Assets::AUDIODIR + "splat_neutral_01.ogg", selectionObject->GetTransform().GetPosition(), 1.0f);
#endif // !_ORBIS
			}
		}
	}

	DebugDrawObjectInfo(selectionObject);

	if (Window::GetKeyboard()->KeyHeld(NCL::KeyboardKeys::F))
		selectionObject->Interact(dt);

}

void TutorialGame::UpdateBGM() {
#ifndef _ORBIS

	switch (state) {
	case GameState::PLAY:
		std::cout << "play";
		bgm->PlaySongFade(Assets::AUDIODIR + "menu_music.ogg", 3.0f);
		break;
	case GameState::PAUSE:
		std::cout << "pause";
		bgm->StopMusic();
		break;
	case GameState::RESET:
		std::cout << "reset";
		bgm->StopMusic();
		break;
	default:
		bgm->StopMusic();
		break;
	}

#endif // !_ORBIS
}

void TutorialGame::PaintObject() {

	Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());
	RayCollision closestCollision;
	if (world->Raycast(ray, closestCollision, true)) {
		auto test = ((GameObject*)closestCollision.node)->GetRenderObject();

		//Debug::DrawLine(ray.GetPosition(), ray.GetPosition() * ray.GetDirection());
		Debug::DrawSphere(closestCollision.collidedAt, 0.5, Vector4(1,0,0,1), 0.f);
		if (test) {
			
			Vector2 texUV_a, texUV_b, texUV_c;
			Vector3 collisionPoint;
			Vector3 barycentric;
			CollisionDetection::GetBarycentricFromRay(ray, *test, texUV_a, texUV_b, texUV_c, barycentric, collisionPoint);
			
			
			// Get the uv from the ray
			renderer->Paint(test, barycentric, collisionPoint, texUV_a, texUV_b, texUV_c, ((GameObject*)closestCollision.node)->GetPaintRadius(), 0.2, 0.2, Vector4(0.3, 0, 0.5, 1));
		}
	}
}
