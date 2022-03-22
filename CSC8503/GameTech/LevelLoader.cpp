#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "LevelLoader.h"
#include "NetworkedGame.h"
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

LevelLoader* LevelLoader::singleton = nullptr;

LevelLoader::LevelLoader(PhysicsSystem* physics, Renderer* renderer) : physics(physics), renderer(renderer) {
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
	loadTexFunc("corridor_wall_n", &corridorWallNormal);
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
					singleton->AddPaintWallToWorld(Vec3FromStr(lineContents[1]), Vector3(5, 5, 4), std::stoi(lineContents[2]), lineContents[0]);
				}
				else if (lineContents[0] == "POWERUP") {
					singleton->AddPowerUpToWorld(Vec3FromStr(lineContents[1]), (const PowerUpType) std::stoi(lineContents[2]));
				}
				else if (lineContents[0] == "SPAWNPOINT") {
					singleton->AddSpawnPointToWorld(Vec3FromStr(lineContents[1]));
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

bool LevelLoader::BoolFromStr(std::string input) {
	return input == "TRUE";
}

Player* LevelLoader::SpawnPlayer(const Vector3& position) {
	Player* character = new Player(GameWorld::GetMainCamera(), "Player");

	return (Player*) singleton->AddPlayerObjectToWorld(position, character);
}

GameObject* LevelLoader::SpawnDummyPlayer(const Vector3& position) {
	GameObject* character = new GameObject("Dummy");

	return singleton->AddPlayerObjectToWorld(position, character);
}

GameObject* LevelLoader::AddFloorToWorld(const Vector3& position) {
	GameObject* floor = new GameObject("Floor", 1.5f);
	Vector3 floorSize = Vector3(250, 1, 250);

	SetFieldsForCube(floor, position, floorSize, CollisionLayer::LAYER_ONE, false, false, false, 0, DEF_ELASTICITY, DEF_LDAMPING, DEF_FRICTION);

	GameWorld::AddGameObject(floor);

	return floor;
}

GameObject* LevelLoader::AddAABBWallToWorld(const Vector3& position, Vector3 dimensions, int rotation, string name) {
	GameObject* cube = new GameObject(name);
	CollisionVolume* volume = (CollisionVolume*) new AABBVolume(dimensions);

	SetMiscFields(cube, volume, position, dimensions * 2, false);
	cube->SetPhysicsObject(GetPhysicsObject(&cube->GetTransform(), volume, CollisionLayer::LAYER_ONE, false, 0, DEF_ELASTICITY, DEF_LDAMPING, DEF_FRICTION));

	cube->GetPhysicsObject()->InitCubeInertia();
	cube->GetTransform().SetOrientation(Quaternion::EulerAnglesToQuaternion(0, rotation, 0));
	
	GameWorld::AddGameObject(cube);

	return cube;
}

GameObject* LevelLoader::AddOBBWallToWorld(const Vector3& position, Vector3 dimensions, int rotation, string name) {
	GameObject* cube = new GameObject(name);
	CollisionVolume* volume = (CollisionVolume*) new OBBVolume(dimensions + Vector3(2, 10, 0));

	SetMiscFields(cube, volume, position, dimensions * 2, false);

	cube->SetPhysicsObject(GetPhysicsObject(&cube->GetTransform(), volume, CollisionLayer::LAYER_ONE, false, 0, DEF_ELASTICITY, DEF_LDAMPING, DEF_FRICTION));
	cube->GetPhysicsObject()->InitCubeInertia();
	cube->GetTransform().SetOrientation(Quaternion::EulerAnglesToQuaternion(0, rotation, 0));

	cube->SetRenderObject(nullptr);

	GameWorld::AddGameObject(cube);

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
			AddRenderPartToWorld(Vector3(position.x, position.y, position.z + i), Vector3(5, 5, 4), rotation, corridorWallStraight, corridorWallAlertTex, corridorWallNormal);
		}
		return physicalObject;
	}
	if (rotation == 180 || rotation == 0)
	{
		for (int i = -dimensions.x; i < dimensions.x; i += 10)
		{
			AddRenderPartToWorld(Vector3(position.x + i, position.y, position.z), Vector3(5, 5, 4), rotation, corridorWallStraight, corridorWallAlertTex, corridorWallNormal);
		}
		return physicalObject;
	}
	return physicalObject;
}

GameObject* LevelLoader::AddPaintWallToWorld(const Vector3& position, Vector3 dimensions, int rotation, string name)
{
	GameObject* cube = new GameObject(name, 12.0f);
	OBBVolume* volume = new OBBVolume(dimensions + Vector3(0, 10, -2));
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2)
		.SetOrientation(Quaternion::EulerAnglesToQuaternion(0, rotation, 0));

	if (rotation == 0)
		cube->GetTransform().SetOffset(Vector3(0, 15, 6.5f));
	if (rotation == 90)
		cube->GetTransform().SetOffset(Vector3(6.5f, 15, 0));
	if (rotation == 180)
		cube->GetTransform().SetOffset(Vector3(0, 15, -6.5f));
	if (rotation == 270)
		cube->GetTransform().SetOffset(Vector3(-6.5f, 15, 0));

