#include "CollisionDetection.h"
#include "CollisionVolume.h"
#include "AABBVolume.h"
#include "OBBVolume.h"
#include "SphereVolume.h"
#include "../../Common/Vector2.h"
#include "../../Common/Window.h"
#include "../../Common/Maths.h"
#include "Debug.h"

#include <list>

using namespace NCL;

bool CollisionDetection::RayPlaneIntersection(const Ray&r, const Plane&p, RayCollision& collisions) {
	float ln = Vector3::Dot(p.GetNormal(), r.GetDirection());

	if (ln == 0.0f) {
		return false; //direction vectors are perpendicular!
	}
	
	Vector3 planePoint = p.GetPointOnPlane();

	Vector3 pointDir = planePoint - r.GetPosition();

	float d = Vector3::Dot(pointDir, p.GetNormal()) / ln;

	collisions.collidedAt = r.GetPosition() + (r.GetDirection() * d);

	return true;
}

bool CollisionDetection::RayIntersection(const Ray& r,GameObject& object, RayCollision& collision) {
	bool hasCollided = false;

	const Transform& worldTransform = object.GetTransform();
	const CollisionVolume* volume	= object.GetBoundingVolume();

	if (!volume) {
		return false;
	}

	switch (volume->type) {
		case VolumeType::AABB:		hasCollided = RayAABBIntersection(r, worldTransform, (const AABBVolume&)*volume	, collision); break;
		case VolumeType::OBB:		hasCollided = RayOBBIntersection(r, worldTransform, (const OBBVolume&)*volume	, collision); break;
		case VolumeType::Sphere:	hasCollided = RaySphereIntersection(r, worldTransform, (const SphereVolume&)*volume	, collision); break;
		case VolumeType::Capsule:	hasCollided = RayCapsuleIntersection(r, worldTransform, (const CapsuleVolume&)*volume, collision); break;
	}

	return hasCollided;
}

bool CollisionDetection::RayBoxIntersection(const Ray&r, const Vector3& boxPos, const Vector3& boxSize, RayCollision& collision) {
	Vector3 boxMin = boxPos - boxSize;
	Vector3 boxMax = boxPos + boxSize;

	Vector3 rayPos = r.GetPosition();
	Vector3 rayDir = r.GetDirection();

	Vector3 tVals(-1, -1, -1);

	for (int i = 0; i < 3; ++i) { // get best 3 intersection
		if (rayDir[i] > 0) {
			tVals[i] = (boxMin[i] - rayPos[i]) / rayDir[i];
		}
		else {
			tVals[i] = (boxMax[i] - rayPos[i]) / rayDir[i];
		}
	}
	
	float bestT = tVals.GetMaxElement();
	if (bestT < 0.0f) {
		return false;
	}

	Vector3 intersection = rayPos + (rayDir * bestT);
	const float epsilon = 0.0001f;
	for (int i = 0; i < 3; i++) {
		if (intersection[i] + epsilon < boxMin[i] || intersection[i] - epsilon > boxMax[i]) {
			return false;
		}
	}
	collision.collidedAt = intersection;
	collision.rayDistance = bestT;
	return true;
}

bool CollisionDetection::RayAABBIntersection(const Ray&r, const Transform& worldTransform, const AABBVolume& volume, RayCollision& collision) {
	Vector3 boxPos = worldTransform.GetPosition();
	Vector3 boxSize = volume.GetHalfDimensions();
	return RayBoxIntersection(r, boxPos, boxSize, collision);
}

bool CollisionDetection::RayOBBIntersection(const Ray&r, const Transform& worldTransform, const OBBVolume& volume, RayCollision& collision) {
	Quaternion orientation = worldTransform.GetOrientation();
	Vector3 position = worldTransform.GetPosition();

	Matrix3 transform = Matrix3(orientation);
	Matrix3 invTransform = Matrix3(orientation.Conjugate());

	Vector3 localRayPos = r.GetPosition() - position;

	Ray tempRay(invTransform * localRayPos, invTransform * r.GetDirection());

	bool collided = RayBoxIntersection(tempRay, Vector3(), volume.GetHalfDimensions(), collision);

	if (collided) {
		collision.collidedAt = transform * collision.collidedAt + position;
	}
	return collided;
}

