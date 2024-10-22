#include "BE_Collision.h"

namespace BE_Collision
{
	/////////////////////
	/// Rect Collider ///
	/////////////////////

	//
	//Rect Collider: Intersect Methods
	//

	bool Rect_Collider::Intersects(const Rect_Collider& right) const
	{
		//Get half sizes (left = this, right = in)
		float leftW = m_Size.x * 0.5f;
		float leftH = m_Size.y * 0.5f;
		float rightW = right.m_Size.x * 0.5f;
		float rightH = right.m_Size.y * 0.5f;

		//Checks that the colliders are completely seperate (checks internally)
		return
			m_Centre.x + leftW >= right.m_Centre.x - rightW && //Left Edge Against Right Edge
			m_Centre.x - leftW <= right.m_Centre.x + rightW && //Right Edge Against Left Edge
			m_Centre.y + leftH >= right.m_Centre.y - rightH && //Bottom Edge Against Top Edge
			m_Centre.y - leftH <= right.m_Centre.y + rightH;   //Top Edge Against Bottom Edge
	}

	bool Rect_Collider::Intersects(const Circle_Collider& right) const
	{
		//Rect half sizes
		float leftW = m_Size.x * 0.5f;
		float leftH = m_Size.y * 0.5f;

		//Testing container for edge testing (if circle is inside the rect, then these wont
		//and collision occured).
		float testX = right.m_Centre.x;
		float testY = right.m_Centre.y;

		//Find closest rect edge
		if (right.m_Centre.x < m_Centre.x - leftW)	//Left Edge
			testX = m_Centre.x - leftW;
		else if (right.m_Centre.x > m_Centre.x + leftW)			//Right Edge
			testX = m_Centre.x + leftW;
		if (right.m_Centre.y < m_Centre.y - leftH)				//Top Edge
			testY = m_Centre.y - leftH;
		else if (right.m_Centre.y > m_Centre.y + leftH)			//Bottom Edge
			testY = m_Centre.y + leftH;
		
		//Get distance to closest edge via Pythagoras
		float dist = MathHelper::GetDistance(
			right.m_Centre.x, right.m_Centre.y,
			testX, testY
		);

		//If distance to edge less than then radius, collision occured
		return dist <= right.m_Radius;
	}

	bool Rect_Collider::Intersects(const Line_Collider& right) const
	{
		//Calculate half sizes
		float leftW = m_Size.x * 0.5f;
		float leftH = m_Size.y * 0.5f;

		//Construct a line segment collider for each edge and test it against given line
		return
			right.Intersects(Line_Collider(m_Centre.x - leftW, m_Centre.y - leftH, m_Centre.x + leftW, m_Centre.y - leftH)) || //Top Edge
			right.Intersects(Line_Collider(m_Centre.x - leftW, m_Centre.y + leftH, m_Centre.x + leftW, m_Centre.y + leftH)) || //Bottom Edge
			right.Intersects(Line_Collider(m_Centre.x - leftW, m_Centre.y - leftH, m_Centre.x - leftW, m_Centre.y + leftH)) || //Left Edge
			right.Intersects(Line_Collider(m_Centre.x + leftW, m_Centre.y - leftH, m_Centre.x + leftW, m_Centre.y + leftH));   //Right Edge
	}

	bool Rect_Collider::Intersects(const Polygon_Collider& right) const
	{
		return right.Intersects(*this);
	}

	//
	//Rect Collider: Contains Methods
	//

	bool Rect_Collider::Contains(const Rect_Collider& right) const
	{
		//Get half sizes (left = this, right = in)
		float leftW = m_Size.x * 0.5f;
		float leftH = m_Size.y * 0.5f;
		float rightW = right.m_Size.x * 0.5f;
		float rightH = right.m_Size.y * 0.5f;

		//Checks that the colliders are completely seperate (checks internally)
		return
			m_Centre.x + leftW >= right.m_Centre.x - rightW && //Left Edge Against Right Edge
			m_Centre.x - leftW <= right.m_Centre.x + rightW && //Right Edge Against Left Edge
			m_Centre.y + leftH >= right.m_Centre.y - rightH && //Bottom Edge Against Top Edge
			m_Centre.y - leftH <= right.m_Centre.y + rightH;   //Top Edge Against Bottom Edge
	}

	bool Rect_Collider::Contains(const XMF2& right) const
	{
		//Rect half sizes
		float leftW = m_Size.x * 0.5f;
		float leftH = m_Size.y * 0.5f;

		return
			right.x >= m_Centre.x - leftW &&	//Left Edge Check
			right.x <= m_Centre.x + leftW &&	//Right Edge Check
			right.y >= m_Centre.y - leftH &&	//Top Edge Check
			right.y <= m_Centre.y + leftH;		//Bottom Edge Check
	}

	//
	//Orientated Rect Collider: Contains Methods
	//


	///////////////////////
	/// Circle Collider ///
	///////////////////////

	//
	//Circle Collider: Intersect Methods
	//

	bool Circle_Collider::Intersects(const Rect_Collider& right) const
	{
		return right.Intersects(*this);
	}

