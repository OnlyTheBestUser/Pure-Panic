#include "PhysicsSystem.h"
#include "PhysicsObject.h"
#include "GameObject.h"
#include "CollisionDetection.h"
#include "../../Common/Quaternion.h"
#include "Spring.h"
#include "Constraint.h"

#include "Debug.h"

#include <functional>
using namespace NCL;
using namespace CSC8503;

/*
These two variables help define the relationship between positions
and the forces that are added to objects to change those positions
*/

PhysicsSystem::PhysicsSystem(GameWorld& g) : gameWorld(g)	{
	applyGravity	= false;
	useBroadPhase	= true;	
	dTOffset		= 0.0f;
	globalDamping	= 0.995f;
	SetGravity(Vector3(0.0f, -19.6f, 0.0f));

	// Sets the valid different collision layers here
	/* Col Layer 1 = 1
	Col Layer 2 = 2
	Col Layer 3 = 4
	Col Layer 4 = 8
	Col Layer 5 = 16
	Col Layer 6 = 32 
	...so Col1 | Col3  = 5 */
	validLayers.emplace_back(Vector2(1, 1));
	validLayers.emplace_back(Vector2(1, 3));
	validLayers.emplace_back(Vector2(3, 1));
	validLayers.emplace_back(Vector2(1, 5));
	validLayers.emplace_back(Vector2(5, 1));
	validLayers.emplace_back(Vector2(3, 5));
	validLayers.emplace_back(Vector2(5, 3));
}

PhysicsSystem::~PhysicsSystem()	{
	delete staticTree;
}

void PhysicsSystem::SetGravity(const Vector3& g) {
	gravity = g;
}

void PhysicsSystem::BuildStaticList()
{
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;
	gameWorld.GetObjectIterators(first, last);
	staticTree = new Octree<GameObject*>(Vector3(1024, 1024, 1024), 7, 6);

	for (auto i = first; i != last; ++i) {
		(*i)->UpdateBroadphaseAABB(); // Force update
		Vector3 halfSizes;
		if (!(*i)->GetBroadphaseAABB(halfSizes) || (*i)->IsDynamic())
			continue;
		Vector3 pos = (*i)->GetTransform().GetPosition();
		staticTree->Insert(*i, pos, halfSizes, (*i)->GetName());
	}
}

/*

If the 'game' is ever reset, the PhysicsSystem must be
'cleared' to remove any old collisions that might still
be hanging around in the collision list. If your engine
is expanded to allow objects to be removed from the world,
you'll need to iterate through this collisions list to remove
any collisions they are in.

*/
void PhysicsSystem::Clear() {
	allCollisions.clear();
}

/*

This is the core of the physics engine update

*/
int constraintIterationCount = 10;

//This is the fixed timestep we'd LIKE to have
const int   idealHZ = 120;
const float idealDT = 1.0f / idealHZ;

/*
This is the fixed update we actually have...
If physics takes too long it starts to kill the framerate, it'll drop the 
iteration count down until the FPS stabilises, even if that ends up
being at a low rate. 
*/
int realHZ		= idealHZ;
float realDT	= idealDT;

