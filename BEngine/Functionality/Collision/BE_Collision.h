#pragma once
#include "Utils_D3D.h"		//D3D Types
#include "MathHelper.h"

namespace BE_Collision
{
	//Collider Type Definitions
	enum class BE_ColliderTypes
	{
		RECT_COLLIDER,
		CIRCLE_COLLIDER,
		LINE_COLLIDER,
		POLYGON_COLLIDER
	};

	//Forward Declarations
	struct Circle_Collider;
	struct Line_Collider;
	struct Polygon_Collider;

	struct Rect_Collider
	{
		////////////////////
		/// Constructors ///
		////////////////////

		Rect_Collider() {}
		Rect_Collider(XMF2 position, XMF2 size)
			:m_Centre(position), m_Size(size)
		{}
		Rect_Collider(float posX, float posY, float width, float height)
			:m_Centre(posX, posY), m_Size(width, height)
		{}
		~Rect_Collider() {}

		//////////////////
		/// Operations ///
		//////////////////

		//
		//Intersect Tests
		//

		//Rect vs Rect (Is both Intersect + Contains at current)
		bool Intersects(const Rect_Collider& rectIn) const;
		//Rect vs Circle
		bool Intersects(const Circle_Collider& circleIn) const;
		//Rect vs Line
		bool Intersects(const Line_Collider& lineIn) const;
		//Rect vs Polygon
		bool Intersects(const Polygon_Collider& polygonIn) const;

		//
		//Contains Tests
		//

		//Rect vs Rect
		bool Contains(const Rect_Collider& rectIn) const;
		//AABB vs Point
		bool Contains(const XMF2& pointIn) const;

		////////////
		/// Data ///
		////////////

		//Centre/Position of the collider
		XMF2 m_Centre = { 0.f, 0.f };
		//Size (Width/Height) of the rect
		XMF2 m_Size = { 0.f, 0.f };
	};

	struct Circle_Collider
	{
		////////////////////
		/// Constructors ///
		////////////////////

		Circle_Collider() {}
		Circle_Collider(XMF2 position, float radius)
			:m_Centre(position), m_Radius(radius)
		{}
		~Circle_Collider() {}

		//////////////////
		/// Operations ///
		//////////////////

		//
		//Intersect Tests
		//

		//Circle vs Rect
		bool Intersects(const Rect_Collider& rectIn) const;
		//Circle vs Circle
		bool Intersects(const Circle_Collider& circleIn) const;
		//Circle vs Line
		bool Intersects(const Line_Collider& lineIn) const;
		//Circle vs Polygon
		bool Intersects(const Polygon_Collider& polygonIn) const;

		//
		//Contains Tests
		//

		////////////
		/// Data ///
		////////////

		//Centre/Position of the collider
		XMF2 m_Centre = { 0.f, 0.f };
		//Radius/Size of the circle
		float m_Radius = 0.f;
	};

	struct Line_Collider
	{
		////////////////////
		/// Constructors ///
		////////////////////

		Line_Collider() {}
		Line_Collider(XMF2 pointA, XMF2 pointB)
			:m_PointA(pointA), m_PointB(pointB)
		{}
		Line_Collider(float pointAX, float pointAY, float pointBX, float pointBY)
			:m_PointA(pointAX, pointAY), m_PointB(pointBX, pointBY)
		{}
		~Line_Collider() {}

		//////////////////
		/// Operations ///
		//////////////////

		//
		//Intersect Tests
		//

		//Line vs Rect
		bool Intersects(const Rect_Collider& rectIn) const;
		//Line vs Circle
		bool Intersects(const Circle_Collider& circleIn) const;
		//Line vs Line
		bool Intersects(const Line_Collider& lineIn) const;
		//Line vs Polygon
		bool Intersects(const Polygon_Collider& polygonIn) const;

		//
		//Utilities
		//

		inline float GetLength() const
		{
			return MathHelper::GetDistance(m_PointA.x, m_PointA.y, m_PointB.x, m_PointB.y);
		}

		////////////
		/// Data ///
		////////////

		//Point A/B of line points
		XMF2 m_PointA = { 0.f, 0.f };
		XMF2 m_PointB = { 0.f, 0.f };
	};

