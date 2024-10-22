//*********************************************************************************\\
// 
// UserData interface for use with b2Body, fixtures or anyother b2 type
// that allows UserData to be set. The basic structure is defined to require a 
// pointer to the owning actor (for callbacks) + some common data. Derive 
// different structs to define additional data (See m_DataType on how to define
// and cast appropriately). 
// 
//*********************************************************************************\\

#pragma once

//Library Includes
#include <string>
#include <pointer.h>
#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"

//Engine Includes
#include "Actors/Actor2D_Interface.h"

class Box2D_UserData_Interface
{
public:

	////////////////////
	/// Constructors ///
	////////////////////

	Box2D_UserData_Interface() {}
	Box2D_UserData_Interface(Actor2D_Interface* actor);
	virtual ~Box2D_UserData_Interface() {}



	////////////
	/// Data ///
	////////////

	//Verbose description of the user data
	std::string m_Name = "Nameless_Type";

	/*
		Practically speaking, every contact listener is going to need to callback an actors collision functionality,
		and should be able to do that without slowing down to determine the user data type so store a pointer to it here.
		Module_Box2D_RigidBody2D is configured as such that anytime a body or component is added that this be set
		automatically so this doesn't need defining otherwise.
	*/
	Actor2D_Interface* m_AssociatedActor = nullptr;

	/*
		This indicates what data type this for casting purposes (to avoid type checks).
		As this is project specfic, this should be defined elsewhere with -1 reserved for interface.
	*/
	int m_DataTypeID = -1;

	/*
		This is broad category ID that can be used to identify the object quickly. This has similar
		usages as the Actor_Interface version where this could signify a Player, Enemy, Terrain etc.,
		so long as application of it is uniform.
	*/
	int m_CategoryID = -1;
};