#ifdef _WIN64
	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), corridorWallStraight, corridorWallAlertTex, basicShader , OGLTexture::RGBATextureEmpty(corridorWallAlertTex->GetHeight()/16, corridorWallAlertTex->GetWidth()/16), corridorWallNormal));
#else
	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), corridorWallStraight, corridorWallAlertTex, basicShader, PS4::PS4Texture::EmptyTex(corridorWallAlertTex->GetHeight() / 16, corridorWallAlertTex->GetWidth() / 16), corridorWallNormal));
#endif
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(0.0f);
	cube->GetPhysicsObject()->InitCubeInertia();

	cube->SetCollisionLayers(CollisionLayer::LAYER_ONE);
	cube->GetPhysicsObject()->SetDynamic(false);

	GameWorld::AddGameObject(cube);

	return cube;
}

void        LevelLoader::AddCornerWallToWorld(const Vector3& position, Vector3 dimensions, int rotation)
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

void		LevelLoader::AddSecurityCameraToWorld(const Vector3& position, int rotation)
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

void		LevelLoader::AddWallHammerToWorld(const Vector3& position, int rotation)
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

GameObject* LevelLoader::AddRenderPartToWorld(const Vector3& position, Vector3 dimensions, int rotation, MeshGeometry* mesh, TextureBase* texture, TextureBase* normal) {
	GameObject* cube = new GameObject();
	cube->SetBoundingVolume(nullptr);
	cube->SetPhysicsObject(nullptr);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2)
		.SetOrientation(Quaternion::EulerAnglesToQuaternion(0, rotation, 0));

#ifdef _WIN64
	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), mesh, texture, basicShader, OGLTexture::RGBATextureEmpty(texture->GetWidth()/16,texture->GetHeight()/16), normal));
#endif
#ifdef _ORBIS
	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), mesh, texture, basicShader, PS4::PS4Texture::EmptyTex(texture->GetWidth() / 16, texture->GetHeight() / 16), normal));
#endif

	GameWorld::AddGameObject(cube);
	return cube;
}

GameObject* LevelLoader::AddSphereToWorld(const Vector3& position, float radius, int layer, bool isTrigger, bool dynamic, bool rubber, bool hollow, float inverseMass) {
	GameObject* sphere = new GameObject();

	SetFieldsForSphere(sphere, position, (CollisionLayer) layer, radius, isTrigger, dynamic, rubber, hollow, inverseMass);

	GameWorld::AddGameObject(sphere);

	return sphere;
}

GameObject* LevelLoader::AddCubeToWorld(const Vector3& position, Maths::Vector3 dimensions, int layer, bool isTrigger, bool dynamic, bool OBB, float inverseMass) {
	GameObject* cube = new GameObject();

	SetFieldsForCube(cube, position, dimensions, (CollisionLayer) layer, isTrigger, dynamic, OBB, inverseMass);

	GameWorld::AddGameObject(cube);

	return cube;
}

GameObject* LevelLoader::AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, int layer, bool isTrigger, bool dynamic, float inverseMass) {
	GameObject* capsule = new GameObject();

	SetFieldsForCapsule(capsule, position, (CollisionLayer) layer, halfHeight, radius, isTrigger, dynamic, inverseMass);

	GameWorld::AddGameObject(capsule);

	return capsule;
}

GameObject* LevelLoader::AddPlayerObjectToWorld(const Vector3& position, GameObject* character) {
	float meshSize = 3.0f;

	CapsuleVolume* volume = new CapsuleVolume(0.85f * meshSize, 0.3f * meshSize);

	SetMiscFields(character, volume, position, Vector3(meshSize, meshSize, meshSize), false);
	character->SetPhysicsObject(GetPhysicsObject(&character->GetTransform(), volume, CollisionLayer::LAYER_ONE | CollisionLayer::LAYER_THREE, true, 20.0f, DEF_ELASTICITY, 3.0f, false));
	character->SetRenderObject(GetRenderObject(&character->GetTransform(), charMeshA, nullptr, basicShader, Vector4(0.5,1,0.5,1)));

	character->GetPhysicsObject()->InitSphereInertia();
	character->GetPhysicsObject()->SetCanSleep(false);
	character->GetPhysicsObject()->SetShouldApplyAngular(false);

	GameWorld::AddGameObject(character);

	return character;
}