	bool Circle_Collider::Intersects(const Circle_Collider& right) const
	{
		//Get distance between objects
		float dist = MathHelper::GetDistance(m_Centre.x, m_Centre.y, right.m_Centre.x, right.m_Centre.y);
		return dist <= m_Radius + right.m_Radius;
	}

	bool Circle_Collider::Intersects(const Line_Collider& right) const
	{
		//Check for either point being inside the circle
		float distA = MathHelper::GetDistance(right.m_PointA.x, right.m_PointA.y, m_Centre.x, m_Centre.y);
		float distB = MathHelper::GetDistance(right.m_PointB.x, right.m_PointB.y, m_Centre.x, m_Centre.y);
		if (distA <= m_Radius || distB <= m_Radius)
			return true;
		
		//Get length of the line
		float length = right.GetLength();

		//Get the dot product of the line and the circle
		float dot = (((m_Centre.x - right.m_PointA.x) * (right.m_PointB.x)) + ((m_Centre.y-right.m_PointA.y) * (right.m_PointB.y-right.m_PointA.y))) / std::powf(length, 2.f);

		//Find the closest point on the line
		float closeX = right.m_PointA.x + (dot * ((right.m_PointB.x - right.m_PointA.x)));
		float closeY = right.m_PointA.y + (dot * ((right.m_PointB.y - right.m_PointA.y)));

		//Check if point is on the line
		distA = MathHelper::GetDistance(closeX, closeY, right.m_PointA.x, right.m_PointA.y);
		distB = MathHelper::GetDistance(closeX, closeY, right.m_PointB.x, right.m_PointB.y);
		
		//If the distances are equal enough (small buffer range), return collision true
		if (distA + distB >= length - 0.1f && distA + distB <= length + 0.1f)
			return true;

		//Lastly, get the distance from closest point and the circles centre and check with radius
		distA = MathHelper::GetDistance(closeX, closeY, m_Centre.x, m_Centre.y);
		return distA <= m_Radius;
	}

	bool Circle_Collider::Intersects(const Polygon_Collider& right) const
	{
		return false;
	}


	/////////////////////
	/// Line Collider ///
	/////////////////////

	//
	//Line Collider: Intersect Methods
	//

	bool Line_Collider::Intersects(const Rect_Collider& right) const
	{
		return right.Intersects(*this);
	}

	bool Line_Collider::Intersects(const Circle_Collider& right) const
	{
		return right.Intersects(*this);
	}

	bool Line_Collider::Intersects(const Line_Collider& right) const
	{
		//Calculate distances for intersection points
		float distA =
			((right.m_PointB.x - right.m_PointA.x) * (m_PointA.y - right.m_PointA.y) - (right.m_PointB.y - right.m_PointA.y) * (m_PointA.x - right.m_PointA.x)) /
			((right.m_PointB.y - right.m_PointA.y) * (m_PointB.x - m_PointA.x) - (right.m_PointB.x - right.m_PointA.x) * (m_PointB.y - m_PointA.y));

		float distB =
			((m_PointB.x - m_PointA.x) * (m_PointA.y - right.m_PointA.y) - (m_PointB.y - m_PointA.y) * (m_PointA.x - right.m_PointA.x)) /
			((right.m_PointB.y - right.m_PointA.y) * (m_PointB.x - m_PointA.x) - (right.m_PointB.x - right.m_PointA.x) * (m_PointB.y - m_PointA.y));

		//If both distances are between 0-1 range, then intersect true
		return (distA >= 0.f && distA <= 1.f && distB >= 0.f && distB <= 1.f);
	}

	bool Line_Collider::Intersects(const Polygon_Collider& right) const
	{
		return false;
	}

	////////////////////////
	/// Polygon Collider ///
	////////////////////////

	//
	//Polygon Collider: Intersect Methods
	//

	bool Polygon_Collider::Intersects(const Rect_Collider& right) const
	{
		unsigned next = 0;
		for (unsigned i(0); i < m_Points.size(); ++i)
		{
			next = i + 1;
			if (next == m_Points.size())
				next = 0;

			//Construct a line collider from current segment
			Line_Collider lineSeg;
			lineSeg.m_PointA = { m_Points[i].x, m_Points[i].y };
			lineSeg.m_PointB = { m_Points[next].x, m_Points[next].y };
			//If intersecting, return true
			if (right.Intersects(lineSeg))
				return true;
		}

		//No intersection detected
		return false;
	}

	bool Polygon_Collider::Intersects(const Circle_Collider& right) const
	{
		unsigned next = 0;
		for (unsigned i(0); i < m_Points.size(); ++i)
		{
			next = i + 1;
			if (next == m_Points.size())
				next = 0;

			//Construct a line collider from current segment
			Line_Collider lineSeg;
			lineSeg.m_PointA = { m_Points[i].x, m_Points[i].y };
			lineSeg.m_PointB = { m_Points[next].x, m_Points[next].y };
			//If intersecting, return true
			if (right.Intersects(lineSeg))
				return true;
		}

		//No intersection detected
		return false;
	}

