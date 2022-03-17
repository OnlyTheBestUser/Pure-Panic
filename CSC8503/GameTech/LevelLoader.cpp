#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "LevelLoader.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Common/TextureLoader.h"
#include "../../Common/Assets.h"

#ifdef _ORBIS
#include "../../Plugins/PlayStation4/PS4Mesh.h"
#include "../../Plugins/PlayStation4/PS4Shader.h"
#include "../../Plugins/PlayStation4/PS4Texture.h"
#endif

#ifdef _WIN64
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#endif

using namespace NCL;
using namespace CSC8503;

LevelLoader::LevelLoader(GameWorld* world, PhysicsSystem* physics, Renderer* renderer) : world(world), physics(physics), renderer(renderer) {
	auto loadFunc = [](const string& name, MeshGeometry** into) {
#ifdef _ORBIS
		* into = new PS4::PS4Mesh(name);
#else
		* into = new OGLMesh(name);
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
#ifdef _WIN64
	basicTex = (OGLTexture*)TextureLoader::LoadAPITexture("checkerboard.png");
	basicShader = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");
	playerTex = (OGLTexture*)TextureLoader::LoadAPITexture("me.png");
#endif
#ifdef _ORBIS
	basicTex = PS4::PS4Texture::LoadTextureFromFile(NCL::Assets::TEXTUREDIR + "checkerboard.gnf");
	basicShader = PS4::PS4Shader::GenerateShader(
		NCL::Assets::SHADERDIR + "PS4/VertexShader.sb",
		NCL::Assets::SHADERDIR + "PS4/PixelShader.sb"
	);
	playerTex = basicTex;
#endif
}

LevelLoader::~LevelLoader() {
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

void LevelLoader::ReadInLevelFile(std::string filename) {
	std::ifstream file;
	std::string line;

	file.open(filename);

	if (file.is_open()) {
		while (getline(file, line)) {
			vector<std::string> lineContents;

			SplitStringOnDelimiter(line, ' ', lineContents);

			if (lineContents.size() > 0) {
				if (lineContents[0] == "FLOOR") {
					AddFloorToWorld(Vec3FromStr(lineContents[1]));
				}
				else if (lineContents[0] == "LONG_WALL") {
					AddLongWallToWorld(Vec3FromStr(lineContents[1]), Vec3FromStr(lineContents[2]), std::stof(lineContents[3]));
				}
				else if (lineContents[0] == "CORNER_WALL") {
					AddCornerWallToWorld(Vec3FromStr(lineContents[1]), Vec3FromStr(lineContents[2]), std::stoi(lineContents[3]));
				}
				else if (lineContents[0] == "SECURITY_CAM") {
					AddSecurityCameraToWorld(Vec3FromStr(lineContents[1]), std::stoi(lineContents[2]));
				}
				else if (lineContents[0] == "WALL_HAMMER") {
					AddWallHammerToWorld(Vec3FromStr(lineContents[1]), std::stoi(lineContents[2]));
				}
				else if (lineContents[0] == "PAINT_WALL") {
					AddPaintWallToWorld(Vec3FromStr(lineContents[1]), Vector3(5, 5, 4), std::stoi(lineContents[2]));
				}
			}
		}

		file.close();
	}
	else {
		std::cout << "Failed to load map" << std::endl;
	}
}

void LevelLoader::SplitStringOnDelimiter(const std::string& s, char delim, vector<std::string>& result) {
	std::istringstream iss(s);
	std::string item;
	while (std::getline(iss, item, delim)) {
		result.push_back(item);
	}
}

Vector3 LevelLoader::Vec3FromStr(std::string input) {
	vector<std::string> values;

	SplitStringOnDelimiter(input, ',', values);

	return Vector3(std::stof(values[0]), std::stof(values[1]), std::stof(values[2]));
}

Player* LevelLoader::AddPlayerToWorld(const Vector3& position) {
	float meshSize = 3.0f;
	float inverseMass = 5.0f;

	Player* character = new Player(world->GetMainCamera(), this, world, "Player");

	CapsuleVolume* volume = new CapsuleVolume(0.85f * meshSize, 0.3f * meshSize);
	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), charMeshA, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->SetLinearDamping(3.0f);
	character->GetPhysicsObject()->InitSphereInertia();
	character->GetPhysicsObject()->SetFriction(false);
	character->GetPhysicsObject()->SetShouldApplyAngular(false);

	character->GetPhysicsObject()->SetDynamic(true);
	character->GetPhysicsObject()->SetCanSleep(false);

	character->SetCollisionLayers(CollisionLayer::LAYER_ONE | CollisionLayer::LAYER_THREE);

	world->AddGameObject(character);

	return character;
}

GameObject* LevelLoader::AddDummyPlayerToWorld(const Vector3& position)
{
	float meshSize = 3.0f;
	float inverseMass = 0.5f;

	GameObject* character = new GameObject("Dummy");

	CapsuleVolume* volume = new CapsuleVolume(0.85f * meshSize, 0.3f * meshSize);
	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), charMeshA, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->SetFriction(1.0f);
	character->GetPhysicsObject()->SetLinearDamping(10.0f);
	character->GetPhysicsObject()->InitSphereInertia();
	character->GetPhysicsObject()->SetShouldApplyAngular(false);
	character->GetPhysicsObject()->SetDynamic(true);
	character->SetCollisionLayers(CollisionLayer::LAYER_ONE | CollisionLayer::LAYER_THREE);

	world->AddGameObject(character);

	return character;
}

