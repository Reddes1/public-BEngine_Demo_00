//***************************************************************************************
// MathHelper.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Modified and extended by Benjamin Kimberley for use in BEngine (2023-)
// Provides several different mathematical based support functions, utilities and tools
// for several different requirements and contexts (both in general and game based).
//
//***************************************************************************************

#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include <cstdint>
#include <SimpleMath.h>
#include <time.h>
#include <random>
#include <chrono>

class MathHelper
{
private:

	static std::mt19937 g_RAND_GEN;

public:



	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	static const float Infinity;
	static const float Pi;
	static const int g_MAX_INT = 2147483647;

	//================================================================================\\
	// General
	//================================================================================\\

	//Classic return true if even
	static bool IsEven(int num)
	{
		return (num % 2) == 0;
	}

	/*
		Comparing floating point numbers is tricky. A number is rarely zero or one, it's more common for it
		to be 0.001 or 0.998. So we need a way of comparing numbers that takes this into account,
		this simple approach won't work for everything, but it's useful most of the time.
	*/
	static bool Equals(float a, float b)
	{
		return (fabs(a - b) < 0.0001f) ? true : false;
	}

	//Gets the distance, or line length, between two points in space
	static float GetDistance(float x1, float y1, float x2, float y2)
	{
		//Calculate adj and opp
		float x = x1 - x2;
		float y = y1 - y2;

		//return hypo
		return sqrtf((x * x) + (y * y));
	}

	//cx, x1, x2, x1, cy, y1, y2
	static float GetDotProduct(float x1, float y1, float x2, float y2)
	{
		return float((x1 * x2) + (y1 * y2));
	}
	//Returns a perportional scaling value against target value (e.g. t(5), c(2), r(2.5))
	static float GetProportionalScaler(float target, float current)
	{
		assert(current != 0.f);
		return target / current;
	}

	//Calculate the difference between two numbers, with optional flag to return only positive numbers only
	static float GetPercentageDifference(float val1, float val2, bool retPositiveOnly)
	{
		float percentage = ((val1 - val2) / ((val1 + val2) / 2)) * 100;

		if (retPositiveOnly && percentage < 0)
			return -percentage;
		else
			return percentage;
	}



	//================================================================================\\
	// Randomisation ( <random> Library )
	//================================================================================\\

	//Seeds random engine with given value, if seed = -1, seeds random via high_resolution_clock
	static void SeedRandom(int seed = -1) 
	{
		if (seed == -1) {
			auto now = std::chrono::high_resolution_clock::now();
			auto duration = now.time_since_epoch();
			auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
			g_RAND_GEN.seed(static_cast<unsigned int>(microseconds));
		}
		else {
			g_RAND_GEN.seed(seed);
		}
	}
	static int Rand(int a, int b)
	{
		//Prevent undefined behaviour
		if (a > b)
		{
			std::swap(a, b);
		}
		std::uniform_int_distribution<int> dist(a, b);
		return dist(g_RAND_GEN);
	}
	static float RandF(float a, float b)
	{
		//Prevent undefined behaviour
		if (a > b) {
			std::swap(a, b);
		}
		std::uniform_real_distribution<float> dist(a, b);
		return dist(g_RAND_GEN);
	}


	//================================================================================\\
	// Angles & Directions
	//================================================================================\\

	// Returns the polar angle of the point (x,y) in [0, 2*PI).
	static float AngleFromXY(float x, float y);

	//Returns a directional vector between and origin point towards a target
	//This works for screen space, not graph space (-result for graph space)
	static DirectX::SimpleMath::Vector2 GetDirectionalVector(const DirectX::SimpleMath::Vector2& origin, const DirectX::SimpleMath::Vector2& target)
	{
		return DirectX::SimpleMath::Vector2(target.x - origin.x, target.y - origin.y);
	}



	//================================================================================\\
	// Limitations
	//================================================================================\\

	template<typename T>
	static T Min(const T& a, const T& b)
	{
		return a < b ? a : b;
	}
	template<typename T>
	static T Max(const T& a, const T& b)
	{
		return a > b ? a : b;
	}
	template<typename T>
	static T Clamp(const T& x, const T& low, const T& high)
	{
		return x < low ? low : (x > high ? high : x);
	}



	//================================================================================\\
	// Conversions
	//================================================================================\\

	//Convert Degrees into Radian
	static float DegreesToRadian(float x);

	//Convert Radian into Degrees
	static float RadianToDegrees(float x);

