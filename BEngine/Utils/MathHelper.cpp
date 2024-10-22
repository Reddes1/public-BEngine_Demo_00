#include "MathHelper.h"

#include <float.h>
#include <cmath>

using namespace DirectX;

const float MathHelper::Infinity = FLT_MAX;
const float MathHelper::Pi = 3.1415926535f;

//Random generator engine
std::mt19937 MathHelper::g_RAND_GEN;

float MathHelper::DegreesToRadian(float x)
{
	return (x * Pi) / 180.0f;
}
float MathHelper::RadianToDegrees(float x)
{
	return (x * 180.0f) / Pi;
}

float MathHelper::AngleFromXY(float x, float y)
{
	float theta = 0.0f;

	// Quadrant I or IV
	if (x >= 0.0f)
	{
		// If x = 0, then atanf(y/x) = +pi/2 if y > 0
		//                atanf(y/x) = -pi/2 if y < 0
		theta = atanf(y / x); // in [-pi/2, +pi/2]

		if (theta < 0.0f)
			theta += 2.0f * Pi; // in [0, 2*pi).
	}

	// Quadrant II or III
	else
		theta = atanf(y / x) + Pi; // in [0, 2*pi).

	return theta;
}

XMVECTOR MathHelper::RandUnitVec3()
{
	XMVECTOR One = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR Zero = XMVectorZero();

	// Keep trying until we get a point on/in the hemisphere.
	while (true)
	{
		// Generate random point in the cube [-1,1]^3.
		XMVECTOR v = XMVectorSet(MathHelper::RandF(-1.0f, 1.0f), MathHelper::RandF(-1.0f, 1.0f), MathHelper::RandF(-1.0f, 1.0f), 0.0f);

		// Ignore points outside the unit sphere in order to get an even distribution 
		// over the unit sphere.  Otherwise points will clump more on the sphere near 
		// the corners of the cube.

		if (XMVector3Greater(XMVector3LengthSq(v), One))
			continue;

		return XMVector3Normalize(v);
	}
}

XMVECTOR MathHelper::RandHemisphereUnitVec3(XMVECTOR n)
{
	XMVECTOR One = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR Zero = XMVectorZero();

	// Keep trying until we get a point on/in the hemisphere.
	while (true)
	{
		// Generate random point in the cube [-1,1]^3.
		XMVECTOR v = XMVectorSet(MathHelper::RandF(-1.0f, 1.0f), MathHelper::RandF(-1.0f, 1.0f), MathHelper::RandF(-1.0f, 1.0f), 0.0f);

		// Ignore points outside the unit sphere in order to get an even distribution 
		// over the unit sphere.  Otherwise points will clump more on the sphere near 
		// the corners of the cube.

		if (XMVector3Greater(XMVector3LengthSq(v), One))
			continue;

		// Ignore points in the bottom hemisphere.
		if (XMVector3Less(XMVector3Dot(n, v), Zero))
			continue;

		return XMVector3Normalize(v);
	}
}

float MathHelper::GetSimplePolygonArea(const std::vector<DirectX::SimpleMath::Vector2>& vertices)
{
	//Holding container
	float area = 0.f;

	//Loop through each vertice and build area value up (apart from last vertices and its connection to the first)
	for (unsigned i(0); i < vertices.size() - 1; ++i)
	{
		area += vertices[i].x * vertices[i + 1].y;
		area -= vertices[i].y * vertices[i + 1].x;
	}
	//Calculate final vertices connection
	area += vertices[vertices.size() - 1].x * vertices[0].y;
	area -= vertices[vertices.size() - 1].y * vertices[0].x;

	//Finalise by getting absolute value and dividing, then return it
	area = std::abs(area) / 2.f;
	return area;
}

float MathHelper::GetConvexPolygonPerimeter(const std::vector<DirectX::SimpleMath::Vector2>& vertices)
{
	//Holding container
	float perimeter = 0.f;

	//Loop through each vertice and build perimeter value up (apart from last vertices and its connection to the first)
	for (unsigned i(0); i < vertices.size() - 1; ++i)
	{
		//Calculate the length of the line
		float sideLength = std::sqrtf(std::powf(vertices[i + 1].x - vertices[i].x, 2) + std::powf(vertices[i + 1].y - vertices[i].y, 2));
		perimeter += sideLength;
	}

	//Calculate final vertices connection
	perimeter += std::sqrtf(std::powf(vertices[0].x - vertices[vertices.size() - 1].x, 2) + std::powf(vertices[0].x - vertices[vertices.size() - 1].y, 2));

	//Return completed value
	return perimeter;
}

float MathHelper::GetConvexPolygonInradius(const std::vector<DirectX::SimpleMath::Vector2>& vertices)
{
	//Get the area and perimeter of the polygon
	float a = GetSimplePolygonArea(vertices);
	float p = GetConvexPolygonPerimeter(vertices);

	//Check to see if the polygon is degenerate or not, and return area/perimeter if it isnt
	if (a > 0.f && p > 0.f)
		return a / p;
	//Degenerate, so return -1.f as an indicator of this fact
	else
		return -1.f;
}