void PhysicsSystem::Update(float dt) {	
	/*if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::B)) {
		useBroadPhase = !useBroadPhase;
		std::cout << "Setting broadphase to " << useBroadPhase << std::endl;
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::I)) {
		constraintIterationCount--;
		std::cout << "Setting constraint iterations to " << constraintIterationCount << std::endl;
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::O)) {
		constraintIterationCount++;
		std::cout << "Setting constraint iterations to " << constraintIterationCount << std::endl;
	}*/

	dTOffset += dt; //We accumulate time delta here - there might be remainders from previous frame!

	GameTimer t;
	t.GetTimeDeltaSeconds();

	UpdateObjectAABBs();

	while(dTOffset >= realDT) {
		std::vector<GameObject*>::const_iterator first;
		std::vector<GameObject*>::const_iterator last;
		gameWorld.GetObjectIterators(first, last);
		for (auto i = first; i != last; ++i) {
			PhysicsObject* object = (*i)->GetPhysicsObject();
			if (object == nullptr)
				continue;
			CheckToWake(object);
			IntegrateAccel(realDT, *i); //Update accelerations from external forces
		}

		BroadPhase();
		NarrowPhase();

		// TODO
		//This is our simple iterative solver - 
		//we just run things multiple times, slowly moving things forward
		//and then rechecking that the constraints have been met		
		float constraintDt = realDT /  (float)constraintIterationCount;
		for (int i = 0; i < constraintIterationCount; ++i) {
			UpdateConstraints(constraintDt);	
		}

		gameWorld.GetObjectIterators(first, last);
		for (auto i = first; i != last; ++i) {
			PhysicsObject* object = (*i)->GetPhysicsObject();
			if (object == nullptr)
				continue;
			CheckToSleep(object);
			Transform& transform = (*i)->GetTransform();
			IntegrateVelocity(realDT, object, transform); //update positions from new velocity changes
		}
		dTOffset -= realDT;
	}

	ClearForces();	//Once we've finished with the forces, reset them to zero
	//UpdateCollisionList(); //Remove any old collisions

	t.Tick();
	float updateTime = t.GetTimeDeltaSeconds();
	
	//Uh oh, physics is taking too long...
	if (updateTime > realDT) {
		realHZ /= 2;
		realDT *= 2;
		std::cout << "Dropping iteration count due to long physics time...(now " << realHZ << ")\n";
	}
	else if(dt*2 < realDT) { //we have plenty of room to increase iteration count!
		int temp = realHZ;
		realHZ *= 2;
		realDT /= 2;

		if (realHZ > idealHZ) {
			realHZ = idealHZ;
			realDT = idealDT;
		}
		if (temp != realHZ) {
			std::cout << "Raising iteration count due to short physics time...(now " << realHZ << ")\n";
		}
	}
}

/*
Later on we're going to need to keep track of collisions
across multiple frames, so we store them in a set.

The first time they are added, we tell the objects they are colliding.
The frame they are to be removed, we tell them they're no longer colliding.

From this simple mechanism, we we build up gameplay interactions inside the
OnCollisionBegin / OnCollisionEnd functions (removing health when hit by a 
rocket launcher, gaining a point when the player hits the gold coin, and so on).
*/
//void PhysicsSystem::UpdateCollisionList() {
//	for (std::set<CollisionDetection::CollisionInfo>::iterator i = allCollisions.begin(); i != allCollisions.end(); ) {
//		if (i->a == nullptr || i->b == nullptr) {
//			i = allCollisions.erase(i);
//			continue;
//		}
//		if ((*i).framesLeft == numCollisionFrames) {
//			i->a->OnCollisionBegin(i->b, i->point.localA, i->point.localB, i->point.normal);
//			i->b->OnCollisionBegin(i->a, i->point.localB, i->point.localA, -i->point.normal);
//		}
//		(*i).framesLeft = (*i).framesLeft - 1;
//		if ((*i).framesLeft < 0) {
//			i->a->OnCollisionEnd(i->b);
//			i->b->OnCollisionEnd(i->a);
//			i = allCollisions.erase(i);
//		}
//		else {
//			++i;
//		}
//	}
//}

void PhysicsSystem::UpdateObjectAABBs() {
	gameWorld.OperateOnContents(
		[](GameObject* g) {
			g->UpdateBroadphaseAABB();
		}
	);
}

