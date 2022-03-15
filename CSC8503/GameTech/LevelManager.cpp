#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "LevelManager.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Common/TextureLoader.h"
#include "../../Common/Assets.h"

#ifdef _ORBIS
	#include "../../Plugins/PlayStation4/PS4Mesh.h"
	#include "../../Plugins/PlayStation4/PS4Shader.h"
	#include "../../Plugins/PlayStation4/PS4Texture.h"
#else
	#include "../../Plugins/OpenGLRendering/OGLMesh.h"
	#include "../../Plugins/OpenGLRendering/OGLShader.h"
	#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#endif

using namespace NCL;
using namespace CSC8503;

LevelManager* LevelManager::singleton = nullptr;

LevelManager::LevelManager(PhysicsSystem* physics, Renderer* renderer) : physics(physics), renderer(renderer) {
	singleton = this;

	auto loadFunc = [](const string& name, MeshGeometry** into) {
		#ifdef _ORBIS
			*into = new PS4::PS4Mesh(name);
		#else
			*into = new OGLMesh(name);
		#endif

		(*into)->SetPrimitiveType(GeometryPrimitive::Triangles);
		(*into)->UploadToGPU();
	};

	loadFunc("courier.msh", &cubeMesh);
	loadFunc("cube.msh", &cubeMesh);
	loadFunc("sphere.msh", &sphereMesh);
	loadFunc("Male1.msh", &charMeshA);
	loadFunc("courier.msh", &charMeshB);
	loadFunc("security.msh", &enemyMesh);
	loadFunc("coin.msh", &bonusMesh);
	loadFunc("capsule.msh", &capsuleMesh);
	loadFunc("Corridor_Floor_Basic.msh", &corridorFloor);
	loadFunc("Corridor_Wall_Alert.msh", &corridorWallAlert);
	loadFunc("Corridor_Wall_Corner_In_Both.msh", &corridorWallCorner);
	loadFunc("Corridor_Wall_Light.msh", &corridorWallLight);
	loadFunc("Security_Camera.msh", &securityCamera);
	loadFunc("corridor_wall_screen.msh", &corridorWallScreen);
	loadFunc("corridor_Wall_Straight_Mid_end_L.msh", &corridorWallStraight);
	loadFunc("Corridor_Wall_Hammer.msh", &corridorWallHammer);
	
	auto loadTexFunc = [](const string& name, TextureBase** into) {
		#ifdef _ORBIS
			*into = PS4::PS4Texture::LoadTextureFromFile(NCL::Assets::TEXTUREDIR + name + ".gnf");
		#else
			*into = (OGLTexture*)TextureLoader::LoadAPITexture(name + ".png");
		#endif
	};

	loadTexFunc("Corridor_Light_Colour", &corridorFloorTex);
	loadTexFunc("corridor_wall_c", &corridorWallAlertTex);
	loadTexFunc("Corridor_Walls_Redux_Metal", &corridorWallCornerTex);
	loadTexFunc("checkerboard", &corridorWallLightTex);
	loadTexFunc("checkerboard", &securityCameraTex);
	loadTexFunc("Animated_Screens_A_Colour", &corridorWallScreenTex);
	loadTexFunc("Corridor_Walls_Redux_Colour", &corridorWallStraightTex);
	loadTexFunc("checkerboard", &corridorWallHammerTex);
	loadTexFunc("checkerboard", &basicTex);

	#ifdef _ORBIS
		basicTex = PS4::PS4Texture::LoadTextureFromFile(NCL::Assets::TEXTUREDIR + "checkerboard.gnf");
		basicShader = PS4::PS4Shader::GenerateShader(
			NCL::Assets::SHADERDIR + "PS4/VertexShader.sb",
			NCL::Assets::SHADERDIR + "PS4/PixelShader.sb"
		);

		playerTex = basicTex;
	#else
		basicTex = (OGLTexture*)TextureLoader::LoadAPITexture("checkerboard.png");
		basicShader = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");
		playerTex = (OGLTexture*)TextureLoader::LoadAPITexture("me.png");
	#endif
}

LevelManager::~LevelManager() {
	delete basicTex;
	delete basicShader;

	delete cubeMesh;
	delete sphereMesh;
	delete capsuleMesh;

	delete charMeshA;
	delete charMeshB;
	delete enemyMesh;
	delete bonusMesh;

	delete corridorFloor;
	delete corridorFloorTex;
	delete corridorWallAlert;
	delete corridorWallAlertTex;
	delete corridorWallCorner;
	delete corridorWallCornerTex;
	delete corridorWallLight;
	delete corridorWallLightTex;
	delete corridorWallScreen;
	delete corridorWallScreenTex;
	delete corridorWallStraight;
	delete corridorWallStraightTex;
	delete corridorWallHammer;
	delete corridorWallHammerTex;
	delete securityCamera;
	delete securityCameraTex;
}