bool CollisionDetection::RayCapsuleIntersection(const Ray& r, const Transform& worldTransform, const CapsuleVolume& volume, RayCollision& collision) {
	// Generate plane
	Vector3 pointA = worldTransform.GetPosition() + (worldTransform.GetOrientation() * (Vector3(0, 1, 0) * (volume.GetHalfHeight() - volume.GetRadius())));
	Vector3 pointB = worldTransform.GetPosition() - (worldTransform.GetOrientation() * (Vector3(0, 1, 0) * (volume.GetHalfHeight() - volume.GetRadius())));

	Vector3 normal = r.GetPosition() - worldTransform.GetPosition();
	Vector3 cross = Vector3::Cross(pointA - pointB, normal);
	Vector3 pointC = worldTransform.GetPosition() + (cross.Normalised() * 5.0f);

	Plane p = Plane::PlaneFromTri(pointA, pointB, pointC);

	bool hit = RayPlaneIntersection(r, p, collision);

	if (!hit)
		return false;

	Vector3 capsuleDir = pointA - pointB;
	float capsuleLineLength = capsuleDir.Length();
	capsuleDir.Normalise();

	Vector3 rayCapDir = collision.collidedAt - pointB;
	float dot = Maths::Clamp(Vector3::Dot(rayCapDir, capsuleDir), 0.0f, capsuleLineLength);
	
	// Create sphere at point and check ray / sphere
	Vector3 spherePos = pointB + (capsuleDir * dot);
	SphereVolume s(volume.GetRadius());
	Transform t;
	t.SetPosition(spherePos);
	t.SetScale(Vector3(volume.GetRadius(), volume.GetRadius(), volume.GetRadius()));

	bool sphereCheck = RaySphereIntersection(r, t, s, collision);
	return sphereCheck;
}

bool CollisionDetection::RaySphereIntersection(const Ray&r, const Transform& worldTransform, const SphereVolume& volume, RayCollision& collision) {
	Vector3 spherePos = worldTransform.GetPosition();
	float sphereRadius = volume.GetRadius();

	//Get the direction between the ray origin and the sphere origin
	Vector3 dir = spherePos - r.GetPosition();

	//Then project the sphere's origin onto our ray direction vector
	float sphereProj = Vector3::Dot(dir, r.GetDirection());

	if (sphereProj < 0.0f)
		return false;

	//Get closest point on ray line to sphere
	Vector3 point = r.GetPosition() + (r.GetDirection() * sphereProj);

	float sphereDist = (point - spherePos).Length();

	if (sphereDist > sphereRadius)
		return false;

	float offset = sqrt((sphereRadius * sphereRadius) - (sphereDist * sphereDist));

	collision.rayDistance = sphereProj - offset;
	collision.collidedAt = r.GetPosition() + (r.GetDirection() * collision.rayDistance);
	return true;
}

Matrix4 GenerateInverseView(const Camera &c) {
	float pitch = c.GetPitch();
	float yaw	= c.GetYaw();
	Vector3 position = c.GetPosition();

	Matrix4 iview =
		Matrix4::Translation(position) *
		Matrix4::Rotation(-yaw, Vector3(0, -1, 0)) *
		Matrix4::Rotation(-pitch, Vector3(-1, 0, 0));

	return iview;
}

Vector3 CollisionDetection::Unproject(const Vector3& screenPos, const Camera& cam) {
	Vector2 screenSize = Window::GetWindow()->GetScreenSize();

	float aspect	= screenSize.x / screenSize.y;
	float fov		= cam.GetFieldOfVision();
	float nearPlane = cam.GetNearPlane();
	float farPlane  = cam.GetFarPlane();

	//Create our inverted matrix! Note how that to get a correct inverse matrix,
	//the order of matrices used to form it are inverted, too.
	Matrix4 invVP = GenerateInverseView(cam) * GenerateInverseProjection(aspect, fov, nearPlane, farPlane);

	//Our mouse position x and y values are in 0 to screen dimensions range,
	//so we need to turn them into the -1 to 1 axis range of clip space.
	//We can do that by dividing the mouse values by the width and height of the
	//screen (giving us a range of 0.0 to 1.0), multiplying by 2 (0.0 to 2.0)
	//and then subtracting 1 (-1.0 to 1.0).
	Vector4 clipSpace = Vector4(
		(screenPos.x / (float)screenSize.x) * 2.0f - 1.0f,
		(screenPos.y / (float)screenSize.y) * 2.0f - 1.0f,
		(screenPos.z),
		1.0f
	);

	//Then, we multiply our clipspace coordinate by our inverted matrix
	Vector4 transformed = invVP * clipSpace;

	//our transformed w coordinate is now the 'inverse' perspective divide, so
	//we can reconstruct the final world space by dividing x,y,and z by w.
	return Vector3(transformed.x / transformed.w, transformed.y / transformed.w, transformed.z / transformed.w);
}

