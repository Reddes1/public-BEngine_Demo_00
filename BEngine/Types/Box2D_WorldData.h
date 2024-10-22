//*********************************************************************************\\
// 
// Wrapper structure for hosting a Box2D World resource, as well as associated
// listener(s).
// 
// Any listeners with custom behaviour beyond virtual overrides must be accessed
// deliberately and directly.
//
//*********************************************************************************\\

#pragma once

//Libary Includes
#include <memory>

#include "box2d/b2_world.h"
#include "box2d/b2_contact.h"


class Box2D_WorldData
{
public:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	////////////////////
	/// Constructors ///
	////////////////////

	//Default construtor using internal defaults
	Box2D_WorldData();
	//Takes in iteration and gravity data (still uses default contact listener)
	Box2D_WorldData(b2Vec2& initGravity, int velIters, int posIters);
	//Takes in iteration and gravity data, as well as taking over a custom contact listener resource (must already be allocated)
	Box2D_WorldData(b2Vec2& initGravity, int velIters, int posIters, b2ContactListener* cListener);

	//////////////////
	/// Operations ///
	//////////////////

	//Starts the world step/simulation using current iteration counts
	void StartWorldStep(float dt) { m_World.Step(dt, m_VelocityIters, m_PositionIters); }
	//Starts the world step/simulation using given iteration values (overriding internal)
	void StartWorldStep(float dt, int velIters, int posIters) { m_World.Step(dt, velIters, posIters); }

	/////////////////
	///	Accessors ///
	/////////////////
	


	////////////
	/// Data ///
	////////////

	//World Data
	b2World m_World;

	//Iteration counts for world simulation (mirrors recommended defaults)
	int m_VelocityIters = 6;
	int m_PositionIters = 2;

	//Default gravity (can he used to reset world to a norm if gravity is altered)
	b2Vec2 m_DefaultGravity = { 0.0f, -10.f };

private:

	/*
		Called during simulation, allowing for custom resolution between actors associated with
		with the b2 types in the collision event. See ContactListener_BE_Default
	*/
	std::shared_ptr<b2ContactListener> m_ContactListener;
	/*
		Additional listener type that looks for destruction message during simulation,
		aloowing callbacks to associated actors like contact listener. Default type NYI.
	*/
	std::shared_ptr<b2DestructionListener> m_DestructionListener;
};