void LevelManager::ReadInLevelFile(std::string filename) {
	std::ifstream file;
	std::string line;

	file.open(filename);

	if (file.is_open()) {
		while (getline(file, line)) {
			vector<std::string> lineContents;

			SplitStringOnDelimiter(line, ' ', lineContents);

			if (lineContents.size() > 0) {
				if (lineContents[0] == "FLOOR") {
					singleton->AddFloorToWorld(Vec3FromStr(lineContents[1]));
				}
				else if (lineContents[0] == "LONG_WALL") {
					singleton->AddLongWallToWorld(Vec3FromStr(lineContents[1]), Vec3FromStr(lineContents[2]), std::stof(lineContents[3]));
				}
				else if (lineContents[0] == "CORNER_WALL") {
					singleton->AddCornerWallToWorld(Vec3FromStr(lineContents[1]), Vec3FromStr(lineContents[2]), std::stoi(lineContents[3]));
				}
				else if (lineContents[0] == "SECURITY_CAM") {
					singleton->AddSecurityCameraToWorld(Vec3FromStr(lineContents[1]), std::stoi(lineContents[2]));
				}
				else if (lineContents[0] == "WALL_HAMMER") {
					singleton->AddWallHammerToWorld(Vec3FromStr(lineContents[1]), std::stoi(lineContents[2]));
				}
				else if (lineContents[0] == "SPHERE") {
					singleton->AddSphereToWorld(Vec3FromStr(lineContents[1]), std::stof(lineContents[2]), std::stof(lineContents[3]), BoolFromStr(lineContents[4]), BoolFromStr(lineContents[5]), BoolFromStr(lineContents[6]));
				}
				else if (lineContents[0] == "CUBE") {
					singleton->AddCubeToWorld(Vec3FromStr(lineContents[1]), Vec3FromStr(lineContents[2]), BoolFromStr(lineContents[3]), std::stof(lineContents[4]), std::stoi(lineContents[5]), BoolFromStr(lineContents[6]), BoolFromStr(lineContents[7]));
				}
				else if (lineContents[0] == "CAPSULE") {
					singleton->AddCapsuleToWorld(Vec3FromStr(lineContents[1]), std::stof(lineContents[2]), std::stof(lineContents[3]), std::stof(lineContents[4]));
				}
				else if (lineContents[0] == "PAINT_WALL") {
					singleton->AddPaintWallToWorld(Vec3FromStr(lineContents[1]), Vector3(5, 5, 4), std::stoi(lineContents[2]));
				}
				else if (lineContents[0] == "POWERUP") {
					singleton->AddPowerUpToWorld(Vec3FromStr(lineContents[1]), (const PowerUpType) std::stoi(lineContents[2]));
				}
			}
		}

		file.close();
	}
	else {
		std::cout << "Failed to load map" << std::endl;
	}
}

void LevelManager::SplitStringOnDelimiter(const std::string& s, char delim, vector<std::string>& result) {
	std::istringstream iss(s);
	std::string item;

	while (std::getline(iss, item, delim)) {
		result.push_back(item);
	}
}

Vector3 LevelManager::Vec3FromStr(std::string input) {
	vector<std::string> values;

	SplitStringOnDelimiter(input, ',', values);

	return Vector3(std::stof(values[0]), std::stof(values[1]), std::stof(values[2]));
}

bool LevelManager::BoolFromStr(std::string input) {
	return input == "TRUE";
}

Player* LevelManager::SpawnPlayer(const Vector3& position) {
	Player* character = new Player(GameWorld::GetMainCamera(), "Player");

	return (Player*) singleton->AddPlayerObjectToWorld(position, character);
}

GameObject* LevelManager::SpawnDummyPlayer(const Vector3& position) {
	GameObject* character = new GameObject("Dummy");

	return singleton->AddPlayerObjectToWorld(position, character);
}

Projectile* LevelManager::SpawnProjectile(Player* owner, const float& initialSpeed, const float& meshSize) {
	return SpawnProjectile((GameObject*)owner, owner->GetCam()->GetPitch(), owner->GetPlayerID(), initialSpeed, meshSize);
}