GameObject* LevelLoader::AddFloorToWorld(const Maths::Vector3& position) {
	GameObject* floor = new GameObject("Floor");

	Vector3 floorSize = Vector3(250, 1, 250);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position);
#ifdef _WIN64
	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, OGLTexture::RGBATextureEmpty(basicTex->GetWidth(), basicTex->GetHeight()), basicShader));
#else
	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, PS4::PS4Texture::EmptyTex(basicTex->GetWidth(), basicTex->GetHeight()),basicShader));
	//floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicTex, basicShader));
#endif
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	floor->SetCollisionLayers(CollisionLayer::LAYER_ONE);
	floor->GetPhysicsObject()->SetDynamic(false);
	world->AddGameObject(floor);
	return floor;
}

GameObject* LevelLoader::AddAABBWallToWorld(const Vector3& position, Vector3 dimensions, int rotation, string name) {
	GameObject* cube = new GameObject(name);
	AABBVolume* volume = new AABBVolume(dimensions);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2)
		.SetOrientation(Quaternion::EulerAnglesToQuaternion(0, rotation, 0));

	cube->SetRenderObject(nullptr);
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(0.0f);
	cube->GetPhysicsObject()->InitCubeInertia();

	cube->SetCollisionLayers(CollisionLayer::LAYER_ONE);
	cube->GetPhysicsObject()->SetDynamic(false);
	world->AddGameObject(cube);
	return cube;
}

GameObject* LevelLoader::AddOBBWallToWorld(const Vector3& position, Vector3 dimensions, int rotation, string name) {
	GameObject* cube = new GameObject(name);
	OBBVolume* volume = new OBBVolume(dimensions + Vector3(2, 10, 0));
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2)
		.SetOrientation(Quaternion::EulerAnglesToQuaternion(0, rotation, 0));

	cube->SetRenderObject(nullptr);
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(0.0f);
	cube->GetPhysicsObject()->InitCubeInertia();

	cube->SetCollisionLayers(CollisionLayer::LAYER_ONE);
	cube->GetPhysicsObject()->SetDynamic(false);
	world->AddGameObject(cube);
	return cube;
}

GameObject* LevelLoader::AddLongWallToWorld(const Vector3& position, Vector3 dimensions, int rotation, string name) {
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

GameObject* LevelLoader::AddPaintWallToWorld(const Vector3& position, Vector3 dimensions, int rotation, string name)
{
	GameObject* cube = new GameObject(name);
	OBBVolume* volume = new OBBVolume(dimensions + Vector3(0, 10, -2));
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2)
		.SetOrientation(Quaternion::EulerAnglesToQuaternion(0, rotation, 0));

	if (rotation == 0)
		cube->GetTransform().SetOffset(Vector3(0, 15, 6));
	if (rotation == 90)
		cube->GetTransform().SetOffset(Vector3(6, 15, 0));
	if (rotation == 180)
		cube->GetTransform().SetOffset(Vector3(0, 15, -6));
	if (rotation == 270)
		cube->GetTransform().SetOffset(Vector3(-6, 15, 0));

#ifdef _WIN64
	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), corridorWallStraight, corridorWallAlertTex, OGLTexture::RGBATextureEmpty(corridorWallAlertTex->GetHeight()/16, corridorWallAlertTex->GetWidth()/16), basicShader));
#else
	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), corridorWallStraight, corridorWallAlertTex, PS4::PS4Texture::EmptyTex(corridorWallAlertTex->GetHeight() / 16, corridorWallAlertTex->GetWidth() / 16), basicShader));
#endif
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(0.0f);
	cube->GetPhysicsObject()->InitCubeInertia();

	cube->SetCollisionLayers(CollisionLayer::LAYER_ONE);
	cube->GetPhysicsObject()->SetDynamic(false);
	world->AddGameObject(cube);
	return cube;
}

void LevelLoader::AddCornerWallToWorld(const Vector3& position, Vector3 dimensions, int rotation)
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

void LevelLoader::AddSecurityCameraToWorld(const Vector3& position, int rotation)
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

void LevelLoader::AddWallHammerToWorld(const Vector3& position, int rotation)
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

GameObject* LevelLoader::AddRenderPartToWorld(const Vector3& position, Vector3 dimensions, int rotation, MeshGeometry* mesh, TextureBase* texture) {
	GameObject* cube = new GameObject();
	cube->SetBoundingVolume(nullptr);

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
	cube->SetPhysicsObject(nullptr);

	world->AddGameObject(cube);
	return cube;
}