/*

In tutorial 5, we start determining the correct response to a collision,
so that objects separate back out. 

*/
void PhysicsSystem::ImpulseResolveCollision(GameObject& a, GameObject& b, CollisionDetection::ContactPoint& p) const {
	PhysicsObject* physA = a.GetPhysicsObject();
	PhysicsObject* physB = b.GetPhysicsObject();

	Transform& transformA = a.GetTransform();
	Transform& transformB = b.GetTransform();

	float totalMass = physA->GetInverseMass() + physB->GetInverseMass();

	if (totalMass == 0)
		return; // two static obj

	// Separate using projection
	transformA.SetPosition(transformA.GetPosition() - (p.normal * p.penetration * (physA->GetInverseMass() / totalMass)));
	transformB.SetPosition(transformB.GetPosition() + (p.normal * p.penetration * (physB->GetInverseMass() / totalMass)));

	Vector3 relativeA = p.localA;
	Vector3 relativeB = p.localB;

	Vector3 angVelocityA = Vector3::Cross(physA->GetAngularVelocity(), relativeA);
	Vector3 angVelocityB = Vector3::Cross(physB->GetAngularVelocity(), relativeB);

	Vector3 fullVelocityA = physA->GetLinearVelocity() + angVelocityA;
	Vector3 fullVelocityB = physB->GetLinearVelocity() + angVelocityB;

	Vector3 contactVelocity = fullVelocityB - fullVelocityA;

	float impulseForce = Vector3::Dot(contactVelocity, p.normal);

	// now to work out the effect of inertia...
	Vector3 inertiaA = Vector3::Cross(physA->GetInertiaTensor() * Vector3::Cross(relativeA, p.normal), relativeA);
	Vector3 inertiaB = Vector3::Cross(physB->GetInertiaTensor() * Vector3::Cross(relativeB, p.normal), relativeB);
	float angularEffect = Vector3::Dot(inertiaA + inertiaB, p.normal);

	float cRestitution = (physA->GetElasticity() + physB->GetElasticity()) / 2.0f;

	float j = (-(1.0f + cRestitution) * impulseForce) / (totalMass + angularEffect);

	Vector3 fullImpulse = p.normal * j;

	float magnitude = fullImpulse.Length();
	
	physA->ApplyLinearImpulse(-fullImpulse);
	physB->ApplyLinearImpulse(fullImpulse);

	physA->ApplyAngularImpulse(Vector3::Cross(relativeA, -fullImpulse));
	physB->ApplyAngularImpulse(Vector3::Cross(relativeB, fullImpulse));

	if (physA->UsesFriction() && physB->UsesFriction()) {
		// Friction
		float mu = (physA->GetFriction() + physB->GetFriction()) / 2.0f;
		Vector3 t = contactVelocity - (p.normal * (Vector3::Dot(contactVelocity, p.normal)));
		inertiaA = Vector3::Cross(physA->GetInertiaTensor() * Vector3::Cross(relativeA, t), relativeA);
		inertiaB = Vector3::Cross(physB->GetInertiaTensor() * Vector3::Cross(relativeB, t), relativeB);
		angularEffect = Vector3::Dot(inertiaA + inertiaB, t);
		float frictionJ = -(Vector3::Dot(contactVelocity * mu, t)) / (totalMass + angularEffect);
		Vector3 frictionImpulse = t * frictionJ;

		physA->ApplyLinearImpulse(-frictionImpulse);
		physB->ApplyLinearImpulse(frictionImpulse);

		physA->ApplyAngularImpulse(Vector3::Cross(relativeA, -frictionImpulse));
		physB->ApplyAngularImpulse(Vector3::Cross(relativeB, frictionImpulse));
	}
}

