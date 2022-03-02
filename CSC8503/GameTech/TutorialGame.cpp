#include "TutorialGame.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Common/TextureLoader.h"
#include "../../Common/Quaternion.h"

#include "../CSC8503Common/InputHandler.h"
#include "../CSC8503Common/GameActor.h"
#include "../CSC8503Common/Command.h"

using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame()	{
	world		= new GameWorld();
	renderer	= new Renderer(*world);
	physics		= new PhysicsSystem(*world);
	levelLoader = new LevelLoader(world, physics);

	forceMagnitude	= 30.0f;
	useGravity		= true;
	physics->UseGravity(useGravity);

	inSelectionMode = false;

	testStateObject = nullptr;

	state = PLAY;

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

	// Character movement Go to Line 354

	inputHandler = new InputHandler();

	Command* toggleGrav = new ToggleGravityCommand(physics);
	Command* toggleDebug = new ToggleBoolCommand(&debugDraw);
	
	inputHandler->BindButtonG(toggleGrav);
	inputHandler->BindButtonJ(toggleDebug);

#pragma endregion

	InitialiseAssets();
}
/*

Each of the little demo scenarios used in the game uses the same 2 meshes, 
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it if you like!

*/
void TutorialGame::InitialiseAssets() {
	InitCamera();
	InitWorld();
}

TutorialGame::~TutorialGame() {
	delete physics;
	delete renderer;
	delete world;
	delete levelLoader;
}

void TutorialGame::UpdateGame(float dt) {
	Debug::SetRenderer(renderer);
	switch (state) {
	case PLAY: UpdateGameWorld(dt); break;
	case PAUSE: UpdatePauseScreen(dt); break;
	case WIN: UpdateWinScreen(dt); break;
	case RESET: {
		InitCamera();
		InitWorld();
		selectionObject = nullptr;
		break;
	}
	}

	inputHandler->HandleInput();

	//Debug::DrawLine(Vector3(), Vector3(0, 20, 0), Debug::RED);
	//Debug::DrawLine(Vector3(), Vector3(360, 0, 0), Debug::RED);
	//Debug::DrawLine(Vector3(360, 0, 0), Vector3(360, 0, 360), Debug::RED);
	//Debug::DrawLine(Vector3(360, 0, 360), Vector3(0, 0, 360), Debug::RED);
	//Debug::DrawLine(Vector3(0, 0, 360), Vector3(0, 0, 0), Debug::RED);

	renderer->Update(dt);

	Debug::FlushRenderables(dt);
	renderer->Render();
}

void TutorialGame::UpdateGameWorld(float dt)
{
	if (!inSelectionMode) {
		world->GetMainCamera()->UpdateCamera(dt);
	}

	UpdateKeys();

#ifdef _ORBIS
	float frameSpeed = 10 * dt;
	Camera* cam = world->GetMainCamera();
	float deadzone = 0.2f;
	if (input->GetAxis(1).y < -deadzone || input->GetAxis(1).y > deadzone) {
		cam->SetPitch(cam->GetPitch() - input->GetAxis(1).y);
	}
	if (input->GetAxis(1).x < -deadzone || input->GetAxis(1).x > deadzone) {
		cam->SetYaw(cam->GetYaw() - input->GetAxis(1).x);
	}

	// Movement
	if (input->GetAxis(0).y < -deadzone) {
		cam->SetPosition(cam->GetPosition() + Matrix4::Rotation(cam->GetYaw(), Vector3(0, 1, 0)) * Vector3(0, 0, -1) * frameSpeed);
	}
	if (input->GetAxis(0).y > deadzone) {
		cam->SetPosition(cam->GetPosition() - Matrix4::Rotation(cam->GetYaw(), Vector3(0, 1, 0)) * Vector3(0, 0, -1) * frameSpeed);
	}
	if (input->GetAxis(0).x < -deadzone) {
		cam->SetPosition(cam->GetPosition() + Matrix4::Rotation(cam->GetYaw(), Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * frameSpeed);
	}
	if (input->GetAxis(0).x > deadzone) {
		cam->SetPosition(cam->GetPosition() - Matrix4::Rotation(cam->GetYaw(), Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * frameSpeed);
	}
#endif

	if (physics->GetGravity()) {
		Debug::Print("(G)ravity on", Vector2(5, 95));
	}
	else {
		Debug::Print("(G)ravity off", Vector2(5, 95));
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

	SelectObject();
	MoveSelectedObject(dt);
	physics->Update(dt);

	world->UpdateWorld(dt);
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

void TutorialGame::UpdateKeys() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1)) {
		InitWorld(); //We can reset the simulation at any time with F1
		selectionObject = nullptr;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F2)) {
		InitCamera(); //F2 will reset the camera to a specific default place
	}
	
	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9)) {
		world->ShuffleConstraints(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10)) {
		world->ShuffleConstraints(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F7)) {
		world->ShuffleObjects(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F8)) {
		world->ShuffleObjects(false);
	}
	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::L)) {
		player1->ChangeCamLock();
	}

	DebugObjectMovement();
}

