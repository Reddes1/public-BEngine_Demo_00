//*********************************************************************************\\
// 
// Wrapper module for Box2D body. Acts as the physics body for an actor
// Wrapping module for Box2D b2Body functionality + utilities to form a RigidBody
// class. Specifically manages a single b2Body pointer that is assigned to single
// b2World (with pointer to that world held).
//
//*********************************************************************************\\

#pragma once

#include "Module_Interface.h"

//Utilties
#include "Utils/Utils_Box2D.h"

//Library Includes
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"

//Engine Includes
#include "Types/BE_SharedTypes.h"
#include "Types/Box2D_UserData_Interface.h"

class Module_Box2D_RigidBody2D : public Module_Interface
{
public:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	//Box2D doesn't provide b2shape identifiers by default, so define some of them here
	enum class b2ShapeID : unsigned
	{
		B2_CIRCLE,
		B2_POLYBOX,
		B2_POLYSHAPE,
		B2_EDGE
	};

	////////////////////
	/// Constructors ///
	////////////////////

	/*
		TO-DO:

		Constructors MUST in some form take in a UserData interface or derr

	*/

	//Typical baseline init, where b2Body isn't set (see init options)
	Module_Box2D_RigidBody2D(Actor2D_Interface* actor)
		:Module_Interface(actor, std::string("Nameless_RigidBody"), ModuleTypeID::BOX2D_RIGIDBODY)
	{ }
	//Extention of the above, just with a verbose ID given
	Module_Box2D_RigidBody2D(Actor2D_Interface* actor, std::string& name)
		:Module_Interface(actor, name, ModuleTypeID::BOX2D_RIGIDBODY)
	{ }
	//Takes over management of existing b2Body pointer
	Module_Box2D_RigidBody2D(Actor2D_Interface* actor, b2Body* body);
	//Like above, just with a verbose ID given
	Module_Box2D_RigidBody2D(Actor2D_Interface* actor, std::string& name, b2Body* body);

	~Module_Box2D_RigidBody2D();

	//////////////////
	/// Overrrides ///
	//////////////////

	//
	//Updates & Rendering Stages
	//

	void Update_Main(System& sys) override {}
	void Update_PrePhysics(System& sys) override {}
	void Update_Physics(System& sys) override {}
	void Update_PostPhysics(System& sys) override {}
	void Update_PreRender(System& sys) override {}
	void Render(System& sys) override {}

	//
	//Utilities
	//

	void ReSyncWithActor(Actor2D_Interface* actor) override;

	void SyncModulePosition(Module_Interface* otherMod) override;

	//////////////////
	/// Operations ///
	//////////////////

	//
	//Inits & Setups
	//

	/*
		Performs a full setup of a new b2body from the given definition into the target world.
		User Data must be derived from Box2D_UserData_Interface (is compile time checked),
		with the internal actor pointer set during this setup (no other data is modified).

		If a body currently exists, it is released alongside its attachments.
	*/
	template<class USER_DATA_TYPE>
	b2Body* AttachNewBody(b2BodyDef& bodyDef, b2World& world, const USER_DATA_TYPE& data);

	/*
		
	*/
	template <class USER_DATA_TYPE>
	b2Fixture* AttachNewFixture(b2FixtureDef& fixtureDef, const USER_DATA_TYPE& data);
	/*
		Alternative call that automatically attaches a default user data container to the fixture.
		This is require due to assumptions made in Contact Listener logic (this will work with engine
		default listeners).
	*/
	b2Fixture* AttachNewFixture(b2FixtureDef& fixtureDef);

	//
	//Sync Utilties
	//

	/*
		Syncronises the current body data (Namely Position and Rotation) with the given sprite data.
		Does all the neccesary conversions via Mgr_Box2D static functions.
	*/
	void SyncWithSpriteData(SpriteData& sprData);


	/////////////////
	/// Accessors ///
	/////////////////

	b2Body* GetBody() { return m_Body; }

	/*
		Return the interface pointer of the user data attached to the body.
		If a derived type, see TypeID for additional recasting.
	*/
	Box2D_UserData_Interface* GetBodyUserData();

protected:

	//////////////////
	/// Operations ///
	//////////////////

	////////////
	/// Data ///
	////////////

	//Main body pointer (the memory allocation is handled by the associated world)
	b2Body* m_Body = nullptr;
};

template<class USER_DATA_TYPE>
inline b2Body* Module_Box2D_RigidBody2D::AttachNewBody(b2BodyDef& bodyDef, b2World& world, const USER_DATA_TYPE& data)
{
	//If existing body present, output debug info and release resources
	if (m_Body)
	{
		std::string str;
		str += "Releasing RigidBody, Actor Data: \n";
		m_Actor->GetDebugStr_CoreIDs(str);
		DBOUT(str);

		//Now release body (releases body, all attachments and each user data allocation on each)
		ReleaseBox2DBody(m_Body);
	}

	//Use util call to do bulk of the setup work
	m_Body = SetupB2Body(bodyDef, world, data);

	if (m_Body)
	{
		//Final touch is set the user data actor pointer
		reinterpret_cast<Box2D_UserData_Interface*>(m_Body->GetUserData().pointer)->m_AssociatedActor = m_Actor;
	}
	else
	{
		assert(false);
		return nullptr;
	}
	

	return m_Body;
}

template<class USER_DATA_TYPE>
inline b2Fixture* Module_Box2D_RigidBody2D::AttachNewFixture(b2FixtureDef& fixtureDef, const USER_DATA_TYPE& data)
{
	//Pre-checks
	assert(m_Body);

	//Use util for main setup (creates fixture with body, allocates and assigns data to fixture)
	b2Fixture* nFix = SetupB2Fixture(fixtureDef, *m_Body, data);

	/*
		Sets the actor pointer for user data. This is technically a redundant action as the body user data
		is accessable via the fixture (which should have this set), but as the same data interface is being used
		set this as a back up.
	*/
	reinterpret_cast<Box2D_UserData_Interface*>(nFix->GetUserData().pointer)->m_AssociatedActor = m_Actor;

	//Return fixture for any addition setup
	return nFix;
}