Ray CollisionDetection::BuildRayFromMouse(const Camera& cam) {
	Vector2 screenMouse = Window::GetMouse()->GetAbsolutePosition();
	Vector2 screenSize	= Window::GetWindow()->GetScreenSize();

	//We remove the y axis mouse position from height as OpenGL is 'upside down',
	//and thinks the bottom left is the origin, instead of the top left!
	Vector3 nearPos = Vector3(screenMouse.x,
		screenSize.y - screenMouse.y,
		-0.99999f
	);

	//We also don't use exactly 1.0 (the normalised 'end' of the far plane) as this
	//causes the unproject function to go a bit weird. 
	Vector3 farPos = Vector3(screenMouse.x,
		screenSize.y - screenMouse.y,
		0.99999f
	);

	Vector3 a = Unproject(nearPos, cam);
	Vector3 b = Unproject(farPos, cam);
	Vector3 c = b - a;

	c.Normalise();

	//std::cout << "Ray Direction:" << c << std::endl;

	return Ray(cam.GetPosition(), c);
}

//http://bookofhook.com/mousepick.pdf
Matrix4 CollisionDetection::GenerateInverseProjection(float aspect, float fov, float nearPlane, float farPlane) {
	Matrix4 m;

	float t = tan(fov*PI_OVER_360);

	float neg_depth = nearPlane - farPlane;

	const float h = 1.0f / t;

	float c = (farPlane + nearPlane) / neg_depth;
	float e = -1.0f;
	float d = 2.0f*(nearPlane*farPlane) / neg_depth;

	m.array[0]  = aspect / h;
	m.array[5]  = tan(fov*PI_OVER_360);

	m.array[10] = 0.0f;
	m.array[11] = 1.0f / d;

	m.array[14] = 1.0f / e;

	m.array[15] = -c / (d*e);

	return m;
}

/*
And here's how we generate an inverse view matrix. It's pretty much
an exact inversion of the BuildViewMatrix function of the Camera class!
*/
Matrix4 CollisionDetection::GenerateInverseView(const Camera &c) {
	float pitch = c.GetPitch();
	float yaw	= c.GetYaw();
	Vector3 position = c.GetPosition();

	Matrix4 iview =
Matrix4::Translation(position) *
Matrix4::Rotation(yaw, Vector3(0, 1, 0)) *
Matrix4::Rotation(pitch, Vector3(1, 0, 0));

return iview;
}


/*
If you've read through the Deferred Rendering tutorial you should have a pretty
good idea what this function does. It takes a 2D position, such as the mouse
position, and 'unprojects' it, to generate a 3D world space position for it.

Just as we turn a world space position into a clip space position by multiplying
it by the model, view, and projection matrices, we can turn a clip space
position back to a 3D position by multiply it by the INVERSE of the
view projection matrix (the model matrix has already been assumed to have
'transformed' the 2D point). As has been mentioned a few times, inverting a
matrix is not a nice operation, either to understand or code. But! We can cheat
the inversion process again, just like we do when we create a view matrix using
the camera.

So, to form the inverted matrix, we need the aspect and fov used to create the
projection matrix of our scene, and the camera used to form the view matrix.

*/
Vector3	CollisionDetection::UnprojectScreenPosition(Vector3 position, float aspect, float fov, const Camera &c) {
	//Create our inverted matrix! Note how that to get a correct inverse matrix,
	//the order of matrices used to form it are inverted, too.
	Matrix4 invVP = GenerateInverseView(c) * GenerateInverseProjection(aspect, fov, c.GetNearPlane(), c.GetFarPlane());

	Vector2 screenSize = Window::GetWindow()->GetScreenSize();

	//Our mouse position x and y values are in 0 to screen dimensions range,
	//so we need to turn them into the -1 to 1 axis range of clip space.
	//We can do that by dividing the mouse values by the width and height of the
	//screen (giving us a range of 0.0 to 1.0), multiplying by 2 (0.0 to 2.0)
	//and then subtracting 1 (-1.0 to 1.0).
	Vector4 clipSpace = Vector4(
		(position.x / (float)screenSize.x) * 2.0f - 1.0f,
		(position.y / (float)screenSize.y) * 2.0f - 1.0f,
		(position.z) - 1.0f,
		1.0f
	);

	//Then, we multiply our clipspace coordinate by our inverted matrix
	Vector4 transformed = invVP * clipSpace;

	//our transformed w coordinate is now the 'inverse' perspective divide, so
	//we can reconstruct the final world space by dividing x,y,and z by w.
	return Vector3(transformed.x / transformed.w, transformed.y / transformed.w, transformed.z / transformed.w);
}

