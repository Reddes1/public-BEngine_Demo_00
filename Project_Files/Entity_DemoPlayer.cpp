#include "Entity_DemoPlayer.h"

//Utils
#include "Utils/Utils_General.h"
#include "Utils/MathHelper.h"

//Engine Includes
#include "All_Managers.h"

//Modules
#include "Modules/Module_AnimatedSprite.h"
#include "Modules/Module_Box2D_Body.h"

//Project Includes
#include "Texture_Enums/BE_2DTestingTextureEnums.h"

void Entity_DemoPlayer::Update_Main(System& sys)
{
	//
	//Pre-Game Data
	//

	//Get copy of dt (prevent repetative access)
	float dt = sys.m_GameTime->DeltaTime();

	//Get sprite and body module
	Module_AnimatedSprite* spr = recast_static(Module_AnimatedSprite*, m_Modules[(int)ModuleIndexes::ANIMATED_SPRITE].get());
	Module_Box2D_RigidBody2D* body = recast_static(Module_Box2D_RigidBody2D*, m_Modules[(int)ModuleIndexes::RIGIDBODY].get());

	//This is the pending velocity changes to the player, based on inputs and states
	b2Vec2 pendingVel = { 0.f, 0.f };
	//This is the current/old velocity of the body as per this frame
	b2Vec2 oldVel = body->GetBody()->GetLinearVelocity();
	//Also keep track of any applied inpulses (applied after setting new velocity)
	b2Vec2 newImpulse = { 0.f, 0.f };

	//Flipped when an input is detected, allowing shared input behaviour to execute
	bool detectedInputH = false;
	bool detectedInputV = false;

	//
	//Input Process & Actions
	//

	{
		//
		//Pre-setup & pre-checks
		//

		//Get the current animation ID 
		unsigned animID = spr->GetAnimator().GetAnimationID();
		//Check for certain frames that lock x velocity
		if (animID != (unsigned)BE_TextureEnums::BE_2DTestingTextureFrames::TEMPLATE_PLAYER_LANDING_L && animID != (unsigned)BE_TextureEnums::BE_2DTestingTextureFrames::TEMPLATE_PLAYER_LANDING_R)
			m_IsMidHardLanding = false;

		/*
			Because Box2D physics are expressing gravity on the player, we want to make sure that the current Y velocity (which should include
			gravity's effect on the player) are applying to the pending velocity.
		*/
		pendingVel.y = oldVel.y;

		////////////////////
		/// Input Checks ///
		//////////////////// 

		/*
			A general input lock check. If the lock is put in place, ensure that any data is modified appropriate when setting it
			to prevent strange behaviour (like player moving based on velocity).
		*/
		if (!m_LockAllInputs)
		{

			//These are states that specifically shouldn't allow movement inputs
			bool disallowMovement = m_IsMidHardLanding;

			//
			//Horizontal Movement
			//

			//If allowed to move, check for inputs
			if (!disallowMovement)
			{


				//Check for Move Left Input
				if (sys.m_KBMMgr->IsKeyHeld(VK_A))
				{
					//Apply x velocity speed and update direction
					pendingVel.x = -m_BaseMS.x;
					m_FacingDirection = Directions4::LEFT;
					detectedInputH = true;
				}
				//Check for Move Right Input
				else if (sys.m_KBMMgr->IsKeyHeld(VK_D))
				{
					//Apply x velocity speed and update direction
					pendingVel.x = m_BaseMS.x;
					m_FacingDirection = Directions4::RIGHT;
					detectedInputH = true;
				}
				//We only want acceleration, with immediate deceleration so just cancel out speed + timer
				else
				{
				m_AccelElapsed = 0.f;
				}

				//If an input was detected then run some shared behaviour
				if (detectedInputH)
				{
					//If the player is mid-air, we want to instead decelerate
					if (m_IsMidAir)
						m_AccelElapsed = MathHelper::Clamp(m_AccelElapsed -= dt, 0.f, m_AccelElapsedMax);
					else
						m_AccelElapsed = MathHelper::Clamp(m_AccelElapsed += dt, 0.f, m_AccelElapsedMax);
					//Update speed modifier
					m_SpeedMod.x = MathHelper::Lerp(1.f, m_AccelPercentageMax, MathHelper::Clamp(m_AccelElapsed / m_AccelElapsedMax, 0.f, 1.f));
				}
			}
			//No inputs detected
			else
			{
			//Decelerate speed mod at standard rate
			m_AccelElapsed = MathHelper::Clamp(m_AccelElapsed -= (dt * m_DecelRate), 0.f, m_AccelElapsedMax);
			m_SpeedMod.x = MathHelper::Lerp(1.f, m_AccelPercentageMax, MathHelper::Clamp(m_AccelElapsed / m_AccelElapsedMax, 0.f, 1.f));
			}

			//
			//Vertical
			//

			//If allowed to move, check for inputs
			if (!disallowMovement)
			{
				//If pressed, able to jump, and not mid landing animation
				if (sys.m_KBMMgr->IsKeyPressedNoRepeat(VK_SPACE) && m_CanJump)
				{
					//If mid-hang
					if (m_IsWallHanging)
					{
						//Determine facing direction
						if (m_FacingDirection == Directions4::LEFT)
						{
							//Apply strong force away from the wall
							newImpulse = { 200.f, 175.f };
							m_FacingDirection = Directions4::RIGHT;
						}
						//Logically must be right (this may change later)
						else
						{
							//Apply strong force away from the wall
							newImpulse = { -200.f, 175.f };
							m_FacingDirection = Directions4::LEFT;
						}

						//Disable jump and update jump timer (it locks the player into the jump away)
						m_CanJump = false;
						m_IsMidAir = true;
						m_IsMidWallJump = true;
						//Reset the wall jump timer (this is checked against min timer later)
						m_WallJumpElapsed = 0.f;
					}
					//Regular jump action (off a ground surface)
					else
					{
						//Apply fixed upward velocity and signal flags
						pendingVel.y = m_BaseMS.y;
						m_IsGrounded = false;
						m_IsMidAir = true;
						m_CanJump = false;
					}
				}
			}
		}

		//
		//Post Checks
		//

		/*
			The follow section looks for reason to modify or override the pending velocity. This flag indicates what
			velocity to use, which is modified as required by the following section.
		*/
		bool applyNewVel = true;

		//If wallhanging, downtick timer and see if timer is done
		if (m_IsWallHanging && (m_HangTimeRemaining -= dt) < 0.f)
		{
			//Reset gravity (in essence allow the player to start falling again)
			body->GetBody()->SetGravityScale(m_DefaultGravity);

			//Set appropriate flags for new state
			m_IsWallHanging = false;
			m_AllowWallHang = false;
			m_CanJump = false;
		}

		//Mutually exclusive check to see if the player is mid jump, if the min time has passed, and if a horizontal input was detected
		if (m_IsMidWallJump)
		{
			if ((m_WallJumpElapsed += dt) >= m_WallJumpMin && detectedInputH)
			{
				//Disable mid wall jump flag
				m_IsMidWallJump = false;
				//Clean up elapsed
				m_WallJumpElapsed = 0.f;
			}
			//Timers not up and/or player hasn't input to breakout out of the wall jump so override new velocity
			else
				applyNewVel = false;
		}
		

		//
		//Finalising
		//
		
		//Apply movespeed mod, then any additional velocity on top of that (isn't effected by modifier by default)
		pendingVel.x *= m_SpeedMod.x;
		pendingVel.y *= m_SpeedMod.y;
		pendingVel += m_AdditionalVel;

		//Apply either the new velocity or the original
		if (applyNewVel)
			body->GetBody()->SetLinearVelocity(pendingVel);
		else
			body->GetBody()->SetLinearVelocity(oldVel);

		//Apply any impulses 
		body->GetBody()->ApplyLinearImpulseToCenter(newImpulse, true);
	}


	//
	//Post-Input Animation
	//

	if (pendingVel.x != 0.f && m_IsGrounded)
	{
		if (pendingVel.x < 0.f)
			spr->GetAnimator().SetAnimation((int)BE_TextureEnums::BE_2DTestingTextureFrames::TEMPLATE_PLAYER_RUN_L, true);
		else if (pendingVel.x > 0.f)
			spr->GetAnimator().SetAnimation((int)BE_TextureEnums::BE_2DTestingTextureFrames::TEMPLATE_PLAYER_RUN_R, true);
	}
	else if (pendingVel.x == 0.f && m_IsGrounded)
	{
		if (m_FacingDirection == Directions4::LEFT)
			spr->GetAnimator().SetAnimation((int)BE_TextureEnums::BE_2DTestingTextureFrames::TEMPLATE_PLAYER_IDLE_L, true);
		else
			spr->GetAnimator().SetAnimation((int)BE_TextureEnums::BE_2DTestingTextureFrames::TEMPLATE_PLAYER_IDLE_R, true);
	}


	if (pendingVel.y != 0.f && !m_IsGrounded && !m_IsWallHanging)
	{
		if (m_FacingDirection == Directions4::LEFT)
		{
			//If doing the opposite animation, branch to this animation instead of playing it normally, else start as normal
			if (spr->GetAnimator().GetAnimationID() == (unsigned)BE_TextureEnums::BE_2DTestingTextureFrames::TEMPLATE_PLAYER_JUMP_R)
				spr->GetAnimator().BranchAnimation((int)BE_TextureEnums::BE_2DTestingTextureFrames::TEMPLATE_PLAYER_JUMP_L, true, false, false, true);
			else
				spr->GetAnimator().SetAnimation((int)BE_TextureEnums::BE_2DTestingTextureFrames::TEMPLATE_PLAYER_JUMP_L, true, false, false, false);
		}
		else if (m_FacingDirection == Directions4::RIGHT)
		{
			//If doing the opposite animation, branch to this animation instead of playing it normally, else start as normal
			if (spr->GetAnimator().GetAnimationID() == (unsigned)BE_TextureEnums::BE_2DTestingTextureFrames::TEMPLATE_PLAYER_JUMP_L)
				spr->GetAnimator().BranchAnimation((int)BE_TextureEnums::BE_2DTestingTextureFrames::TEMPLATE_PLAYER_JUMP_R, true, false, false, true);
			else
				spr->GetAnimator().SetAnimation((int)BE_TextureEnums::BE_2DTestingTextureFrames::TEMPLATE_PLAYER_JUMP_R, true, false, false, false);
		}

	}


	//Update animator
	m_Modules[(int)ModuleIndexes::ANIMATED_SPRITE]->Update_Main(sys);
}