	//Calculates percentage one number is of another(i.e 25 of 100 = 25 %).
	//Optional flag to adjust the decimal position to make it a whole number
	static float GetPercentageOfNumber(float lowNum, float highNum, bool adjDecimalPos)
	{
		float percentage = (lowNum / highNum);
		if (adjDecimalPos)
			return percentage * 100;
		else
			return percentage;
	}



	//================================================================================\\
	// Matrices
	//================================================================================\\

	//Returns an indentity matrix
	static DirectX::XMFLOAT4X4 Identity4x4()
	{
		static DirectX::XMFLOAT4X4 I(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		return I;
	}

	static DirectX::XMVECTOR SphericalToCartesian(float radius, float theta, float phi)
	{
		return DirectX::XMVectorSet(
			radius * sinf(phi) * cosf(theta),
			radius * cosf(phi),
			radius * sinf(phi) * sinf(theta),
			1.0f);
	}

	static DirectX::XMMATRIX InverseTranspose(DirectX::CXMMATRIX M)
	{
		// Inverse-transpose is just applied to normals.  So zero out 
		// translation row so that it doesn't get into our inverse-transpose
		// calculation--we don't want the inverse-transpose of the translation.
		DirectX::XMMATRIX A = M;
		A.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(A);
		return DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, A));
	}

	//Emulates 2x2 / 1x2 Matrix Multiplication. Specifically used to translate a 2D coordinate with a given angle.
	//Ideal for rotating an anchor point alongside an objects overall rotation
	static DirectX::SimpleMath::Vector2 GetRotatedCoordinates(const DirectX::SimpleMath::Vector2& coords, float angle)
	{
		float sin = std::sinf(angle);
		float cos = std::cosf(angle);

		return DirectX::SimpleMath::Vector2(
			(cos * coords.x) + (-sin * coords.y), //X
			(sin * coords.x) + (-cos * coords.y)  //Y
		);
	}

	static DirectX::XMVECTOR RandUnitVec3();
	static DirectX::XMVECTOR RandHemisphereUnitVec3(DirectX::XMVECTOR n);



	//================================================================================\\
	// Polygons
	//================================================================================\\

	//Returns the area of a simple polygon (no overlapping lines) using the Shoelace Formula.
	//Works for concave and convex polygons just fine.
	static float GetSimplePolygonArea(const std::vector<DirectX::SimpleMath::Vector2>& vertices);

	//Returns the perimeter value of a concave polygon
	static float GetConvexPolygonPerimeter(const std::vector<DirectX::SimpleMath::Vector2>& vertices);

	//Returns the Inradius of a convex polygon. Returns -1.f if the polygon is determined to be Degenerate.
	//This may occur for some of the following reasons: Zero Area, Coincident Vertices, Self-Intersecting Edges, Incorrect Winding Order, Non-Convex with Holes
	static float GetConvexPolygonInradius(const std::vector<DirectX::SimpleMath::Vector2>& vertices);



	//================================================================================\\
	// Lerps
	//================================================================================\\

	//Sourced from: http://www.gizma.com/easing/
	//Locally store cTime as elapsed time, effectStr should be the inverse of startVal to
	//produce full effect (100: -100) (Over may break the effect, under means full slowdown wont be reached)

	//cTime = Elapsed Time (i.e. accumulated delta time)
	//startVal = Initial Speed
	//effectStr = Change in Value, like the strength of the effect (should typically be the inverse of startVal, otherwise breaks effect)
	//duration = Duration of the effect in real time (i.e. Ease In over 4 seconds)

	//From 100-0, producing decelerating effect
	static float EaseOut(float cTime, float startVal, float effectStr, float duration)
	{
		cTime /= duration;
		return -effectStr * cTime * (cTime - 2) + startVal;
	}
	//From 0-100, producing accelerating effect
	static float EaseIn(float cTime, float startVal, float effectStr, float duration)
	{
		cTime /= duration;
		return effectStr * cTime * cTime + duration;
	}
	//From 0-100 to 100-0, accelerating and then decelerating 
	static float EaseInToEaseOut(float cTime, float startVal, float effectStr, float duration)
	{
		cTime /= duration / 2.f;
		if (cTime < 1.f)
			return effectStr / 2.f * cTime * cTime + startVal;
		--cTime;
		return -effectStr / 2.f * (cTime * (cTime - 2.f) - 1.f) + startVal;
	}

	//Standard linear lerp
	template<typename T>
	static T Lerp(const T& a, const T& b, float t)
	{
		return a + (b - a) * t;
	}


};