bool CollisionDetection::ObjectIntersection(GameObject* a, GameObject* b, CollisionInfo& collisionInfo) {
	const CollisionVolume* volA = a->GetBoundingVolume();
	const CollisionVolume* volB = b->GetBoundingVolume();

	if (!volA || !volB) {
		return false;
	}

	collisionInfo.a = a;
	collisionInfo.b = b;

	Transform& transformA = a->GetTransform();
	Transform& transformB = b->GetTransform();

	VolumeType pairType = (VolumeType)((int)volA->type | (int)volB->type);

	if (pairType == VolumeType::AABB) {
		return AABBIntersection((AABBVolume&)*volA, transformA, (AABBVolume&)*volB, transformB, collisionInfo);
	}

	if (pairType == VolumeType::Sphere) {
		return SphereIntersection((SphereVolume&)*volA, transformA, (SphereVolume&)*volB, transformB, collisionInfo);
	}

	if (pairType == VolumeType::OBB) {
		return OBBIntersection((OBBVolume&)*volA, transformA, (OBBVolume&)*volB, transformB, collisionInfo);
	}

	if (volA->type == VolumeType::AABB && volB->type == VolumeType::Sphere) {
		return AABBSphereIntersection((AABBVolume&)*volA, transformA, (SphereVolume&)*volB, transformB, collisionInfo);
	}
	if (volA->type == VolumeType::Sphere && volB->type == VolumeType::AABB) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return AABBSphereIntersection((AABBVolume&)*volB, transformB, (SphereVolume&)*volA, transformA, collisionInfo);
	}

	if (volA->type == VolumeType::OBB && volB->type == VolumeType::Sphere) {
		return SphereOBBIntersection((SphereVolume&)* volB, transformB, (OBBVolume&) * volA, transformA, collisionInfo);
	}
	if (volA->type == VolumeType::Sphere && volB->type == VolumeType::OBB) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return SphereOBBIntersection((SphereVolume&)* volA, transformA, (OBBVolume&) * volB, transformB, collisionInfo);
	}


	if (volA->type == VolumeType::Capsule && volB->type == VolumeType::Sphere) {
		return SphereCapsuleIntersection((CapsuleVolume&)*volA, transformA, (SphereVolume&)*volB, transformB, collisionInfo);
	}
	if (volA->type == VolumeType::Sphere && volB->type == VolumeType::Capsule) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return SphereCapsuleIntersection((CapsuleVolume&)*volB, transformB, (SphereVolume&)*volA, transformA, collisionInfo);
	}

	if (volA->type == VolumeType::AABB && volB->type == VolumeType::Capsule) {
		return AABBCapsuleIntersection((AABBVolume&)* volA, transformA, (CapsuleVolume&)* volB, transformB, collisionInfo);
	}
	if (volA->type == VolumeType::Capsule && volB->type == VolumeType::AABB) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return AABBCapsuleIntersection((AABBVolume&)* volB, transformB, (CapsuleVolume&)* volA, transformA, collisionInfo);
	}

	if (volA->type == VolumeType::OBB && volB->type == VolumeType::Capsule) {

		return OBBCapsuleIntersection((OBBVolume&)* volA, transformA, (CapsuleVolume&)* volB, transformB, collisionInfo);
	}
	if (volA->type == VolumeType::Capsule && volB->type == VolumeType::OBB) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return OBBCapsuleIntersection((OBBVolume&)* volB, transformB, (CapsuleVolume&)* volA, transformA, collisionInfo);
	}

	if (volA->type == VolumeType::Capsule && volB->type == VolumeType::Capsule) {
		return CapsuleIntersection((CapsuleVolume&)*volA, transformA, (CapsuleVolume&)*volB, transformB, collisionInfo);
	}

	if ((volA->type == VolumeType::AABB && volB->type == VolumeType::OBB) || (volA->type == VolumeType::OBB && volB->type == VolumeType::AABB)) {
		return OBBIntersection((OBBVolume&)* volA, transformA, (OBBVolume&)* volB, transformB, collisionInfo);
	}

	return false;
}

bool CollisionDetection::AABBTest(const Vector3& posA, const Vector3& posB, const Vector3& halfSizeA, const Vector3& halfSizeB) {
	Vector3 delta = posB - posA;
	Vector3 totalSize = halfSizeA + halfSizeB;

	if (abs(delta.x) < totalSize.x && abs(delta.y) < totalSize.y && abs(delta.z) < totalSize.z)
		return true;

	return false;
}

