//*********************************************************************************\\
// 
// Collection of Box2D orientated utilities and functions for use within the
// engine.
//
//*********************************************************************************\\

#pragma once

//Library Includes
#include "box2d/box2d.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"

//Forward Declarations (Engine)
class Actor2D_Interface;		 //Game main actor class
class Box2D_UserData_Interface; //b2 class UserData interface
//Library Declarations (Box2D)
class b2Body;


//================================================================================\\
// Resource Setup
//================================================================================\\

/*
	Templated function that manages the creation of the b2Body in target world and
	setups the allocation of user data for that body. Runs compile time checks
	that inbound template type is derived from Box2D_UserData_Interface.
*/
template<class USER_DATA_TYPE>
inline b2Body* SetupB2Body(b2BodyDef& def, b2World& world, const USER_DATA_TYPE& data)
{
	//Static type check to ensure proper type being passed in
	static_assert(std::is_base_of<Box2D_UserData_Interface, USER_DATA_TYPE>::value,
		"Data type being passed is not derived from Box2D_UserData_Interface!");

	//Create new allocated data via inbound
	USER_DATA_TYPE* nData = new USER_DATA_TYPE(data);

	//Create new body through world and give ownership of the user data to the body
	b2Body* nBody = world.CreateBody(&def);
	if(nBody)
		nBody->GetUserData().pointer = reinterpret_cast<uintptr_t>(nData);
	//World failed to create body
	else
	{
		assert(false);
		return nullptr;
	}

	//Return body for use (ideally with module)
	return nBody;
}

template<class USER_DATA_TYPE>
inline b2Fixture* SetupB2Fixture(b2FixtureDef& def, b2Body& body, const USER_DATA_TYPE& data)
{
	//Static type check to ensure proper type being passed in
	static_assert(std::is_base_of<Box2D_UserData_Interface, USER_DATA_TYPE>::value,
		"Data type being passed is not derived from Box2D_UserData_Interface!");

	//Create new allocated data via inbound
	USER_DATA_TYPE* nData = new USER_DATA_TYPE(data);

	//Create new fixture out of def
	b2Fixture* nFix = body.CreateFixture(&def);
	//Set user data for new fixture
	nFix->GetUserData().pointer = reinterpret_cast<uintptr_t>(nData);

	return nFix;
}


//================================================================================\\
// Resource Clean-up
//================================================================================\\

/*
	Provides clean-up for a b2Body from target world, by cleaning up any
	fixture user data, the body user data, and then finally removing the body 
	from the world.
*/
void ReleaseBox2DBody(b2Body* body);