void PhysicsSystem::ResolveSpringCollision(GameObject& a, GameObject& b, CollisionDetection::ContactPoint& p) const
{
	PhysicsObject* physA = a.GetPhysicsObject();
	PhysicsObject* physB = b.GetPhysicsObject();

	Vector3 relativeVel = physA->GetLinearVelocity() - physB->GetLinearVelocity();

	Spring s(0.0f, 0.2f);
	s.SetDamping(0.8f);

	Vector3 force = p.normal * -(s.GetK() * (p.penetration - s.GetLength()));
	force *= s.GetD();
	//force -= p.normal * s.GetD() * (Vector3::Dot(relativeVel, p.normal));

	physA->ApplyLinearImpulse(force);
	physB->ApplyLinearImpulse(-force);

	//physA->AddForce(force);
	//physB->AddForce(-force);
	//Debug::DrawLine(b.GetTransform().GetPosition(), b.GetTransform().GetPosition() + (force.Normalised() * 10), Debug::RED);
	//Debug::DrawLine(b.GetTransform().GetPosition(), b.GetTransform().GetPosition() + Vector3(10,0,0), Debug::BLUE);
	//Debug::DrawLine(a.GetTransform().GetPosition(), a.GetTransform().GetPosition() + Vector3(20,0,0), Debug::GREEN);
}

/*
Later, we replace the BasicCollisionDetection method with a broadphase
and a narrowphase collision detection method. In the broad phase, we
split the world up using an acceleration structure, so that we can only
compare the collisions that we absolutely need to. 
*/
void PhysicsSystem::BroadPhase() {
	broadphaseCollisions.clear();
	Octree<GameObject*> tree(Vector3(1024, 1024, 1024), 7, 6);
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;
	gameWorld.GetObjectIterators(first, last);

	for (auto i = first; i != last; ++i) {
		Vector3 halfSizes;
		if (!(*i)->GetBroadphaseAABB(halfSizes) || !(*i)->IsDynamic()) {
				continue;
		}

		Vector3 pos = (*i)->GetTransform().GetPosition();
		tree.Insert(*i, pos, halfSizes);
	}

	// Test dynamic against static
	std::list<OctreeEntry<GameObject*>> list;
	tree.OperateOnContents([&](std::list<OctreeEntry<GameObject*>>& data) {
		CollisionDetection::CollisionInfo info;
		for (auto i = data.begin(); i != data.end(); ++i) {
			list.clear();
			//if(i->object->GetName() == "player")
			staticTree->GetContentsAtNode(i->object, i->pos, i->size, list);
			for (auto j = list.begin(); j != list.end(); j++) {
				info.a = std::min((*i).object, (*j).object);
				info.b = std::max((*i).object, (*j).object);
				if (ValidCollisionLayers(info.a->GetCollisionLayers(), info.b->GetCollisionLayers())) {
					broadphaseCollisions.insert(info);
				}
			}
		}
		});
	// Test all dynamic objects against eachother
	tree.OperateOnContents([&](std::list<OctreeEntry<GameObject*>>& data) {
		CollisionDetection::CollisionInfo info;
		for (auto i = data.begin(); i != data.end(); ++i) {
			for (auto j = std::next(i); j != data.end(); ++j) {
				//is this pair of items already in the collision set - 
				// if the same pair is in another Octree node together etc
#ifdef _WIN64
				info.a = std::min((*i).object, (*j).object);
				info.b = std::max((*i).object, (*j).object);
#else // _WIN64
				info.a = std::min((*i).object, (*j).object);
				info.b = std::max((*i).object, (*j).object);
#endif
				if (ValidCollisionLayers(info.a->GetCollisionLayers(), info.b->GetCollisionLayers()) && !(!info.a->IsDynamic() && !info.b->IsDynamic())) {
					broadphaseCollisions.insert(info);
				}
			}
		}
	});
}