Projectile* LevelManager::SpawnProjectile(GameObject* owner, float pitch, int playerID, const float& initialSpeed, const float& meshSize) {
	return singleton->AddProjectileToWorld(owner, pitch, playerID, initialSpeed, meshSize);
}

GameObject* LevelManager::AddFloorToWorld(const Vector3& position) {
	GameObject* floor = new GameObject("Floor");
	Vector3 floorSize = Vector3(250, 1, 250);

	SetFieldsForCube(floor, position, floorSize, CollisionLayer::LAYER_ONE, false, false, false, 0, DEF_ELASTICITY, DEF_LDAMPING, DEF_FRICTION);

	GameWorld::AddGameObject(floor);

	return floor;
}

GameObject* LevelManager::AddAABBWallToWorld(const Vector3& position, Vector3 dimensions, int rotation, string name) {
	GameObject* cube = new GameObject(name);
	CollisionVolume* volume = (CollisionVolume*) new AABBVolume(dimensions);

	SetFields(cube, volume, position, dimensions * 2, false);
	cube->SetPhysicsObject(GetPhysicsObject(&cube->GetTransform(), volume, CollisionLayer::LAYER_ONE, false, 0, DEF_ELASTICITY, DEF_LDAMPING, DEF_FRICTION));

	cube->GetPhysicsObject()->InitCubeInertia();
	cube->GetTransform().SetOrientation(Quaternion::EulerAnglesToQuaternion(0, rotation, 0));
	
	GameWorld::AddGameObject(cube);

	return cube;
}

GameObject* LevelManager::AddOBBWallToWorld(const Vector3& position, Vector3 dimensions, int rotation, string name) {
	GameObject* cube = new GameObject(name);
	CollisionVolume* volume = (CollisionVolume*) new OBBVolume(dimensions + Vector3(2, 10, 0));

	SetFields(cube, volume, position, dimensions * 2, false);

	cube->SetPhysicsObject(GetPhysicsObject(&cube->GetTransform(), volume, CollisionLayer::LAYER_ONE, false, 0, DEF_ELASTICITY, DEF_LDAMPING, DEF_FRICTION));
	cube->GetPhysicsObject()->InitCubeInertia();
	cube->GetTransform().SetOrientation(Quaternion::EulerAnglesToQuaternion(0, rotation, 0));

	cube->SetRenderObject(nullptr);

	GameWorld::AddGameObject(cube);

	return cube;
}

GameObject* LevelManager::AddLongWallToWorld(const Vector3& position, Vector3 dimensions, int rotation, string name) {
	Vector3 location = position + Vector3(0, 10, 0);

	if (position.x < 0)
		location += Vector3(5, 0, -5);
	if (position.x > 0)
		location += Vector3(-7, 0, -4);

	if (position.z < 0)
		location += Vector3(1, 0, 10);
	if (position.z > 0)
		location += Vector3(1, 0, -2);

	GameObject* physicalObject = AddAABBWallToWorld(location, dimensions, rotation, name);
	physicalObject->GetPhysicsObject()->Sleep();
	if (rotation == 90 || rotation == 270)
	{
		for (int i = -dimensions.z; i < dimensions.z; i += 10)
		{
			AddRenderPartToWorld(Vector3(position.x, position.y, position.z + i), Vector3(5, 5, 4), rotation, corridorWallStraight, corridorWallAlertTex);
		}
		return physicalObject;
	}
	if (rotation == 180 || rotation == 0)
	{
		for (int i = -dimensions.x; i < dimensions.x; i += 10)
		{
			AddRenderPartToWorld(Vector3(position.x + i, position.y, position.z), Vector3(5, 5, 4), rotation, corridorWallStraight, corridorWallAlertTex);
		}
		return physicalObject;
	}
	return physicalObject;
}

GameObject* LevelManager::AddPaintWallToWorld(const Vector3& position, Vector3 dimensions, int rotation, string name)
{
	GameObject* cube = new GameObject(name);
	CollisionVolume* volume = (CollisionVolume*) new OBBVolume(dimensions + Vector3(2, 10, 0));

	SetFields(cube, volume, position, dimensions * 2, false);

	cube->SetPhysicsObject(GetPhysicsObject(&cube->GetTransform(), volume, CollisionLayer::LAYER_ONE, false, 0, DEF_ELASTICITY, DEF_LDAMPING, DEF_FRICTION));
	cube->GetPhysicsObject()->InitCubeInertia();
	cube->GetTransform().SetOrientation(Quaternion::EulerAnglesToQuaternion(0, rotation, 0));

	#ifdef _WIN64
		cube->SetRenderObject(new RenderObject(&cube->GetTransform(), corridorWallStraight, corridorWallAlertTex, OGLTexture::RGBATextureEmpty(corridorWallAlertTex->GetHeight()/16, corridorWallAlertTex->GetWidth()/16), basicShader));
	#else
		cube->SetRenderObject(new RenderObject(&cube->GetTransform(), corridorWallStraight, corridorWallAlertTex, basicShader));
	#endif

	GameWorld::AddGameObject(cube);

	return cube;
}