void Entity_DemoPlayer::Update_PrePhysics(System& sys)
{

}

void Entity_DemoPlayer::Update_PostPhysics(System& sys)
{
	//Physics sim done so update the sprite position with the current rigidbody position
	m_Modules[(int)ModuleIndexes::ANIMATED_SPRITE].get()->SyncModulePosition(m_Modules[(int)ModuleIndexes::RIGIDBODY].get());
}

void Entity_DemoPlayer::Update_PreRender(System& sys)
{
	//Submit to render group
	sys.m_GraphicsMgr->SubmitToRenderGroup(m_UtilityIDs.m_RenderGroupIndex, this);
}

void Entity_DemoPlayer::Render(System& sys)
{
	if (m_Flags.m_CanRender)
		m_Modules[(unsigned)ModuleIndexes::ANIMATED_SPRITE]->Render(sys);
}

void Entity_DemoPlayer::Render(System& sys, DirectX::SpriteBatch* batch)
{
	if (m_Flags.m_CanRender)
		m_Modules[(unsigned)ModuleIndexes::ANIMATED_SPRITE]->Render(sys, batch);
}

void Entity_DemoPlayer::OnCollisionEnter(Actor2D_Interface* otherActor, b2Fixture* fixture, b2Contact* contact)
{
	//Get the category of the inbound object (tells us what we are looking at on a broad level)
	int categoryID = recast_reint(Box2D_UserData_Interface*, fixture->GetBody()->GetUserData().pointer)->m_CategoryID;

	switch ((BE_Actor_Enums::CategoryIDs)categoryID)
	{
	case BE_Actor_Enums::CategoryIDs::STATIC_TERRAIN_TYPE_A:
		OnEnterCollision_TerrainA(otherActor, fixture, contact);
		break;
	}
}