/*
The broadphase will now only give us likely collisions, so we can now go through them,
and work out if they are truly colliding, and if so, add them into the main collision list
*/
void PhysicsSystem::NarrowPhase() {
	for (std::set<CollisionDetection::CollisionInfo>::iterator i = broadphaseCollisions.begin(); i != broadphaseCollisions.end(); ++i) {
		CollisionDetection::CollisionInfo info = *i;
		if (CollisionDetection::ObjectIntersection(info.a, info.b, info)) {
			info.framesLeft = numCollisionFrames;
			if (!(info.a->IsTrigger() || info.b->IsTrigger())) {
				if(info.a->GetPhysicsObject()->UseSpringRes() || info.b->GetPhysicsObject()->UseSpringRes())
					ResolveSpringCollision(*info.a, *info.b, info.point);
				else
					ImpulseResolveCollision(*info.a, *info.b, info.point);	
			}

			info.a->OnCollisionBegin(info.b, info.point.localA, info.point.localB, info.point.normal);
			info.b->OnCollisionBegin(info.a, info.point.localB, info.point.localA, -info.point.normal);

			allCollisions.insert(info); // insert into our main set
		}
	}
}

void PhysicsSystem::CheckToWake(PhysicsObject* object)
{
	const float threshold = 0.01;
	// Wake objects if new forces have been applied above a threshold
	if ((object->GetLinearVelocity().Length() > threshold || object->GetForce().Length() > threshold
		|| object->GetLinearVelocity().Length() < -threshold || object->GetForce().Length() < -threshold))
		object->Wake();
}

void PhysicsSystem::CheckToSleep(PhysicsObject* object)
{
	const int maxQueueSize = 6;
	const float bounceTolerance = 0.5;

	// for all objects in world, update their queue of previous velocities dot products and previous positions
	// only ever want a queue of set length, so if gonna be longer remove the oldest and add latest
	if (object->GetPrevVelocitiesSize() >= maxQueueSize)
		object->RemoveFromPreviousVelocities();
	object->AddToPreviousVelocities(Vector3::Dot(object->GetLinearVelocity(), Vector3(1, 1, 1)));

	if (object->GetPreviousPositionsSize() >= maxQueueSize)
		object->RemoveFromPreviousPositions();
	object->AddToPreviousPositions(object->GetTransform()->GetPosition().Length());

	// Check if 2 consecutive values in the queue are of the same sign, if so continue, if not sleep object as its bouncing permanently on the floor
	if (object->GetPrevVelocitiesSize() == maxQueueSize)
	{
		std::queue<float> velQueue = object->GetPrevVelocities();
		bool velShouldSleep = true;
		float velQueuePrev = velQueue.front();
		float velAverage = velQueuePrev;
		velQueue.pop();

		std::queue<float> posQueue = object->GetPreviousPositions();
		bool posShouldSleep = true;
		float firstPos = posQueue.front();
		float lastPos = 0.0;
		float posQueuePrev = firstPos;
		posQueue.pop();

		// Queues are the same length
		while (!velQueue.empty())
		{
			float velQueueNext = velQueue.front();
			velAverage += velQueueNext;
			velQueue.pop();

			float posQueueNext = posQueue.front();
			lastPos = posQueueNext;
			posQueue.pop();

			// Check if values are same sign or it is gonna start moving from rest, set sleeping to false
			// Velocity checks for AABB and Sphere
			if ((object->GetVolumeType() == VolumeType::AABB || object->GetVolumeType() == VolumeType::Sphere) &&
				((velQueuePrev / abs(velQueuePrev)) == (velQueueNext / abs(velQueueNext))
				|| (velQueuePrev == 0.0) && ((velQueueNext / abs(velQueueNext)) != 0.0)
				|| ((velQueuePrev / abs(velQueuePrev)) != 0.0) && (velQueueNext == 0.0)))
				velShouldSleep = false;

			// Position checks for all types, * 1000 to get rid of the e to power of as that messes up the checks
			if (abs(posQueueNext - posQueuePrev) * 1000 > bounceTolerance)
				posShouldSleep = false;

			velQueuePrev = velQueueNext;
			posQueuePrev = posQueueNext;
		}
		
		// Velocity checks for OBB and Capsule
		if ((object->GetVolumeType() == VolumeType::OBB || object->GetVolumeType() == VolumeType::Capsule) && (abs(velAverage / 5) > 0.1))
			velShouldSleep = false;
			
		// if object should sleep remove all velocity and set to sleep
		if (abs(lastPos - firstPos) * 1000 < bounceTolerance && posShouldSleep && velShouldSleep)
		{
			object->SetLinearVelocity(Vector3(0, 0, 0));
			object->Sleep();
		}
	}	
}

