//*********************************************************************************\\
// 
// Physics enabled prop object for use in BEngine demo.
//
//*********************************************************************************\\

#pragma once

#include "Actors/Actor2D_Interface.h"	//Parent

//Engine Includes
#include "box2d/b2_world.h"


class Entity_DemoProp : public Actor2D_Interface
{
public:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	//Some Enums defining how to set this object up (setup dictatesb vgv b bv
	enum class PropSetupID : unsigned
	{
		BLOCK_SMALL,
		BLOCK_MED,
		BLOCK_LARGE,

		BALL_SMALL,
		BALL_MED,
		BALL_LARGE,

		WALL,
		FLOOR
	};

	//Internal module indexes (assumes default setup) for quick access
	enum class ModuleIndexes : unsigned
	{
		SPRITE,
		RIGIDBODY
	};

	////////////////////
	/// Constructors ///
	////////////////////

	Entity_DemoProp() {}
	~Entity_DemoProp() {}

	/////////////////
	/// Overrides ///
	/////////////////

	//
	//Updates & Rendering Stages
	//

	void Update_Main(System& sys) override;
	void Update_PrePhysics(System& sys) override;
	void Update_PostPhysics(System& sys) override;
	void Update_PreRender(System& sys) override;

	void Render(System& sys) override;
	void Render(System& sys, DirectX::SpriteBatch* batch) override;

	//
	//Callbacks & Responses
	//

	//What OnColision functions are called depends on the Contact Listener being used (fulfil to needs)
	void OnCollisionEnter(Actor2D_Interface* otherActor) override { }
	void OnContinuedCollision(Actor2D_Interface* otherActor) override { }
	void OnCollisonExit(Actor2D_Interface* otherActor) override { }

	void OnCollisionEnter(Actor2D_Interface* otherActor, b2Fixture* fixture) override { }
	void OnContinuedCollision(Actor2D_Interface* otherActor, b2Fixture* fixture) override { }
	void OnCollisonExit(Actor2D_Interface* otherActor, b2Fixture* fixture) override { }

	void OnBodyDestroyed(b2Body* body) { }
	void OnFixtureDestroyed(b2Fixture* fixture) { }
	void OnJointDestroyed(b2Joint* joint) { }

	//
	//Setups & Resets
	//

	//Simply add expected modules
	bool RunOnceInit(System& sys) override;

	void SoftReset(System& sys) override { }
	void HardReset(System& sys) override { }

	//
	//Utilities
	//

	//////////////////
	/// Operations ///
	//////////////////

	//
	//Setup
	//

	/*
		Run a setup type based on given enum (adds modules if not already added).
		Assumes expected texture is loaded (as per demo setup), modify otherwise.
	*/
	bool InitProp(PropSetupID id, b2World* targetWorld);

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