void        LevelManager::AddCornerWallToWorld(const Vector3& position, Vector3 dimensions, int rotation)
{
	Vector3 location = position + Vector3(0, 15, 0);
	if (rotation == 45)
		location += Vector3(3, 0, 3);
	if (rotation == 135)
		location += Vector3(3, 0, -3);
	if (rotation == 225)
		location += Vector3(-3, 0, -3);
	if (rotation == 315)
		location += Vector3(-3, 0, 3);

	GameObject* physicalObject = AddOBBWallToWorld(location, dimensions, rotation, "Corner wall");
	physicalObject->GetPhysicsObject()->Sleep();
	AddRenderPartToWorld(position, dimensions, rotation, corridorWallCorner, corridorWallAlertTex);
	return;
}

void		LevelManager::AddSecurityCameraToWorld(const Vector3& position, int rotation)
{
	Vector3 location = position + Vector3(0, 24, 0);
	Vector3 dimensions = Vector3(2, 3, 4);
	if (rotation == 0)
	{
		dimensions = Vector3(2, 2, 4);
		location += Vector3(0, 0, -4);
	}
	if (rotation == 90)
	{
		dimensions = Vector3(4, 2, 2);
		location += Vector3(-4, 0, 0);
	}
	if (rotation == 180)
	{
		dimensions = Vector3(2, 2, 4);
		location += Vector3(0, 0, 4);
	}
	if (rotation == 270)
	{
		dimensions = Vector3(4, 2, 2);
		location += Vector3(4, 0, 0);
	}

	GameObject* physicalObject = AddAABBWallToWorld(location, dimensions, rotation, "Security Camera");
	physicalObject->GetPhysicsObject()->Sleep();
	AddRenderPartToWorld(position, Vector3(5, 5, 5), rotation, securityCamera, securityCameraTex);
	return;
}

void		LevelManager::AddWallHammerToWorld(const Vector3& position, int rotation)
{
	Vector3 location = position + Vector3(0, 18.25, 0);
	Vector3 dimensions = Vector3(1, 5.5, 4);
	if (rotation == 0)
	{
		dimensions = Vector3(4, 5.5, 1);
		location += Vector3(-0.25, 0, -5.5);
	}
	if (rotation == 90)
	{
		dimensions = Vector3(1, 5.5, 4);
		location += Vector3(-5.5, 0, 0.25);
	}
	if (rotation == 180)
	{
		dimensions = Vector3(4, 5.5, 1);
		location += Vector3(0.25, 0, 5.5);
	}
	if (rotation == 270)
	{
		dimensions = Vector3(1, 5.5, 4);
		location += Vector3(5.5, 0, -0.25);
	}

	GameObject* physicalObject = AddAABBWallToWorld(location, dimensions, rotation, "Wall Hammer");
	physicalObject->GetPhysicsObject()->Sleep();
	AddRenderPartToWorld(position, Vector3(10, 10, 6), rotation, corridorWallHammer, corridorWallHammerTex);
	return;
}

GameObject* LevelManager::AddRenderPartToWorld(const Vector3& position, Vector3 dimensions, int rotation, MeshGeometry* mesh, TextureBase* texture) {
	GameObject* cube = new GameObject();
	cube->SetBoundingVolume(nullptr);
	cube->SetPhysicsObject(nullptr);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2)
		.SetOrientation(Quaternion::EulerAnglesToQuaternion(0, rotation, 0));

#ifdef _WIN64
	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), mesh, texture, OGLTexture::RGBATextureEmpty(texture->GetWidth()/16,texture->GetHeight()/16), basicShader));
#endif
#ifdef _ORBIS
	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), mesh, texture, basicShader));
#endif

	GameWorld::AddGameObject(cube);
	return cube;
}