	bool Polygon_Collider::Intersects(const Line_Collider& right) const
	{
		unsigned next = 0;
		for (unsigned i(0); i < m_Points.size(); ++i)
		{
			next = i + 1;
			if (next == m_Points.size())
				next = 0;

			//Construct a line collider from current segment
			Line_Collider lineSeg;
			lineSeg.m_PointA = { m_Points[i].x, m_Points[i].y };
			lineSeg.m_PointB = { m_Points[next].x, m_Points[next].y };
			//If intersecting, return true
			if (right.Intersects(lineSeg))
				return true;
		}

		//No intersection detected
		return false;
	}

	bool Polygon_Collider::Intersects(const Polygon_Collider& right) const
	{
		unsigned next = 0;
		for (unsigned i(0); i < m_Points.size(); ++i)
		{
			next = i + 1;
			if (next == m_Points.size())
				next = 0;

			//Construct a line collider from current segment
			Line_Collider lineSeg;
			lineSeg.m_PointA = { m_Points[i].x, m_Points[i].y };
			lineSeg.m_PointB = { m_Points[next].x, m_Points[next].y };
			//If intersecting, return true
			if (right.Intersects(lineSeg))
				return true;
		}

		return false;
	}

	void Polygon_Collider::CreateFromRect(float width, float height, Vec2 position, float rotation, Vec2 origin)
	{
		//Prep containers
		m_ModelShape.clear();
		m_Points.clear();
		m_ModelShape.reserve(4);
		m_Points.reserve(4);

		//PH for origin adjustment
		float xOrigin = 0.75f;
		float yOrigin = 0.75f;

		//Calculate x and y values with origin adjustments (if origin = 0.f, then this would be 0, 0, width, height)
		float aX = width * origin.x;
		float aY = height * origin.y;
		float bX = width * (1.f - origin.x);
		float bY = height * (1.f - origin.y);

		//Set vertices using origin adjusted points
		m_ModelShape.push_back({ -aX, -aY });	//Left-Top
		m_ModelShape.push_back({ bX, -aY });	//Right-Top
		m_ModelShape.push_back({ bX, bY });		//Right-Bot
		m_ModelShape.push_back({ -aX, bY });	//Left-Bot

		//Store new model in points
		for (auto& a : m_ModelShape)
			m_Points.push_back(a);

		//Store position and rotation
		m_Position = position;
		m_Rotation = rotation;

		//Regenerate point using new model shape
		UpdatePoints();
	}

	void Polygon_Collider::UpdatePoints()
	{
		//Construct transformation matrix
		DirectX::XMFLOAT4X4 transform;
		DirectX::XMStoreFloat4x4(&transform, DirectX::XMMatrixScaling(m_Scale.x, m_Scale.y, 0.f) * DirectX::XMMatrixRotationZ(m_Rotation) * DirectX::XMMatrixTranslation(m_Position.x , m_Position.y , 0.f));

		//Regenerate points
		DirectX::SimpleMath::Vector2::Transform(&m_ModelShape[0], m_ModelShape.size(), transform, &m_Points[0]);
	}

	////////////////////////////
	/// Shared Methods (WIP) ///
	////////////////////////////

	bool SAT_Check(Polygon_Collider& lPoly, Polygon_Collider& rPoly)
	{
		//Hold local pointers to each poly as we will be switching them round occasionally.
		Polygon_Collider* polyA = &lPoly;
		Polygon_Collider* polyB = &rPoly;

		//For each polygon being tested (which is always 2 in this case)
		for (unsigned i(0); i < 2; i++)
		{
			//Switch around check for each polygon
			if (i == 1)
			{
				polyA = &rPoly;
				polyB = &lPoly;
			}

			//For each side in polygonA
			for (int a(0); a < polyA->m_Points.size(); a++)
			{
				//Get the next vertice in the polygon (wraps to 0 if A == last vertice)
				int b = (a + 1) % polyA->m_Points.size();

				//Constructs the projected axis of the current line segment (the normal of that line)
				Vec2 projectedAxis =
				{
					-(polyA->m_Points[b].y - polyA->m_Points[a].y),
					polyA->m_Points[b].x - polyA->m_Points[a].x
				};

				//Work out the min and max points of the first polygon on the 1D axis
				float minR1 = INFINITY;
				float maxR1 = -INFINITY;
				for (unsigned p(0); p < polyA->m_Points.size(); p++)
				{
					float q = (polyA->m_Points[p].x * projectedAxis.x + polyA->m_Points[p].y * projectedAxis.y);
					minR1 = min(minR1, q);
					maxR1 = max(maxR1, q);
				}

				//Then the 2nd polygon
				float minR2 = INFINITY;
				float maxR2 = -INFINITY;
				for (unsigned p(0); p < polyB->m_Points.size(); p++)
				{
					float q = (polyB->m_Points[p].x * projectedAxis.x + polyB->m_Points[p].y * projectedAxis.y);
					minR2 = min(minR2, q);
					maxR2 = max(maxR2, q);
				}

				//If not overlap detected (similar to AABB detection) then collision cant be possible, so return false
				if (!(maxR2 >= minR1 && maxR1 >= minR2))
					return false;
			}
		}

		//Collision must be occuring as no fail state hit
		return true;
	}
}