	struct Polygon_Collider
	{
		//////////////////
		/// Defintions ///
		//////////////////

		static const int RESERVE_COUNT = 8;

		////////////////////
		/// Constructors ///
		////////////////////

		Polygon_Collider() 
		{
			m_ModelShape.reserve(RESERVE_COUNT);
			m_ModelShape.reserve(RESERVE_COUNT);
		}
		~Polygon_Collider()
		{
			m_ModelShape.clear();
			m_Points.clear();
		}

		Polygon_Collider& operator=(const Polygon_Collider& in)
		{
			m_Rotation = in.m_Rotation;
			m_Position = in.m_Position;

			m_ModelShape.clear();
			m_Points.clear();
			m_ModelShape.reserve(in.m_ModelShape.size());
			m_Points.reserve(in.m_Points.size());
			m_ModelShape = in.m_ModelShape;
			m_Points = in.m_Points;

			return *this;
		}

		//////////////////
		/// Operations ///
		//////////////////
				
		//
		//Intersect Tests
		//

		//Polygon vs Rect
		bool Intersects(const Rect_Collider& rectIn) const;
		//Polygon vs Circle
		bool Intersects(const Circle_Collider& circleIn) const;
		//Polygon vs Line
		bool Intersects(const Line_Collider& lineIn) const;
		//Polygon vs Polygon
		bool Intersects(const Polygon_Collider& polygonIn) const;


		//
		//Utility
		//

		//Configures polygon as a rect, using width/height to space vertices around a position.
		//Optional origin (0.5f by default, which would build the rect around a centre point), and rotation variable.
		void CreateFromRect(float width, float height, Vec2 position, float rotation = 0.f, Vec2 origin = {0.5f, 0.5f});

		//
		//General Operation
		//

		//Regenerates the points with current position and rotation
		void UpdatePoints();

		////////////
		/// Data ///
		////////////

		//The original shape of the polygon (box, star, contextual shape etc)
		std::vector<Vec2> m_ModelShape;
		//The transformed shape (where position, rotation etc, have been applied)
		std::vector<Vec2> m_Points;
		//Stores the sum of rotates made to polygon so the positions can be reset if called
		float m_Rotation = 0.f;
		//Scales the vertices
		Vec2 m_Scale = { 1.f, 1.f };
		//The world space position of the polygon
		Vec2 m_Position = { 0.f, 0.f };
	};

	/*
		Omni-Collider struct that uses a union + tag to indicate what collider should be used.
	*/
	struct Collider
	{
		Collider() {}
		~Collider()
		{

		}

		Collider& operator=(const Collider& in)
		{
			m_TypeID = in.m_TypeID;
			m_ColliderActive = in.m_ColliderActive;
			m_ColliderLayer = in.m_ColliderLayer;

			switch (m_TypeID)
			{
			case BE_ColliderTypes::RECT_COLLIDER:
				m_Rect = in.m_Rect;
				break;

			case BE_ColliderTypes::CIRCLE_COLLIDER:
				m_Circle = in.m_Circle;
				break;

			case BE_ColliderTypes::LINE_COLLIDER:
				m_Line = in.m_Line;
				break;

			case BE_ColliderTypes::POLYGON_COLLIDER:
				m_Polygon = in.m_Polygon;
			}

			return *this;
		}
		
		//As data is unionised, keep track of what data to use
		BE_ColliderTypes m_TypeID = BE_ColliderTypes::RECT_COLLIDER;		
		//If the collision box is active, and should be considered for collision tests
		bool m_ColliderActive = true;
		//Colliders layer as an int (define externally)
		int m_ColliderLayer = -1;

		//Different collider types bundled as a union
		union
		{
			Rect_Collider m_Rect;
			Circle_Collider m_Circle;
			Line_Collider m_Line;
		};

		//Polygon has non-trival deconstructor & dynamic memory concerns, so stored seperate of the union
		Polygon_Collider m_Polygon;
	};


	////////////////////////////
	/// Shared Methods (WIP) ///
	////////////////////////////

	//Performs SAT (Seperated Axis Theorem) based check between two CONVEX polygons
	//Returns true if overlap found in every axis
	bool SAT_Check(Polygon_Collider& lPoly, Polygon_Collider& rPoly);
}