void Entity_DemoPlayer::OnContinuedCollision(Actor2D_Interface* otherActor, b2Fixture* fixture, b2Contact* contact)
{

}

void Entity_DemoPlayer::OnCollisonExit(Actor2D_Interface* otherActor, b2Fixture* fixture, b2Contact* contact)
{
	//Get the category of the inbound object (tells us what we are looking at on a broad level)
	int categoryID = recast_reint(Box2D_UserData_Interface*, fixture->GetBody()->GetUserData().pointer)->m_CategoryID;
	
	switch ((BE_Actor_Enums::CategoryIDs)categoryID)
	{
	case BE_Actor_Enums::CategoryIDs::STATIC_TERRAIN_TYPE_A:
		OnExitCollision_TerrainA(otherActor, fixture, contact);
		break;
	}
}

bool Entity_DemoPlayer::RunOnceInit(System& sys)
{
	if (m_Flags.m_CanRunOnceInit)
	{
		//Add modules
		Module_AnimatedSprite* spr = AddNewModule<Module_AnimatedSprite>(std::string("Animated_Sprite"));
		Module_Box2D_RigidBody2D* body = AddNewModule<Module_Box2D_RigidBody2D>(std::string("RigidBody"));
		
		//
		//Sprite setup
		//

		spr->GetSpriteData().SetTexture(std::string(BE_2DTestingTexture_ALIAS), sys);

		//Set initial animation state
		spr->GetAnimator().SetAnimation((int)BE_TextureEnums::BE_2DTestingTextureFrames::TEMPLATE_PLAYER_IDLE_L, true);
		//Set initial position
		spr->GetSpriteData().m_Position =
		{
			sys.m_Blackboard->m_NativeWinX * 0.5f,
			sys.m_Blackboard->m_NativeWinY * 0.5f
		};

		//
		//Body Setup
		//

		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.fixedRotation = true;
		bodyDef.linearDamping = 0.1f;
		bodyDef.gravityScale = m_DefaultGravity;

		b2FixtureDef fixDef;
		fixDef.density = 10.f;
		fixDef.friction = 0.0f;
		fixDef.restitution = 0.0f;
		b2PolygonShape shape;
		shape.SetAsBox(
			Mgr_Box2DPhysics::PixelsToMetres(spr->GetSpriteData().GetFrameSizeX_Scaled() * 0.7f) * 0.5f,
			Mgr_Box2DPhysics::PixelsToMetres(spr->GetSpriteData().GetFrameSizeY_Scaled()) * 0.51f
		);
		fixDef.shape = &shape;

		body->AttachNewBody(
			bodyDef,
			sys.m_Box2DMgr->GetWorldData(std::string("Test_World_00"))->m_World,
			Box2D_UserData_Interface()
		);
		body->AttachNewFixture(fixDef);

		//Sync body/sprite positions
		body->SyncWithSpriteData(spr->GetSpriteData());

		//
		//Finalising
		//

		//Set render group ID
		m_UtilityIDs.m_RenderGroupIndex = (unsigned)BE_ManagerEnums::SpritebatchIndexes::MAIN_SCENE;

		//Close init flag
		m_Flags.m_CanRunOnceInit = false;
		return true;
	}

	//No init done
	return false;
}

