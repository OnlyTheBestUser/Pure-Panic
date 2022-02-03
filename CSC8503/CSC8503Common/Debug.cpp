#include "Debug.h"
#include "../../Common/Matrix4.h"
#include "../../Common/Matrix3.h"
#define _USE_MATH_DEFINES
#include <math.h>
using namespace NCL;

RendererBase* Debug::renderer = nullptr;

std::vector<Debug::DebugStringEntry>	Debug::stringEntries;
std::vector<Debug::DebugLineEntry>		Debug::lineEntries;

const Vector4 Debug::RED	= Vector4(1, 0, 0, 1);
const Vector4 Debug::GREEN	= Vector4(0, 1, 0, 1);
const Vector4 Debug::BLUE	= Vector4(0, 0, 1, 1);

const Vector4 Debug::BLACK	= Vector4(0, 0, 0, 1);
const Vector4 Debug::WHITE	= Vector4(1, 1, 1, 1);

const Vector4 Debug::YELLOW		= Vector4(1, 1, 0, 1);
const Vector4 Debug::MAGENTA	= Vector4(1, 0, 1, 1);
const Vector4 Debug::CYAN		= Vector4(0, 1, 1, 1);


void Debug::Print(const std::string& text, const Vector2&pos, const Vector4& colour) {
	DebugStringEntry newEntry;

	newEntry.data		= text;
	newEntry.position	= pos;
	newEntry.colour		= colour;

	stringEntries.emplace_back(newEntry);
}

void Debug::DrawLine(const Vector3& startpoint, const Vector3& endpoint, const Vector4& colour, float time) {
	DebugLineEntry newEntry;

	newEntry.start	= startpoint;
	newEntry.end	= endpoint;
	newEntry.colour = colour;
	newEntry.time	= time;

	lineEntries.emplace_back(newEntry);
}

void Debug::DrawCube(const Vector3& center, const Vector3& halfSizes, const Vector4& colour, float time, const Quaternion& rotation) {
	Vector3 adjSizes = halfSizes + Vector3(0.01f, 0.01f, 0.01f);
	Matrix3 rot = Matrix3(rotation);

	Vector3 topFrontRight = center + adjSizes;
	Vector3 bottomLeftBack = center - adjSizes;

	//Top-front-right corner points
	Debug::DrawLine(center + rot * adjSizes, center + rot * (adjSizes * Vector3(-1, 1, 1)), colour, time);
	Debug::DrawLine(center + rot * adjSizes, center + rot * (adjSizes * Vector3(1, -1, 1)), colour, time);
	Debug::DrawLine(center + rot * adjSizes, center + rot * (adjSizes * Vector3(1, 1, -1)), colour, time);
	//Bottom-back-left corner points
	Debug::DrawLine(center - rot * adjSizes, center + rot * (adjSizes * Vector3(1, -1, -1)), colour, time);
	Debug::DrawLine(center - rot * adjSizes, center + rot * (adjSizes * Vector3(-1, 1, -1)), colour, time);
	Debug::DrawLine(center - rot * adjSizes, center + rot * (adjSizes * Vector3(-1, -1, 1)), colour, time);
	//The connecting lines
	Debug::DrawLine(center + rot * (adjSizes * Vector3(-1, 1, 1)), center + rot * (adjSizes * Vector3(-1, 1, -1)), colour, time);
	Debug::DrawLine(center + rot * (adjSizes * Vector3(1, -1, 1)), center + rot * (adjSizes * Vector3(1, -1, -1)), colour, time);
	Debug::DrawLine(center + rot * (adjSizes * Vector3(1, -1, -1)), center + rot * (adjSizes * Vector3(1, 1, -1)), colour, time);
	Debug::DrawLine(center + rot * (adjSizes * Vector3(1, -1, 1)), center + rot * (adjSizes * Vector3(-1, -1, 1)), colour, time);
	Debug::DrawLine(center + rot * (adjSizes * Vector3(-1, -1, 1)), center + rot * (adjSizes * Vector3(-1, 1, 1)), colour, time);
	Debug::DrawLine(center + rot * (adjSizes * Vector3(-1, 1, -1)), center + rot * (adjSizes * Vector3(1, 1, -1)), colour, time);
}