GameObject* LevelLoader::AddSphereToWorld(const Maths::Vector3& position, float radius, float inverseMass, bool rubber, bool hollow, bool dynamic) {
	GameObject* sphere = new GameObject("Sphere");

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	if (hollow)
		sphere->GetPhysicsObject()->InitHollowSphereInertia();
	else
		sphere->GetPhysicsObject()->InitSphereInertia();

	if (rubber)
		sphere->GetPhysicsObject()->SetElasticity(0.9f);
	else
		sphere->GetPhysicsObject()->SetElasticity(0.2f);

	sphere->SetCollisionLayers(CollisionLayer::LAYER_ONE);

	world->AddGameObject(sphere);
	sphere->GetPhysicsObject()->SetDynamic(dynamic);
	return sphere;
}

GameObject* LevelLoader::AddCubeToWorld(const Maths::Vector3& position, Maths::Vector3 dimensions, bool OBB, float inverseMass, int layer, bool isTrigger, bool dynamic) {
	GameObject* cube = new GameObject();
	if (OBB) {
		OBBVolume* volume = new OBBVolume(dimensions);
		cube->SetBoundingVolume((CollisionVolume*)volume);
	}
	else {
		AABBVolume* volume = new AABBVolume(dimensions);
		cube->SetBoundingVolume((CollisionVolume*)volume);
	}

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);


	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();
	cube->GetPhysicsObject()->SetElasticity(0.2f);

	world->AddGameObject(cube);
	switch (layer)
	{
	default:
		cube->SetCollisionLayers(CollisionLayer::LAYER_ONE);
		break;
	case 1:
		cube->SetCollisionLayers(CollisionLayer::LAYER_ONE);
		break;
	case 2:
		cube->SetCollisionLayers(CollisionLayer::LAYER_TWO);
		break;
	case 3:
		cube->SetCollisionLayers(CollisionLayer::LAYER_THREE);
		break;
	}
	cube->SetTrigger(isTrigger);
	cube->GetPhysicsObject()->SetDynamic(dynamic);
	return cube;
}

GameObject* LevelLoader::AddCapsuleToWorld(const Maths::Vector3& position, float halfHeight, float radius, float inverseMass) {
	GameObject* capsule = new GameObject("Capsule");

	CapsuleVolume* volume = new CapsuleVolume(halfHeight, radius);
	capsule->SetBoundingVolume((CollisionVolume*)volume);

	capsule->GetTransform()
		.SetScale(Vector3(radius * 2, halfHeight, radius * 2))
		.SetPosition(position);

	capsule->SetRenderObject(new RenderObject(&capsule->GetTransform(), capsuleMesh, basicTex, basicShader));
	capsule->SetPhysicsObject(new PhysicsObject(&capsule->GetTransform(), capsule->GetBoundingVolume()));

	capsule->GetPhysicsObject()->SetInverseMass(inverseMass);
	capsule->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(capsule);

	return capsule;
}

Projectile* LevelLoader::SpawnProjectile(Player* owner, const float& initialSpeed, const float& meshSize) {
	return SpawnProjectile((GameObject*)owner, owner->GetCam()->GetPitch(), owner->GetPlayerID(), initialSpeed, meshSize);
}

Projectile* LevelLoader::SpawnProjectile(GameObject* owner, float pitch, int playerID, const float& initialSpeed, const float& meshSize)
{
	float inverseMass = 1.0f;

	Vector3 ownerRot = owner->GetTransform().GetOrientation().ToEuler();

	Vector3 camForwardVector = (Matrix4::Rotation(ownerRot.y, Vector3(0,1,0)) * Matrix4::Rotation(pitch, Vector3(1,0,0)) * Vector3(0,0,-1)).Normalised();

	Projectile* projectile = new Projectile(*world, renderer, playerID);

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

	world->AddGameObject(projectile);

	return projectile;
}

PowerUp* LevelLoader::AddPowerUpToWorld(const Vector3& position, const PowerUpType& ability, const float& radius) {
	PowerUp* powerup = nullptr;
	Vector4 colour;

	switch (ability) {
	case(PowerUpType::FireRate):
		powerup = new FireRate(*world);
		colour = Debug::YELLOW;
		break;
	case(PowerUpType::Heal):
		powerup = new Heal(*world);
		colour = Debug::RED;
		break;
	case(PowerUpType::SpeedBoost):
		powerup = new SpeedBoost(*world);
		colour = Debug::CYAN;
		break;
	}

	if (powerup == nullptr) {
		return powerup;
	}

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	powerup->SetBoundingVolume((CollisionVolume*)volume);

	powerup->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	powerup->SetRenderObject(new RenderObject(&powerup->GetTransform(), sphereMesh, basicTex, basicShader));
	powerup->GetRenderObject()->SetColour(colour);

	powerup->SetPhysicsObject(new PhysicsObject(&powerup->GetTransform(), powerup->GetBoundingVolume()));

	powerup->GetPhysicsObject()->SetInverseMass(0);
	powerup->GetPhysicsObject()->SetDynamic(true);
	powerup->SetTrigger(true);

	world->AddGameObject(powerup);

	powerup->SetCollisionLayers(CollisionLayer::LAYER_FOUR);

	return powerup;
}