void Entity_DemoPlayer::OnEnterCollision_TerrainA(Actor2D_Interface* otherActor, b2Fixture* fixture, b2Contact* contact)
{
	//
	//Data setup
	//
	
	//Get animator data
	SpriteAnimator& sprAnim = recast_static(Module_AnimatedSprite*, m_Modules[(int)ModuleIndexes::ANIMATED_SPRITE].get())->GetAnimator();
	//Get the players body from the module
	b2Body* body = recast_static(Module_Box2D_RigidBody2D*, m_Modules[(int)ModuleIndexes::RIGIDBODY].get())->GetBody();
	//Get current velocity as well (adjustments may need to be made
	b2Vec2 vel = body->GetLinearVelocity();

	//Need to capture what side of the contact data the players body sits on
	bool playerIsA = contact->GetFixtureA()->GetBody() == body;
	//If player == A then set terrain pointer as B, or vice versa
	b2Body* terrainBody = playerIsA ? terrainBody = contact->GetFixtureB()->GetBody() : terrainBody = contact->GetFixtureA()->GetBody();

	//Get the contact point normal (so we can determine the direction of the collision)
	b2WorldManifold wM;
	contact->GetWorldManifold(&wM);
	b2Vec2 normal = wM.normal;

	//Player may not be the first object in the contact data, so flip normals if not for logic
	if (!playerIsA)
		normal = -normal;

	//
	//Animation & State Logic
	//

	/*
		Check to see the collision point is below the player
	*/
	if (normal.y < 0.0f)
	{
		//Check facing and set appropriate 
		if (m_FacingDirection == Directions4::LEFT)
			sprAnim.SetAnimation((int)BE_TextureEnums::BE_2DTestingTextureFrames::TEMPLATE_PLAYER_LANDING_L, true, false, false, false);
		else
			sprAnim.SetAnimation((int)BE_TextureEnums::BE_2DTestingTextureFrames::TEMPLATE_PLAYER_LANDING_R, true, false, false, false);
		//Lock this animation in till it completes
		sprAnim.EnableAnimationLock();

		//Cancel out current X velocity as well (prevents future movement whilst animation/lock time plays out)
		vel.x = 0.f;
		//Set control flags (enable jump)
		m_CanJump = true;

		//Set state flags (grounded & landing)
		m_IsGrounded = true;
		m_IsMidHardLanding = true;
		m_AllowWallHang = true;
		//Force set what can be conflicting states
		m_IsWallHanging = false;
		m_IsMidAir = false;
	}
	//Check to see if the player is sliding down a wall and not grounded (check left then right)
	else if (normal.x < 0.0f && !m_IsGrounded && m_AllowWallHang)
	{
		//Set sliding/Hanging animation
		sprAnim.SetAnimation((int)BE_TextureEnums::BE_2DTestingTextureFrames::TEMPLATE_PLAYER_WALL_SLIDING_L, true, false, false, false);

		//Cancel out the Y velocity, disable gravity (for a hanging effect), and set hanging timer
		vel.y = 0.f;
		body->SetGravityScale(0.f);
		m_HangTimeRemaining = m_HangTimeMax;

		//Set control flags (enable jump)
		m_CanJump = true;
		m_AllowWallHang = false;

		//Set state flags
		m_IsWallHanging = true;
		m_IsMidAir = false;
	}
	else if (normal.x > 0.0f && !m_IsGrounded && m_AllowWallHang)
	{	//Set sliding animation
		sprAnim.SetAnimation((int)BE_TextureEnums::BE_2DTestingTextureFrames::TEMPLATE_PLAYER_WALL_SLIDING_R, true, false, false, false);

		//Cancel out the Y velocity, disable gravity (for a hanging effect), and set hanging timer
		vel.y = 0.f;
		body->SetGravityScale(0.f);
		m_HangTimeRemaining = m_HangTimeMax;

		//Set control flags (enable jump)
		m_CanJump = true;
		m_AllowWallHang = false;

		//Set state flags
		m_IsWallHanging = true;
		m_IsMidAir = false;
	}

	//
	//Post
	//

	//Set new velocity (may have been adjusted)
	body->SetLinearVelocity(vel);
}