GameObject* LevelManager::AddSphereToWorld(const Vector3& position, float radius, int layer, bool isTrigger, bool dynamic, bool rubber, bool hollow, float inverseMass) {
	GameObject* sphere = new GameObject();

	SetFieldsForSphere(sphere, position, (CollisionLayer) layer, radius, isTrigger, dynamic, rubber, hollow, inverseMass);

	GameWorld::AddGameObject(sphere);

	return sphere;
}

GameObject* LevelManager::AddCubeToWorld(const Vector3& position, Maths::Vector3 dimensions, int layer, bool isTrigger, bool dynamic, bool OBB, float inverseMass) {
	GameObject* cube = new GameObject();

	SetFieldsForCube(cube, position, dimensions, (CollisionLayer) layer, isTrigger, dynamic, OBB, inverseMass);

	GameWorld::AddGameObject(cube);

	return cube;
}

GameObject* LevelManager::AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, int layer, bool isTrigger, bool dynamic, float inverseMass) {
	GameObject* capsule = new GameObject();

	SetFieldsForCapsule(capsule, position, (CollisionLayer) layer, halfHeight, radius, isTrigger, dynamic, inverseMass);

	GameWorld::AddGameObject(capsule);

	return capsule;
}

GameObject* LevelManager::AddPlayerObjectToWorld(const Vector3& position, GameObject* character) {
	float meshSize = 3.0f;

	CapsuleVolume* volume = new CapsuleVolume(0.85f * meshSize, 0.3f * meshSize);

	SetFields(character, volume, position, Vector3(meshSize, meshSize, meshSize), false);
	character->SetPhysicsObject(GetPhysicsObject(&character->GetTransform(), volume, CollisionLayer::LAYER_ONE | CollisionLayer::LAYER_THREE, true, 5.0f, DEF_ELASTICITY, 3.0f, false));
	character->SetRenderObject(GetRenderObject(&character->GetTransform(), charMeshA, nullptr, basicShader, Vector4(0.5,1,0.5,1)));

	character->GetPhysicsObject()->InitSphereInertia();
	character->GetPhysicsObject()->SetCanSleep(false);
	character->GetPhysicsObject()->SetShouldApplyAngular(false);

	GameWorld::AddGameObject(character);

	return character;
}

PowerUp*    LevelManager::AddPowerUpToWorld(const Vector3& position, const PowerUpType& ability, const float& radius) {
	PowerUp* powerup = nullptr;
	Vector4 colour;

	switch (ability) 
	{
	case (PowerUpType::FireRate):
		powerup = new FireRate();
		colour = Debug::YELLOW;
		break;
	case (PowerUpType::Heal):
		powerup = new Heal();
		colour = Debug::RED;
		break;
	case (PowerUpType::SpeedBoost):
		powerup = new SpeedBoost();
		colour = Debug::CYAN;
		break;
	}

	if (powerup == nullptr) {
		return powerup;
	}

	SetFieldsForSphere(powerup, position, CollisionLayer::LAYER_FOUR, radius, true, false, false, false, 0);

	GameWorld::AddGameObject(powerup);

	return powerup;
}

Projectile* LevelManager::AddProjectileToWorld(GameObject* owner, float pitch, int playerID, const float& initialSpeed, const float& meshSize) {
	float inverseMass = 1.0f;

	Vector3 ownerRot = owner->GetTransform().GetOrientation().ToEuler();

	Vector3 camForwardVector = (Matrix4::Rotation(ownerRot.y, Vector3(0, 1, 0)) * Matrix4::Rotation(pitch, Vector3(1, 0, 0)) * Vector3(0, 0, -1)).Normalised();

	Projectile* projectile = new Projectile(renderer, playerID);

	SphereVolume* volume = new SphereVolume(meshSize * 1.4);// / 2.0f * meshSize * 1.3f);
	projectile->SetBoundingVolume((CollisionVolume*)volume);

	projectile->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition((owner->GetTransform().GetPosition() + Vector3(0, 2.5f, 0)) + camForwardVector * 5.0f); //+ offset);

	projectile->SetRenderObject(new RenderObject(&projectile->GetTransform(), capsuleMesh, nullptr, basicShader));
	projectile->SetPhysicsObject(new PhysicsObject(&projectile->GetTransform(), projectile->GetBoundingVolume()));

	projectile->GetPhysicsObject()->SetInverseMass(inverseMass);
	projectile->GetPhysicsObject()->InitSphereInertia();

	float velocityDueToMovement = Vector3::Dot(camForwardVector, owner->GetPhysicsObject()->GetLinearVelocity());
	if (velocityDueToMovement < 0.0f) velocityDueToMovement = 0.0f;
	projectile->GetPhysicsObject()->AddAcceleration(camForwardVector * (initialSpeed + velocityDueToMovement));
	projectile->GetTransform().SetOrientation(Quaternion(Matrix3::Rotation(-acos(Vector3::Dot(Vector3(0, 1, 0), camForwardVector)) * 180.0f / 3.14f, Vector3::Cross(camForwardVector, Vector3(0, 1, 0)).Normalised())));

	projectile->GetPhysicsObject()->SetLinearDamping(0.1f);
	projectile->GetPhysicsObject()->SetDynamic(true);
	projectile->SetCollisionLayers(CollisionLayer::LAYER_FIVE);

	GameWorld::AddGameObject(projectile);

	return projectile;
}