//AABB/AABB Collisions
bool CollisionDetection::AABBIntersection(const AABBVolume& volumeA, const Transform& worldTransformA,
	const AABBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	Vector3 boxAPos = worldTransformA.GetPosition();
	Vector3 boxBPos = worldTransformB.GetPosition();

	Vector3 boxASize = volumeA.GetHalfDimensions();
	Vector3 boxBSize = volumeB.GetHalfDimensions();

	bool overlap = AABBTest(boxAPos, boxBPos, boxASize, boxBSize);
	
	if (overlap) {
		static const Vector3 faces[6] = {
			Vector3(-1,0,0),Vector3(1,0,0),
			Vector3(0,-1,0),Vector3(0,1,0),
			Vector3(0,0,-1),Vector3(0,0,1)
		};

		Vector3 maxA = boxAPos + boxASize;
		Vector3 minA = boxAPos - boxASize;

		Vector3 maxB = boxBPos + boxBSize;
		Vector3 minB = boxBPos - boxBSize;

		float distances[6] = {
			(maxB.x - minA.x), // Distance from box 'b' to the left of 'a'.
			(maxA.x - minB.x), // Distance from box 'b' to the right of 'a'.
			(maxB.y - minA.y), // Distance from box 'b' to the bottom of 'a'.
			(maxA.y - minB.y), // Distance from box 'b' to the top of 'a'.
			(maxB.z - minA.z), // Distance from box 'b' to the far of 'a'.
			(maxA.z - minB.z) // Distance from box 'b' to the near of 'a'.
		};
		float penetration = FLT_MAX;
		Vector3 bestAxis;

		for (int i = 0; i < 6; ++i) {
			if (distances[i] < penetration) {
				penetration = distances[i];
				bestAxis = faces[i];
			}
		}
		collisionInfo.AddContactPoint(Vector3(), Vector3(), bestAxis, penetration);
		return true;
	}

	return false;
}

//Sphere / Sphere Collision
bool CollisionDetection::SphereIntersection(const SphereVolume& volumeA, const Transform& worldTransformA,
	const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	float radii = volumeA.GetRadius() + volumeB.GetRadius();
	Vector3 delta = worldTransformB.GetPosition() - worldTransformA.GetPosition();

	float deltaLength = delta.Length();

	if (deltaLength < radii) {
		float penetration = (radii - deltaLength);
		Vector3 normal = delta.Normalised();
		Vector3 localA = normal * volumeA.GetRadius();
		Vector3 localB = -normal * volumeB.GetRadius();

		collisionInfo.AddContactPoint(localA, localB, normal, penetration);
		return true; // colliding
	}
	return false;
}

//AABB - Sphere Collision
bool CollisionDetection::AABBSphereIntersection(const AABBVolume& volumeA, const Transform& worldTransformA,
	const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	Vector3 boxSize = volumeA.GetHalfDimensions();
	
	Vector3 delta = worldTransformB.GetPosition() - worldTransformA.GetPosition();

	Vector3 closestPointOnBox = Maths::Clamp(delta, -boxSize, boxSize);
	Vector3 localPoint = delta - closestPointOnBox;

	float distance = localPoint.Length();

	if (distance < volumeB.GetRadius()) { // Colliding
		Vector3 collisionNormal = localPoint.Normalised();
		float penetration = (volumeB.GetRadius() - distance);

		Vector3 localA = closestPointOnBox;
		Vector3 localB = -collisionNormal * volumeB.GetRadius();

		collisionInfo.AddContactPoint(localA, localB, collisionNormal, penetration);
		return true;
	}
	return false;
}