void Entity_DemoPlayer::OnExitCollision_TerrainA(Actor2D_Interface* otherActor, b2Fixture* fixture, b2Contact* contact)
{
	//
	//Data setup
	//

	//Get spr module
	Module_AnimatedSprite* spr = recast_static(Module_AnimatedSprite*, m_Modules[(int)ModuleIndexes::ANIMATED_SPRITE].get());
	//Get the players body from the module
	b2Body* body = recast_static(Module_Box2D_RigidBody2D*, m_Modules[(int)ModuleIndexes::RIGIDBODY].get())->GetBody();
	//Get current velocity as well (adjustments may need to be made
	b2Vec2 vel = body->GetLinearVelocity();

	//Need to capture what side of the contact data the players body sits on
	bool playerIsA = contact->GetFixtureA()->GetBody() == body;
	//If player == A then set terrain pointer as B, or vice versa
	b2Body* terrainBody = playerIsA ? terrainBody = contact->GetFixtureB()->GetBody() : terrainBody = contact->GetFixtureA()->GetBody();

	//Get the contact point normal
	b2WorldManifold wM;
	contact->GetWorldManifold(&wM);
	b2Vec2 normal = wM.normal;

	//If the player ISN'T object A, we need to flip the normals around for following logic
	if (!playerIsA)
		normal = -normal;

	//If the player was hanging leaving collision with the object, do some checks to determine how to end
	if (m_IsWallHanging)
	{
		//If sliding was detected, do some shared behaviour
		bool endHang = false;

		//Check left/right walls
		if (normal.x < 0.f)
		{
			endHang = true;
			spr->GetAnimator().SetAnimation((int)BE_TextureEnums::BE_2DTestingTextureFrames::TEMPLATE_PLAYER_JUMP_R, true, false, false, false);
		}
		else if (normal.x > 0.f)
		{
			endHang = true;
			spr->GetAnimator().SetAnimation((int)BE_TextureEnums::BE_2DTestingTextureFrames::TEMPLATE_PLAYER_JUMP_L, true, false, false, false);
		}

		//If the player was hanging, do some post hang clean up
		if (endHang)
		{
			m_CanJump = false;
			m_IsWallHanging = false;
			m_AllowWallHang = false;
			m_IsMidAir = true;
			m_HangTimeRemaining = 0.f;
			body->SetGravityScale(m_DefaultGravity);
		}
	}
}
