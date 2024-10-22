#include "Module_Box2D_Body.h"

//Utilities
#include "Utils//Utils_General.h"
#include "Utils/Utils_Debug.h"
#include "Utils/Utils_Box2D.h"

//Engine Includes
#include "Actors/Actor2D_Interface.h"
#include "Managers/Mgr_Box2DPhysics.h"

//Modules
#include "Module_Sprite.h"
#include "Module_AnimatedSprite.h"

Module_Box2D_RigidBody2D::Module_Box2D_RigidBody2D(Actor2D_Interface* actor, b2Body* body)
	:Module_Interface(actor, std::string("Nameless_RigidBody"), ModuleTypeID::BOX2D_RIGIDBODY)
{
	msg_assert(body, "Module_Box2D_RigidBody2D(): Null b2Body passed in constructor!");
	m_Body = body;
	
	//Check that user data is present, and if not create basic user data
	if (m_Body->GetUserData().pointer == 0)
	{
		//Its not strictly an error if there is no data, but inform regardless via dbout
		std::string msg;
		msg += "No UserData set with Box2D_RigidBody Module b2Body. Actor Unique ID: " + std::to_string(m_Actor->GetUniqueGameID());
		msg += ".\nSetting default UserData.";
		DBOUT(msg);

		Box2D_UserData_Interface* data = new Box2D_UserData_Interface(m_Actor);
		m_Body->GetUserData().pointer = reinterpret_cast<uintptr_t>(data);
	}

	//Extra check here if needed: Check that type is at least of interface type
}

Module_Box2D_RigidBody2D::Module_Box2D_RigidBody2D(Actor2D_Interface* actor, std::string& name, b2Body* body)
	:Module_Interface(actor, name, ModuleTypeID::BOX2D_RIGIDBODY)
{
	msg_assert(body, "Module_Box2D_RigidBody2D(): Null b2Body passed in constructor!");
	m_Body = body;

	//Check that user data is present, and if not create basic user data
	if (m_Body->GetUserData().pointer == 0)
	{
		//Its not strictly an error if there is no data, but inform regardless via dbout
		std::string msg;
		msg += "No UserData set with Box2D_RigidBody Module b2Body. Actor Unique ID: " + std::to_string(m_Actor->GetUniqueGameID());
		msg += ".\nSetting default UserData.";
		DBOUT(msg);

		Box2D_UserData_Interface* data = new Box2D_UserData_Interface(m_Actor);
		m_Body->GetUserData().pointer = reinterpret_cast<uintptr_t>(data);
	}
}

Module_Box2D_RigidBody2D::~Module_Box2D_RigidBody2D()
{
	//Release body
	ReleaseBox2DBody(m_Body);
}

void Module_Box2D_RigidBody2D::ReSyncWithActor(Actor2D_Interface* actor)
{
	//Call default for first bit (does error check)
	Module_Interface::ReSyncWithActor(actor);

	//If there is a body we need to resync its pointer as well
	if (m_Body)
		reinterpret_cast<Box2D_UserData_Interface*>(m_Body->GetUserData().pointer)->m_AssociatedActor = actor;
}

void Module_Box2D_RigidBody2D::SyncModulePosition(Module_Interface* otherMod)
{
	if (!otherMod)
	{
		msg_assert(false, "SyncModulePosition(): No valid pointer passed!");
		return;
	}

	//Get type from the mod, recast and seek data as required
	switch (otherMod->GetType())
	{
	case ModuleTypeID::SPRITE:
	{
		//Call existing function
		SyncWithSpriteData(recast_static(Module_Sprite*, otherMod)->GetSpriteData());
	}
	break;
	case ModuleTypeID::ANIMATED_SPRITE:
	{
		//Call existing function
		SyncWithSpriteData(recast_static(Module_AnimatedSprite*, otherMod)->GetSpriteData());
	}
	break;

	//TBD: Other appropriate modules added

	}
}

b2Fixture* Module_Box2D_RigidBody2D::AttachNewFixture(b2FixtureDef& fixtureDef)
{
	//Create a bog standard container and pass it through to the regular call, passing back the result
	return AttachNewFixture(fixtureDef, Box2D_UserData_Interface());
}

void Module_Box2D_RigidBody2D::SyncWithSpriteData(SpriteData& sprData)
{
	//Get sprite position and convert it
	b2Vec2 pos =
	{
		Mgr_Box2DPhysics::PixelsToMetres(sprData.m_Position.x),
		-Mgr_Box2DPhysics::PixelsToMetres(sprData.m_Position.y)
	};
	//Set new transform (added inverted rotation)
	m_Body->SetTransform(pos, -sprData.m_Rotation);
}

Box2D_UserData_Interface* Module_Box2D_RigidBody2D::GetBodyUserData()
{
	if (m_Body)
		return recast_reint(Box2D_UserData_Interface*, m_Body->GetUserData().pointer);

	msg_assert(false, "GetBodyUserData(): Body not created, cannot return data!");
	return nullptr;
}