PowerUp*    LevelLoader::AddPowerUpToWorld(const Vector3& position, const PowerUpType& ability, const float& radius) {
	PowerUp* powerup = nullptr;
	Vector4 colour;

	switch (ability) {
		case(PowerUpType::FireRate):
			powerup = new FireRate();
			colour = Debug::YELLOW;
			break;
		case(PowerUpType::Heal):
			powerup = new Heal();
			colour = Debug::RED;
			break;
		case(PowerUpType::SpeedBoost):
			powerup = new SpeedBoost();
			colour = Debug::CYAN;
			break;
		case(PowerUpType::MultiBullet):
			powerup = new MultiBullet();
			colour = Debug::GREEN;
			break;
	}

	SetFieldsForSphere(powerup, position, CollisionLayer::LAYER_FOUR, radius, true, true, false, false, 0);
	if (powerup) powerup->GetRenderObject()->SetColour(colour);
	
	GameWorld::AddGameObject(powerup);
	NetworkedGame::AddPowerUp(powerup);
	return powerup;
}

Projectile* LevelLoader::SpawnProjectile(Player* owner, const bool& NeedBulletSpread, const float& initialSpeed, const float& meshSize) {
	return SpawnProjectile((GameObject*)owner, NeedBulletSpread, owner->BulletCounter, owner->GetCam()->GetPitch(), owner->GetPlayerID(), initialSpeed, meshSize);
}

Projectile* LevelLoader::SpawnProjectile(GameObject* owner, const bool& NeedBulletSpread, const int bulletIndex, float pitch, int playerID, const float& initialSpeed, const float& meshSize)
{
  #ifndef _ORBIS
	AudioManager::GetInstance()->StartPlayingSound(Assets::AUDIODIR + "gun_fire.ogg", owner->GetTransform().GetPosition(), 0.3f);
#endif // !_ORBIS
  return singleton->AddProjectileToWorld(owner, NeedBulletSpread, bulletIndex, pitch, playerID, initialSpeed, meshSize);
}

Projectile* LevelLoader::AddProjectileToWorld(GameObject* owner, const bool& NeedBulletSpread, const int bulletIndex, float pitch, int playerID, const float& initialSpeed, const float& meshSize) {
  float inverseMass = 1.0f;

	const int SPREAD_ANGLE_CONST = 400;

	Vector3 ownerRot = owner->GetTransform().GetOrientation().ToEuler();

	Vector3 camForwardVector = (Matrix4::Rotation(ownerRot.y, Vector3(0,1,0)) * Matrix4::Rotation(pitch, Vector3(1,0,0)) * Vector3(0,0,-1)).Normalised();
	Vector3 camRightVector = Vector3::Cross(camForwardVector, Vector3(0, 1, 0)).Normalised();
	Vector3 camUpVector = Vector3::Cross(camForwardVector, -camRightVector).Normalised();

	Projectile* projectile = new Projectile(renderer, playerID);

	SphereVolume* volume = new SphereVolume(meshSize * 0.8);// / 2.0f * meshSize * 1.3f);
	projectile->SetBoundingVolume((CollisionVolume*)volume);

	projectile->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition((owner->GetTransform().GetPosition() + Vector3(0, 2.5f, 0)) + camForwardVector * 5.0f); //+ offset);

	projectile->SetRenderObject(new RenderObject(&projectile->GetTransform(), capsuleMesh, nullptr, basicShader));
	projectile->SetPhysicsObject(new PhysicsObject(&projectile->GetTransform(), projectile->GetBoundingVolume()));

	projectile->GetPhysicsObject()->SetInverseMass(inverseMass);
	projectile->GetPhysicsObject()->InitSphereInertia();

	float velocityDueToMovement = Vector3::Dot(camForwardVector, owner->GetPhysicsObject()->GetLinearVelocity());
	
	//#TODO
	//Few sets of fixed random angle for normal firing (based on bullet counter placed in player class)
	//Random spawn fine if multibullet powerup but increase spread
	float angle1 = 0.f, angle2=0.f;
	float angleArr[7] = { -1.f, 1.f, -0.2f, 0.3f, 0.7f, -0.8f, 1.5f};

	if (NeedBulletSpread) {
		angle1 = float((rand() % SPREAD_ANGLE_CONST) - SPREAD_ANGLE_CONST / 2) / (66.67f);
		angle2 = float((rand() % SPREAD_ANGLE_CONST) - SPREAD_ANGLE_CONST / 2) / (66.67f);
	}
	else {
		angle1 = angleArr[bulletIndex % 7];
		angle2 = angleArr[6 - (bulletIndex % 7)];
	}

	if (velocityDueToMovement < 0.0f) velocityDueToMovement = 0.0f;

	projectile->GetPhysicsObject()->AddAcceleration(Matrix4::Rotation(angle1, camUpVector) * Matrix4::Rotation(angle2, camRightVector)*camForwardVector * (initialSpeed + velocityDueToMovement));
	//projectile->GetPhysicsObject()->AddAcceleration(camForwardVector * (initialSpeed + velocityDueToMovement));
	
	projectile->GetTransform().SetOrientation(Quaternion(Matrix3::Rotation(-acos(Vector3::Dot(Vector3(0, 1, 0), camForwardVector)) * 180.0f / 3.14f, Vector3::Cross(camForwardVector, Vector3(0, 1, 0)).Normalised())));

	projectile->GetPhysicsObject()->SetLinearDamping(0.1f);
	projectile->GetPhysicsObject()->SetDynamic(true);
	projectile->SetCollisionLayers(CollisionLayer::LAYER_FIVE);

	GameWorld::AddGameObject(projectile);

	return projectile;
}

