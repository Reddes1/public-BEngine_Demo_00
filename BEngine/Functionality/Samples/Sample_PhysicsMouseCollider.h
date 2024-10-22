//*********************************************************************************\\
// 
// Basic physics class that tracks an invisible fixture at the current mouse
// position to exert behaviours on the environment like any other moving force.
//
//*********************************************************************************\\

#pragma once

#include "Actors/Actor2D_Interface.h"	//Parent

#include "box2d/box2d.h"

class Sample_PhysicsMouseCollider : public Actor2D_Interface
{
public:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	//
	//Custom Data Here!
	//

	////////////////////
	/// Constructors ///
	////////////////////

	//Adds modules (must call init with world pointer however)
	Sample_PhysicsMouseCollider();
	~Sample_PhysicsMouseCollider() {}

	/////////////////
	/// Overrides ///
	/////////////////

	//
	//Updates & Rendering Stages
	//

	void Update_Main(System& sys) override;
	void Update_PrePhysics(System& sys) override { }
	void Update_PostPhysics(System& sys) override { }
	void Update_PreRender(System& sys) override { }

	void Render(System& sys) override { }
	void Render(System& sys, DirectX::SpriteBatch* batch) override { }

	//
	//Callbacks & Responses
	//

	void OnCollisionEnter(Actor2D_Interface* otherActor) override { }
	void OnContinuedCollision(Actor2D_Interface* otherActor) override { }
	void OnCollisonExit(Actor2D_Interface* otherActor) override { }

	void OnBodyDestroyed(b2Body* body) override { }
	void OnFixtureDestroyed(b2Fixture* fixture) override { }
	void OnJointDestroyed(b2Joint* joint) override { }

	//
	//Utilities
	//

	bool RunOnceInit(System& sys) override { return false; }
	void SoftReset(System& sys) override { }
	void HardReset(System& sys) override { }

	//////////////////
	/// Operations ///
	//////////////////

	//
	//Initialisers
	//

	//Initialises collider to target world using default fixture parameters (pass radius in 
	bool Init(b2World* world, float colliderRadius);

	/////////////////
	/// Accessors ///
	/////////////////

	//
	//Custom Behaviour Here!
	//

private:

	//////////////////
	/// Operations ///
	//////////////////
	
	//
	//Custom Behaviour Here!
	//

	////////////
	/// Data ///
	////////////

	//
	//Custom Data Here!
	//

};