void TutorialGame::DebugObjectMovement() {
//If we've selected an object, we can manipulate it with some key presses
	if (inSelectionMode && selectionObject) {
		//Twist the selected object!
		//if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
		//	selectionObject->GetPhysicsObject()->AddTorque(Vector3(-10, 0, 0));
		//}

		//if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
		//	selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		//}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM7)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM8)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, -10, 0));
		}

		//if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
		//	selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		//}

		//if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
		//	selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -10));
		//}

		//if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
		//	selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, 10));
		//}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM5)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
		}
	}

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

	levelLoader->ReadInLevelFile("../../Assets/Maps/map1.txt");
	
	Player* player = levelLoader->AddPlayerToWorld(Vector3(0, 5, 0));

	Command* f = new MoveForwardCommand(player);
	Command* b = new MoveBackwardCommand(player);
	Command* l = new MoveLeftCommand(player);
	Command* r = new MoveRightCommand(player);

	inputHandler->BindButtonW(f);
	inputHandler->BindButtonS(b);
	inputHandler->BindButtonA(l);
	inputHandler->BindButtonD(r);

	GameObject* cap1 = levelLoader->AddCapsuleToWorld(Vector3(15, 5, 0), 3.0f, 1.5f);
	cap1->SetDynamic(true);
	cap1->SetCollisionLayers(CollisionLayer::LAYER_ONE | CollisionLayer::LAYER_TWO);

	player1 = player;

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
			}
		}
	}

	DebugDrawObjectInfo(selectionObject);

	if (Window::GetKeyboard()->KeyHeld(NCL::KeyboardKeys::F))
		selectionObject->Interact(dt);

	/*if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::UP)) {
		if (selectionObject->GetName() == "player")
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -1) * ((Player*)selectionObject)->GetSpeed() * 0.1);
		else
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -1) * forceMagnitude * 0.1);
	}
	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::DOWN)) {
		if (selectionObject->GetName() == "player")
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, 1) * ((Player*)selectionObject)->GetSpeed() * 0.1);
		else
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, 1) * forceMagnitude * 0.1);
	}
	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::LEFT)) {
		if (selectionObject->GetName() == "player")
			selectionObject->GetPhysicsObject()->AddForce(Vector3(-1, 0, 0) * ((Player*)selectionObject)->GetSpeed() * 0.1);
		else
			selectionObject->GetPhysicsObject()->AddForce(Vector3(-1, 0, 0) * forceMagnitude * 0.1);
	}
	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::RIGHT)) {
		if (selectionObject->GetName() == "player")
			selectionObject->GetPhysicsObject()->AddForce(Vector3(1, 0, 0) * ((Player*)selectionObject)->GetSpeed() * 0.1);
		else
			selectionObject->GetPhysicsObject()->AddForce(Vector3(1, 0, 0) * forceMagnitude * 0.1);
	}
	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::SHIFT)) {
		if (selectionObject->GetName() == "player")
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -1, 0) * ((Player*)selectionObject)->GetSpeed() * 0.1);
		else
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -1, 0) * forceMagnitude * 0.1);
	}
	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::SPACE)) {
		if (selectionObject->GetName() == "player")
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 1, 0) * ((Player*)selectionObject)->GetSpeed() * 0.1);
		else
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 1, 0) * forceMagnitude * 0.1);
	}*/
}