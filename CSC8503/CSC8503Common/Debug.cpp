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

	newEntry.start = startpoint;
	newEntry.end = endpoint;
	newEntry.colour = colour;
	newEntry.time = time;

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
	float step = 0.5f;
	float adjRadius = radius + 0.05f;
	//To allow lines to be on the surface of the sphere at lower 'resolutions'
	Vector3 prevPoint = centre + Vector3(adjRadius, 0, 0);

	//Draw One along xy axis...
	for (float angle = 0.0f; angle < 2 * M_PI; angle += step) {
		Vector3 pointOnSphere = Vector3(cos(angle) * adjRadius + centre.x, sin(angle) * adjRadius + centre.y, centre.z);
		Debug::DrawLine(prevPoint, pointOnSphere, colour, time);
		prevPoint = pointOnSphere;
	}
	prevPoint = centre + Vector3(0, 0, adjRadius);

	//And one along the zy axis...
	for (float angle = 0.0f; angle < 2 * M_PI; angle += step) {
		Vector3 pointOnSphere = Vector3(centre.x, sin(angle) * adjRadius + centre.y, cos(angle) * adjRadius + centre.z);
		Debug::DrawLine(prevPoint, pointOnSphere, colour, time);
		prevPoint = pointOnSphere;
	}
	prevPoint = centre + Vector3(adjRadius, 0, 0);

	//And one along the xz axis...
	for (float angle = 0.0f; angle < 2 * M_PI; angle += step) {
		Vector3 pointOnSphere = Vector3(cos(angle) * adjRadius + centre.x, centre.y, sin(angle) * adjRadius + centre.z);
		Debug::DrawLine(prevPoint, pointOnSphere, colour, time);
		prevPoint = pointOnSphere;
	}
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