Vector3     LevelLoader::AddSpawnPointToWorld(const Vector3& position) {
	NetworkedGame::AddSpawnPoint(position);

	return position;
}

void LevelLoader::SetFieldsForSphere(GameObject* sphere, const Vector3& position, CollisionLayer layers, float radius, bool isTrigger, bool dynamic, bool rubber, bool hollow, float inverseMass, float elasticity, float lDamping, float friction) {
	CollisionVolume* volume = (CollisionVolume*) new SphereVolume(radius);
	
	SetMiscFields(sphere, volume, position, Vector3(radius, radius, radius), isTrigger);
	sphere->SetRenderObject(GetRenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(GetPhysicsObject(&sphere->GetTransform(), volume, layers, dynamic, inverseMass, rubber ? 0.9f : 0.2, lDamping, friction));

	if (hollow)	sphere->GetPhysicsObject()->InitHollowSphereInertia();
	else		sphere->GetPhysicsObject()->InitSphereInertia();
}

void LevelLoader::SetFieldsForCube(GameObject* cube, const Vector3& position, Vector3 dimensions, CollisionLayer layers, bool isTrigger, bool dynamic, bool OBB, float inverseMass, float elasticity, float lDamping, float friction) {
	CollisionVolume* volume = OBB ? (CollisionVolume*) new OBBVolume(dimensions) : (CollisionVolume*) new AABBVolume(dimensions);

	SetMiscFields(cube, volume, position, dimensions * 2, isTrigger);
	cube->SetPhysicsObject(GetPhysicsObject(&cube->GetTransform(), volume, layers, dynamic, inverseMass, elasticity, lDamping, friction));

	#ifdef _WIN64
		cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader, OGLTexture::RGBATextureEmpty(basicTex->GetWidth(), basicTex->GetHeight())));
	#elif _ORBIS
		cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader, PS4::PS4Texture::EmptyTex(basicTex->GetWidth(), basicTex->GetHeight())));
	#endif
	
	cube->GetPhysicsObject()->InitCubeInertia();
}

void LevelLoader::SetFieldsForCapsule(GameObject* capsule, const Vector3& position, CollisionLayer layers, float halfHeight, float radius, bool isTrigger, bool dynamic, float inverseMass, float elasticity, float lDamping, float friction) {
	CapsuleVolume* volume = new CapsuleVolume(halfHeight, radius);

	SetMiscFields(capsule, volume, position, Vector3(radius * 2, halfHeight, radius * 2), isTrigger);

	capsule->SetPhysicsObject(GetPhysicsObject(&capsule->GetTransform(), volume, layers, dynamic, inverseMass, elasticity, lDamping, friction));
	capsule->SetRenderObject(GetRenderObject(&capsule->GetTransform(), capsuleMesh, basicTex, basicShader));

	capsule->GetPhysicsObject()->InitCubeInertia();
}

void LevelLoader::SetMiscFields(GameObject* obj, CollisionVolume* volume, const Vector3& position, const Vector3& dimensions, bool isTrigger) {
	obj->SetBoundingVolume(volume);
	obj->SetTrigger(isTrigger);
	obj->GetTransform()
		.SetScale(dimensions)
		.SetPosition(position);
}

PhysicsObject* LevelLoader::GetPhysicsObject(Transform* transform, CollisionVolume* volume, int layers, bool dynamic, float inverseMass, float elasticity, float lDamping, float friction) {
	PhysicsObject* physics = new PhysicsObject(transform, volume);

	physics->SetCollisionLayers(layers);
	physics->SetInverseMass(inverseMass);
	physics->SetElasticity(elasticity);
	physics->SetLinearDamping(lDamping);
	physics->SetDynamic(dynamic);

	return physics;
}

RenderObject* LevelLoader::GetRenderObject(Transform* transform, MeshGeometry* mesh, TextureBase* texture, ShaderBase* shader, Vector4 colour) {
	RenderObject* renderer = new RenderObject(transform, mesh, texture, shader);

	renderer->SetColour(colour);

	return renderer;
}