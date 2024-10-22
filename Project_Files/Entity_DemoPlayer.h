//*********************************************************************************\\
// 
// Physics-enabled platforming player character class for BEngine demo.
//
//*********************************************************************************\\

#pragma once

#include "Actors/Actor2D_Interface.h"	//Parent

//Library Includes
#include "box2d/b2_math.h"

class Entity_DemoPlayer : public Actor2D_Interface
{
public:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////


	//Internal module indexes
	enum class ModuleIndexes
	{
		ANIMATED_SPRITE,
		RIGIDBODY
	};

	////////////////////
	/// Constructors ///
	////////////////////

	Entity_DemoPlayer() {}
	~Entity_DemoPlayer() {}

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

	void OnCollisionEnter(Actor2D_Interface* otherActor, b2Fixture* fixture, b2Contact* contact) override;
	void OnContinuedCollision(Actor2D_Interface* otherActor, b2Fixture* fixture, b2Contact* contact) override;
	void OnCollisonExit(Actor2D_Interface* otherActor, b2Fixture* fixture, b2Contact* contact) override;

	void OnBodyDestroyed(b2Body* body) { }
	void OnFixtureDestroyed(b2Fixture* fixture) { }
	void OnJointDestroyed(b2Joint* joint) { }

	//
	//Setups & Resets
	//

	//Setup modules for use
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
	//Custom Behaviour Here!
	//

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
	//Update_Main Stages
	//

	//
	//OnEnter Collision Functions
	//

	//Resolves TerrainA collision (large, static terrain)
	void OnEnterCollision_TerrainA(Actor2D_Interface* otherActor, b2Fixture* fixture, b2Contact* contact);

	//
	//OnExit Collision Functions
	//

	//Resolves TerrainA collision (large, static terrain)
	void OnExitCollision_TerrainA(Actor2D_Interface* otherActor, b2Fixture* fixture, b2Contact* contact);


	////////////
	/// Data ///
	////////////

	//
	//General Data
	//

	//Current facing direction (informed by inputs/conditions of collision events)
	Directions4 m_FacingDirection = Directions4::LEFT;

	//
	//Movement 
	//

	//Players base movespeed on input (i.e. starting speed before other modifiers)
	Vec2 m_BaseMS = { 10.f, 12.5f };
	//Serves as percentage modifier on the base speed (where 1.0 = 100% speed, 0.5 = 50% etc)
	Vec2 m_SpeedMod = { 1.f, 1.f };
	//This additional, raw velocity on top of existing velocity (allowing for deceleration, acceleration in a raw form)
	b2Vec2 m_AdditionalVel = { 0.f, 0.f };
	//Track the number of jumps available to the player (NYI - See m_CanJump)
	short m_RemainingJumps = 0;

	//As mass doesn't effect speed of fall, we adjust the gravity effect instead
	float m_DefaultGravity = 1.5f;

	//
	//Elapsed Timers + Max Caps
	//

	//Maximum % increase in movespeed (at least on acceleration)
	float m_AccelPercentageMax = 1.5f;

	//Tracks the current acceleration timer (or deceleration)
	float m_AccelElapsed = 0.f;
	//Time required to hit max acceleration
	float m_AccelElapsedMax = 2.f;
	//The maximum acceleration cap (as a raw value, not multiplier)
	float m_AccelCap = 5.f;
	//Modifer on the deceleration rate
	float m_DecelRate = 2.f;

	//Tracks the remaining time allow to hang on a wall if currently hanging on a wall
	float m_HangTimeRemaining = 0.f;
	//Max hanging allowed
	float m_HangTimeMax = 1.f;
	
	//Minimum time that the player is locked into wall jump
	float m_WallJumpMin = 0.5f;
	//Current wall jump timer
	float m_WallJumpElapsed = 0.f;

	//
	//Action + Control Flags
	//

	//General Control Lock
	bool m_LockAllInputs = false;
	//Specific lock for movement inputs
	bool m_LockMovement = false;

	//Check for it the player can jump on input
	bool m_CanJump = true;

	//Tracks if the player is currently hanging of a piece of terrain/big enough object
	bool m_IsWallHanging = false;
	//Tracks if the player is on solid ground (terrain, platorm, large enough object etc)
	bool m_IsGrounded = false;
	//Tracks if the player is current mid-air (mid-air in general, regardless of state)
	bool m_IsMidAir = false;
	//Tracks if the player is mid hard landing (i.e. locked into an animation)
	bool m_IsMidHardLanding = false;
	//Tracks if the player is mid-wall jump (jumping away from a wall)
	bool m_IsMidWallJump = false;

	//Track if the player is able to hang on to a suitable surface currently (would be be re-enabled on landing on ground)
	bool m_AllowWallHang = false;
};