void LevelManager::SetFieldsForSphere(GameObject* sphere, const Vector3& position, CollisionLayer layers, float radius, bool isTrigger, bool dynamic, bool rubber, bool hollow, float inverseMass, float elasticity, float lDamping, float friction) {
	CollisionVolume* volume = (CollisionVolume*) new SphereVolume(radius);
	
	SetFields(sphere, volume, position, Vector3(radius, radius, radius), isTrigger);
	sphere->SetRenderObject(GetRenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(GetPhysicsObject(&sphere->GetTransform(), volume, layers, dynamic, inverseMass, rubber ? 0.9f : 0.2, lDamping, friction));

	if (hollow)	sphere->GetPhysicsObject()->InitHollowSphereInertia();
	else		sphere->GetPhysicsObject()->InitSphereInertia();
}

void LevelManager::SetFieldsForCube(GameObject* cube, const Vector3& position, Vector3 dimensions, CollisionLayer layers, bool isTrigger, bool dynamic, bool OBB, float inverseMass, float elasticity, float lDamping, float friction) {
	CollisionVolume* volume = OBB ? (CollisionVolume*) new OBBVolume(dimensions) : (CollisionVolume*) new AABBVolume(dimensions);

	SetFields(cube, volume, position, dimensions * 2, isTrigger);
	cube->SetPhysicsObject(GetPhysicsObject(&cube->GetTransform(), volume, layers, dynamic, inverseMass, elasticity, lDamping, friction));

	#ifdef _WIN64
		cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, OGLTexture::RGBATextureEmpty(basicTex->GetWidth(), basicTex->GetHeight()), basicShader));
	#else
		cube->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	#endif
	
	cube->GetPhysicsObject()->InitCubeInertia();
}

void LevelManager::SetFieldsForCapsule(GameObject* capsule, const Vector3& position, CollisionLayer layers, float halfHeight, float radius, bool isTrigger, bool dynamic, float inverseMass, float elasticity, float lDamping, float friction) {
	CapsuleVolume* volume = new CapsuleVolume(halfHeight, radius);

	SetFields(capsule, volume, position, Vector3(radius * 2, halfHeight, radius * 2), isTrigger);

	capsule->SetPhysicsObject(GetPhysicsObject(&capsule->GetTransform(), volume, layers, dynamic, inverseMass, elasticity, lDamping, friction));
	capsule->SetRenderObject(GetRenderObject(&capsule->GetTransform(), capsuleMesh, basicTex, basicShader));

	capsule->GetPhysicsObject()->InitCubeInertia();
}

void LevelManager::SetFields(GameObject* obj, CollisionVolume* volume, const Vector3& position, const Vector3& dimensions, bool isTrigger) {
	obj->SetBoundingVolume(volume);
	obj->SetTrigger(isTrigger);
	obj->GetTransform()
		.SetScale(dimensions)
		.SetPosition(position);
}

PhysicsObject* LevelManager::GetPhysicsObject(Transform* transform, CollisionVolume* volume, int layers, bool dynamic, float inverseMass, float elasticity, float lDamping, float friction) {
	PhysicsObject* physics = new PhysicsObject(transform, volume);

	physics->SetCollisionLayers(layers);
	physics->SetInverseMass(inverseMass);
	physics->SetElasticity(elasticity);
	physics->SetLinearDamping(lDamping);
	physics->SetDynamic(dynamic);

	return physics;
}

RenderObject* LevelManager::GetRenderObject(Transform* transform, MeshGeometry* mesh, TextureBase* texture, ShaderBase* shader, Vector4 colour) {
	RenderObject* renderer = new RenderObject(transform, mesh, texture, shader);

	renderer->SetColour(colour);

	return renderer;
}