/*
Integration of acceleration and velocity is split up, so that we can
move objects multiple times during the course of a PhysicsUpdate,
without worrying about repeated forces accumulating etc. 

This function will update both linear and angular acceleration,
based on any forces that have been accumulated in the objects during
the course of the previous game frame.
*/
void PhysicsSystem::IntegrateAccel(float dt, GameObject* gobj) {
	PhysicsObject* object = gobj->GetPhysicsObject();
	if (object->isSleeping())
		return;

	float inverseMass = object->GetInverseMass();

	Vector3 linearVel = object->GetLinearVelocity();
	Vector3 force = object->GetForce();
	Vector3 accel = force * inverseMass;
	
	if (applyGravity && inverseMass > 0 && object->UsesGravity() && gobj->IsDynamic()) {
		accel += gravity;
	}
	
	linearVel += accel * dt;
	object->SetLinearVelocity(linearVel);

	// Angular Stuff
	if (object->ShouldApplyAngular())
	{
		Vector3 torque = object->GetTorque();
		Vector3 angVel = object->GetAngularVelocity();

		object->UpdateInertiaTensor();

		Vector3 angAccel = object->GetInertiaTensor() * torque;
		angVel += angAccel * dt;
		object->SetAngularVelocity(angVel);
	}
}

/*
This function integrates linear and angular velocity into
position and orientation. It may be called multiple times
throughout a physics update, to slowly move the objects through
the world, looking for collisions.
*/
void PhysicsSystem::IntegrateVelocity(float dt, PhysicsObject* object, Transform& transform) {
	
	float frameLinearDamping = 1.0f - (object->GetLinearDamping() * dt);

	if (object->isSleeping())
		return;

	// Position stuff
	Vector3 position = transform.GetPosition();
	Vector3 linearVel = object->GetLinearVelocity();
	position += linearVel * dt;
	transform.SetPosition(position);
	// Linear Damping
	linearVel = linearVel * frameLinearDamping;
	object->SetLinearVelocity(linearVel);

	// Orientation Stuff
	if (object->ShouldApplyAngular())
	{
		Quaternion orientation = transform.GetOrientation();
		Vector3 angVel = object->GetAngularVelocity();

		orientation = orientation + (Quaternion(angVel * dt * 0.5f, 0.0f) * orientation);
		orientation.Normalise();

		transform.SetOrientation(orientation);

		// Damp the angular velocity too
		float frameAngularDamping = 1.0f - (0.4f * dt);
		angVel = angVel * frameAngularDamping;
		object->SetAngularVelocity(angVel);
	}
}

/*
Once we're finished with a physics update, we have to
clear out any accumulated forces, ready to receive new
ones in the next 'game' frame.
*/
void PhysicsSystem::ClearForces() {
	gameWorld.OperateOnContents(
		[](GameObject* o) {
			if (o->GetPhysicsObject() != nullptr)
				o->GetPhysicsObject()->ClearForces();
		}
	);
}

/*
As part of the final physics tutorials, we add in the ability
to constrain objects based on some extra calculation, allowing
us to model springs and ropes etc. 
*/
void PhysicsSystem::UpdateConstraints(float dt) {
	std::vector<Constraint*>::const_iterator first;
	std::vector<Constraint*>::const_iterator last;
	gameWorld.GetConstraintIterators(first, last);

	for (auto i = first; i != last; ++i) {
		(*i)->UpdateConstraint(dt);
	}
}

bool PhysicsSystem::ValidCollisionLayers(int aLayers, int bLayers)
{
	for (auto v : validLayers)
	{
		if (aLayers == v.x && bLayers == v.y)
			return true;
	}
	return false;
}