void Debug::DrawSphere(const Vector3& centre, const float& radius, const Vector4& colour, float time) {
	float expandedRad = radius + 0.05f;
	DrawCircle(centre, expandedRad, Quaternion::EulerAnglesToQuaternion(0.0f, 0.0f, 0.0f), colour, time);
	DrawCircle(centre, expandedRad, Quaternion::EulerAnglesToQuaternion(90.0f, 0.0f, 0.0f), colour, time);
	DrawCircle(centre, expandedRad, Quaternion::EulerAnglesToQuaternion(0.0f, 0.0f, 90.0f), colour, time);
}

void Debug::DrawCircle(const Vector3& centre, const float& radius, const Quaternion& rotation, const Vector4& colour, float time) {
	Vector3 firstpoint = Vector3(centre.x, centre.y, centre.z) + rotation * Vector3(radius, 0, 0);
	Vector3 prevPoint = firstpoint;
	Vector3 curPoint;
	int pointNum = 20;
	float rot = 0.0f;
	for (int i = 0; i < pointNum; i++) {
		rot += ((M_PI * 2) / pointNum);
		curPoint = Vector3(centre.x, centre.y, centre.z) + rotation * Vector3((cos(rot) * radius), 0, (sin(rot) * radius));
		Debug::DrawLine(prevPoint, curPoint, Vector4(1, 0, 0, 1));
		prevPoint = curPoint;
	}
}

void Debug::DrawCapsule(const Vector3& centre, const float& radius, const float& halfHeight, const Quaternion& rotation, const Vector4& colour, float time){
	Vector3 topPos = centre + rotation * Vector3(0, halfHeight-radius, 0);
	Vector3 botPos = centre - rotation * Vector3(0, halfHeight-radius, 0);

	DrawSphere(topPos, radius, colour, time);
	DrawSphere(botPos, radius, colour, time);
	DrawLine(topPos + rotation * Vector3(radius, 0, 0), botPos + rotation * Vector3(radius, 0, 0), colour);
	DrawLine(topPos + rotation * Vector3(-radius, 0, 0), botPos + rotation * Vector3(-radius, 0, 0), colour);
	DrawLine(topPos + rotation * Vector3(0, 0, radius), botPos + rotation * Vector3(0, 0, radius), colour);
	DrawLine(topPos + rotation * Vector3(0, 0, -radius), botPos + rotation * Vector3(0, 0, -radius), colour);
}

void Debug::DrawAxisLines(const Matrix4& modelMatrix, float scaleBoost, float time) {
	Matrix4 local = modelMatrix;
	local.SetPositionVector({0, 0, 0});

	Vector3 fwd		= local * Vector4(0, 0, -1, 1.0f);
	Vector3 up		= local * Vector4(0, 1, 0, 1.0f);
	Vector3 right	= local * Vector4(1, 0, 0, 1.0f);

	Vector3 worldPos = modelMatrix.GetPositionVector();

	DrawLine(worldPos, worldPos + (right * scaleBoost)	, Debug::RED, time);
	DrawLine(worldPos, worldPos + (up * scaleBoost)		, Debug::GREEN, time);
	DrawLine(worldPos, worldPos + (fwd * scaleBoost)	, Debug::BLUE, time);
}


void Debug::FlushRenderables(float dt) {
	if (!renderer) {
		return;
	}
	for (const auto& i : stringEntries) {
		renderer->DrawString(i.data, i.position);
	}
	int trim = 0;
	for (int i = 0; i < lineEntries.size(); ) {
		DebugLineEntry* e = &lineEntries[i]; 
		renderer->DrawLine(e->start, e->end, e->colour);
		e->time -= dt;
		if (e->time < 0) {			
			trim++;				
			lineEntries[i] = lineEntries[lineEntries.size() - trim];
		}
		else {
			++i;
		}		
		if (i + trim >= lineEntries.size()) {
			break;
		}
	}
	lineEntries.resize(lineEntries.size() - trim);

	stringEntries.clear();
}