bool CollisionDetection::OBBIntersection(
	const OBBVolume& volumeA, const Transform& worldTransformA,
	const OBBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {

	Vector3 directions[15]{

		// A XYZ
		worldTransformA.GetOrientation() * Vector3(1, 0, 0),
		worldTransformA.GetOrientation() * Vector3(0, 1, 0),
		worldTransformA.GetOrientation() * Vector3(0, 0, 1),

		// B XYZ
		worldTransformB.GetOrientation()* Vector3(1, 0, 0),
		worldTransformB.GetOrientation()* Vector3(0, 1, 0),
		worldTransformB.GetOrientation()* Vector3(0, 0, 1),

		// Ax cross B XYZ
		Vector3::Cross(worldTransformA.GetOrientation() * Vector3(1, 0, 0), worldTransformB.GetOrientation() * Vector3(1, 0, 0)), 
		Vector3::Cross(worldTransformA.GetOrientation() * Vector3(1, 0, 0), worldTransformB.GetOrientation() * Vector3(0, 1, 0)),
		Vector3::Cross(worldTransformA.GetOrientation() * Vector3(1, 0, 0), worldTransformB.GetOrientation() * Vector3(0, 0, 1)),

		// Ay cross B XYZ
		Vector3::Cross(worldTransformA.GetOrientation() * Vector3(0, 1, 0), worldTransformB.GetOrientation() * Vector3(1, 0, 0)),
		Vector3::Cross(worldTransformA.GetOrientation() * Vector3(0, 1, 0), worldTransformB.GetOrientation() * Vector3(0, 1, 0)),
		Vector3::Cross(worldTransformA.GetOrientation() * Vector3(0, 1, 0), worldTransformB.GetOrientation() * Vector3(0, 0, 1)),

		// Az cross B XYZ
		Vector3::Cross(worldTransformA.GetOrientation() * Vector3(0, 0, 1), worldTransformB.GetOrientation() * Vector3(1, 0, 0)),
		Vector3::Cross(worldTransformA.GetOrientation() * Vector3(0, 0, 1), worldTransformB.GetOrientation() * Vector3(0, 1, 0)),
		Vector3::Cross(worldTransformA.GetOrientation() * Vector3(0, 0, 1), worldTransformB.GetOrientation() * Vector3(0, 0, 1)),
	};

	float leastPenetration = FLT_MAX;
	int penIndex = -1;

	for (int i = 0; i < 15; ++i) {
		if (Vector3::Dot(directions[i], directions[i]) < 0.99f)
			continue;

		// Get min and max extents for both shapes along an axis
		Vector3 maxA = OBBSupport(worldTransformA, directions[i]);
		Vector3 minA = OBBSupport(worldTransformA, -directions[i]);

		Vector3 maxB = OBBSupport(worldTransformB, directions[i]);
		Vector3 minB = OBBSupport(worldTransformB, -directions[i]);
		
		//Debug::DrawLine(-directions[i] * 500.0f, directions[i] * 500.0f, Debug::RED);
		
		// Project those points on to the line
		float denom = Vector3::Dot(directions[i], directions[i]);

		Vector3 minExtentA = directions[i] * (Vector3::Dot(minA, directions[i]) / denom);
		Vector3 maxExtentA = directions[i] * (Vector3::Dot(maxA, directions[i]) / denom);
		Vector3 minExtentB = directions[i] * (Vector3::Dot(minB, directions[i]) / denom);
		Vector3 maxExtentB = directions[i] * (Vector3::Dot(maxB, directions[i]) / denom);

		//Debug::DrawLine(minExtentA, minExtentA + Vector3(0,1,0), Debug::GREEN);
		//Debug::DrawLine(maxExtentA, maxExtentA + Vector3(0,1,0), Debug::CYAN);
		//Debug::DrawLine(minExtentB, minExtentB + Vector3(0,1,0), Debug::BLUE);
		//Debug::DrawLine(maxExtentB, maxExtentB + Vector3(0,1,0), Debug::MAGENTA);

		float distance = FLT_MAX;
		float length = FLT_MAX;
		float penDist = FLT_MAX;

		float left = Vector3::Dot(maxExtentA - minExtentA, minExtentB - minExtentA);
		float right = Vector3::Dot(minExtentA - maxExtentA, maxExtentB - maxExtentA);

		if (right > 0.0f) {
			// Object B to the left
			distance = (maxExtentB - maxExtentA).Length();
			length = (maxExtentA - minExtentA).Length();
			if (distance <= length) {
				penDist = length - distance;
				if (penDist < leastPenetration) {
					leastPenetration = penDist;
					collisionInfo.point.localA = minA;
					collisionInfo.point.localB = maxB;
					collisionInfo.point.normal = -directions[i];
					penIndex = i;
				}
				continue;
			}
		}

		if (left > 0.0f) {
			// Object A to the left
			distance = (minExtentB - minExtentA).Length();
			length = (maxExtentA - minExtentA).Length();
			if (distance <= length) {
				penDist = length - distance;
				if (penDist < leastPenetration) {
					leastPenetration = penDist;
					collisionInfo.point.localA = maxA;
					collisionInfo.point.localB = minB;
					collisionInfo.point.normal = directions[i];
					penIndex = i;
				}
				continue;
			}
		}

		if (left < 0.0f && right < 0.0f) {
			// Fully eveloped, should never be minimum intersection so don't update
			
			//Debug::DrawLine(-directions[i] * 500.0f, directions[i] * 500.0f, Debug::CYAN);
			//penDist = (maxExtentA - minExtentA).Length();
			penIndex = i;
			continue;
		}
		
		return false;
	}
	//std::cout << "Colliding" << std::endl;
	//Debug::DrawLine(collisionInfo.point.localA, collisionInfo.point.localA + Vector3(0, 5, 0), Debug::RED);
	//Debug::DrawLine(worldTransformA.GetPosition(), worldTransformA.GetPosition() + Vector3(0, 5, 0), Debug::CYAN);
	//Debug::DrawLine(collisionInfo.point.localB, collisionInfo.point.localB + Vector3(0, 5, 0), Debug::GREEN);
	//Debug::DrawLine(worldTransformB.GetPosition(), worldTransformB.GetPosition() + Vector3(0, 5, 0), Debug::MAGENTA);
	//Debug::DrawLine(collisionInfo.point.normal + Vector3(5,5,0), collisionInfo.point.normal + (collisionInfo.point.normal.Normalised() * 5.0f) + Vector3(5, 5, 0), Debug::BLUE);
	collisionInfo.point.penetration = leastPenetration;
	collisionInfo.point.localA -= worldTransformA.GetPosition();
	collisionInfo.point.localB -= worldTransformB.GetPosition();
	return true;
}

bool CollisionDetection::SphereCapsuleIntersection(
	const CapsuleVolume& volumeA, const Transform& worldTransformA,
	const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	// Generate plane
	Vector3 pointA = worldTransformA.GetPosition() + (worldTransformA.GetOrientation() * (Vector3(0, 1, 0) * (volumeA.GetHalfHeight() - volumeA.GetRadius())));
	Vector3 pointB = worldTransformA.GetPosition() - (worldTransformA.GetOrientation() * (Vector3(0, 1, 0) * (volumeA.GetHalfHeight() - volumeA.GetRadius())));

	Vector3 capsuleDir = pointA - pointB;
	float capsuleLineLength = capsuleDir.Length();
	capsuleDir.Normalise();

	Vector3 rayCapDir = worldTransformB.GetPosition() - pointB;
	float dot = Maths::Clamp(Vector3::Dot(rayCapDir, capsuleDir), 0.0f, capsuleLineLength);

	// Create sphere at point and check ray / sphere
	Vector3 spherePos = pointB + (capsuleDir * dot);
	SphereVolume s(volumeA.GetRadius());
	Transform t;
	t.SetPosition(spherePos);
	t.SetScale(Vector3(volumeA.GetRadius(), volumeA.GetRadius(), volumeA.GetRadius()));
	bool sphereCheck = SphereIntersection(s,t,volumeB,worldTransformB,collisionInfo);
	collisionInfo.point.localA += spherePos - worldTransformA.GetPosition();
	//Debug::DrawLine(collisionInfo.point.localA, collisionInfo.point.localA + (collisionInfo.point.normal * 5.0f), Vector4(1,0,0,1), 0.01f);
	return sphereCheck;
}

bool NCL::CollisionDetection::AABBCapsuleIntersection(const AABBVolume& volumeB, const Transform& worldTransformB, const CapsuleVolume& volumeA, const Transform& worldTransformA, CollisionInfo& collisionInfo)
{
	// Generate plane
	Vector3 pointA = worldTransformA.GetPosition() + (worldTransformA.GetOrientation() * (Vector3(0, 1, 0) * (volumeA.GetHalfHeight() - volumeA.GetRadius())));
	Vector3 pointB = worldTransformA.GetPosition() - (worldTransformA.GetOrientation() * (Vector3(0, 1, 0) * (volumeA.GetHalfHeight() - volumeA.GetRadius())));

	Vector3 cubePos = Maths::Clamp(worldTransformA.GetPosition(), worldTransformB.GetPosition() - volumeB.GetHalfDimensions(), worldTransformB.GetPosition() + volumeB.GetHalfDimensions());
	
	Vector3 capsuleDir = pointA - pointB;
	float capsuleLength = capsuleDir.Length();
	capsuleDir.Normalise();

	//Debug::DrawLine(cubePos, cubePos + Vector3(0, 5, 0), Debug::GREEN);
	float dot = Maths::Clamp(Vector3::Dot(cubePos - pointB, capsuleDir), 0.0f, capsuleLength);

	Vector3 spherePos = pointB + (capsuleDir * dot);
//	Debug::DrawLine(spherePos, cubePos, Debug::CYAN);

	SphereVolume s(volumeA.GetRadius());
	Transform t;
	t.SetPosition(spherePos);
	t.SetScale(Vector3(1, 1, 1) * volumeA.GetRadius());

	bool collided = AABBSphereIntersection(volumeB, worldTransformB, s, t, collisionInfo);
	collisionInfo.point.localB += t.GetPosition() - worldTransformA.GetPosition();
//	Debug::DrawLine(worldTransformA.GetPosition() + collisionInfo.point.localB, worldTransformA.GetPosition() + collisionInfo.point.localB + Vector3(0, 10, 0), Debug::MAGENTA);
	return collided;
}

bool NCL::CollisionDetection::OBBCapsuleIntersection(const OBBVolume& volumeB, const Transform& worldTransformB, const CapsuleVolume& volumeA, const Transform& worldTransformA, CollisionInfo& collisionInfo)
{
	Quaternion orientation = worldTransformB.GetOrientation();
	Matrix3 invTransform = Matrix3(orientation.Conjugate());

	CapsuleVolume c(volumeA.GetHalfHeight(), volumeA.GetRadius());
	Transform cT;
	cT.SetPosition(invTransform * worldTransformA.GetPosition());
	cT.SetScale(Vector3(volumeA.GetRadius(), volumeA.GetHalfHeight(), volumeA.GetRadius()));
	cT.SetOrientation(invTransform * worldTransformA.GetOrientation());

	AABBVolume a(volumeB.GetHalfDimensions());
	Transform aT;
	aT.SetPosition(invTransform * worldTransformB.GetPosition());
	aT.SetScale(volumeB.GetHalfDimensions());

	bool collided = AABBCapsuleIntersection(a, aT, c, cT, collisionInfo);
	collisionInfo.point.localA = Matrix3(orientation) * collisionInfo.point.localA;
	collisionInfo.point.localB = Matrix3(orientation) * collisionInfo.point.localB;
	collisionInfo.point.normal = Matrix3(orientation) * collisionInfo.point.normal;
	return collided;
}

bool NCL::CollisionDetection::SphereOBBIntersection(const SphereVolume& volumeA, const Transform& worldTransformA, const OBBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo)
{
	Quaternion orientation = worldTransformB.GetOrientation();
	Vector3 position = worldTransformB.GetPosition();

	Matrix3 transform = Matrix3(orientation);
	Matrix3 invTransform = Matrix3(orientation.Conjugate());

	SphereVolume s(volumeA.GetRadius());
	Transform sT;
	sT.SetPosition(invTransform * worldTransformA.GetPosition());
	sT.SetScale(Vector3(1,1,1) * volumeA.GetRadius());

	AABBVolume a(volumeB.GetHalfDimensions());
	Transform aT;
	aT.SetPosition(invTransform * worldTransformB.GetPosition());
	aT.SetScale(volumeB.GetHalfDimensions());

	bool check = AABBSphereIntersection(a, aT, s, sT, collisionInfo);

	if (check) {
		collisionInfo.point.localA = transform * collisionInfo.point.localA;
		collisionInfo.point.localB = transform * collisionInfo.point.localB;
		collisionInfo.point.normal = transform * collisionInfo.point.normal;
	}

	return check;
}

/*
* Reference:
https://wickedengine.net/2020/04/26/capsule-collision-detection/
*/

bool NCL::CollisionDetection::CapsuleIntersection(const CapsuleVolume& volumeA, const Transform& worldTransformA, const CapsuleVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo)
{

	// Cap A
	Vector3 aPointA = worldTransformA.GetPosition() + (worldTransformA.GetOrientation() * (Vector3(0, 1, 0) * (volumeA.GetHalfHeight() - volumeA.GetRadius())));
	Vector3 aPointB = worldTransformA.GetPosition() - (worldTransformA.GetOrientation() * (Vector3(0, 1, 0) * (volumeA.GetHalfHeight() - volumeA.GetRadius())));
	Vector3 aNormal = (aPointA - aPointB).Normalised();

	// Cap B
	Vector3 bPointA = worldTransformB.GetPosition() + (worldTransformB.GetOrientation() * (Vector3(0, 1, 0) * (volumeB.GetHalfHeight() - volumeB.GetRadius())));
	Vector3 bPointB = worldTransformB.GetPosition() - (worldTransformB.GetOrientation() * (Vector3(0, 1, 0) * (volumeB.GetHalfHeight() - volumeB.GetRadius())));
	Vector3 bNormal = (bPointA - bPointB).Normalised();

	// Distances
	float d0 = Vector3::Dot(bPointA - aPointA, bPointA - aPointA);
	float d1 = Vector3::Dot(bPointB - aPointA, bPointB - aPointA);
	float d2 = Vector3::Dot(bPointA - aPointB, bPointA - aPointB);
	float d3 = Vector3::Dot(bPointB - aPointB, bPointB - aPointB);

	Vector3 bestA;
	if (d2 < d0 || d2 < d1 || d3 < d0 || d3 < d1)
		bestA = aPointB;
	else
		bestA = aPointA;

	Vector3 bestB = ClosestPointOnLine(bPointA, bPointB, bestA);
	bestA = ClosestPointOnLine(aPointA, aPointB, bestB);

	SphereVolume aS(volumeA.GetRadius());
	Transform aT;
	aT.SetPosition(bestA);
	aT.SetScale(Vector3(volumeA.GetRadius(), volumeA.GetRadius(), volumeA.GetRadius()));

	SphereVolume bS(volumeB.GetRadius());
	Transform bT;
	bT.SetPosition(bestB);
	bT.SetScale(Vector3(volumeB.GetRadius(), volumeB.GetRadius(), volumeB.GetRadius()));

	bool hit = SphereIntersection(aS, aT, bS, bT, collisionInfo);
	if (hit) {
		collisionInfo.point.localA += bestA - worldTransformA.GetPosition();
		collisionInfo.point.localB += bestB - worldTransformB.GetPosition();
	}

	return hit;
}