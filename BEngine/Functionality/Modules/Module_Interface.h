//*********************************************************************************\\
// 
// WIP emulation of the Unreal Component system. In essence, any functionality tied
// to an Actor2D_Interface derived class that be defined in a module (like sprites)
// would be a module added to the interface and then accessed as required.
//
//*********************************************************************************\\

#pragma once

//Library Includes
#include <string>
#include <memory>
#include "SpriteBatch.h"

//Forward Declarations
class Actor2D_Interface;
struct System;

class Module_Interface
{
public:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	/*
		Like the Actor_Interface, each individual module may have some behaviour that requires general,
		granular control, so here are some defined flags.
	*/
	struct Flags
	{
		Flags()
			:m_IsActive(true), m_CanUpdate(true), m_CanRender(true)
		{}

		//Is the object active (broad state flag, no actionable behaviour allowed)?
		bool m_IsActive : 1;
		//Broad update enable flag
		bool m_CanUpdate : 1;
		//Broad render enable flag
		bool m_CanRender : 1;
		//Padding (Pending new flags)
		bool m_Padding04 : 1;
		bool m_Padding03 : 1;
		bool m_Padding02 : 1;
		bool m_Padding01 : 1;
		bool m_Padding00 : 1;
	};

	/*
		These define the different types of functionality modules available for use and their IDs.
	*/
	enum class ModuleTypeID
	{
		UNDEFINED = -1,

		//
		//Visuals
		//
		
		SPRITE,
		ANIMATED_SPRITE,

		//
		//Physics
		//

		BOX2D_RIGIDBODY,

		//
		//UI
		//

		UI_MOUSE_COLLIDER,
		UI_SF_STRING
	};


	////////////////////
	/// Constructors ///
	////////////////////

	Module_Interface(Actor2D_Interface* actor)
		:m_Actor(actor), m_Name("Nameless Module")
	{}
	Module_Interface(Actor2D_Interface* actor, const std::string& name)
		:m_Actor(actor), m_Name(name)
	{}

	virtual ~Module_Interface() {}

	////////////////
	/// Virtuals ///
	////////////////

	//
	//Updates & Rendering Stages
	//

	/*
		Like the mode and actors update and render stages, each module may have a purpose in
		any number of them so provide an optional window to each of them for the module to 
		define as neccesary.
	*/
	virtual void Update_Main(System& sys) {}
	virtual void Update_PrePhysics(System& sys) {}
	virtual void Update_Physics(System& sys) {}
	virtual void Update_PostPhysics(System& sys) {}
	virtual void Update_PreRender(System& sys) {}
	virtual void Render(System& sys) {}
	virtual void Render(System& sys, DirectX::SpriteBatch* batch) {}

	//
	//Utilities
	//

	/*
		There are some cases where the internal actor pointer of the module can become invalid, 
		so this can be called by the owning actor to re-sync the pointer as required.
		This can be overridden if addition use of the actor pointer is needed (such as Box2D UserData)
	*/
	virtual void ReSyncWithActor(Actor2D_Interface* actor);

	/*
		As most modules have some level of positioning (especially in regard to sprites & UI elements),
		and often need to align in position somehow (such overlapping a UI collider with a UI element).
		This call does that, with the module themselves assessing how to manage that.

		This call specifically adjusts the position element of the module, and not any offsetting (if present).
		As a general rule, implementations do/should take data from the passed module and apply it to the called
		module.
	*/
	virtual void SyncModulePosition(Module_Interface* otherMod) {}

	//////////////////
	/// Operations ///
	//////////////////

	//Binds an actor to this module (this should be the owning actor)
	bool BindActor(Actor2D_Interface* actor);

	/////////////////
	/// Accessors ///
	/////////////////

	const std::string& GetName() { return m_Name; }
	ModuleTypeID GetType() { return m_TypeID; }
	Flags& GetFlags() { return m_Flags; }

protected:

	////////////////////
	/// Constructors ///
	////////////////////
		
	//Protected constructor to allow derived modules to set type properly.
	Module_Interface(Actor2D_Interface* actor, const std::string& name, ModuleTypeID type)
		:m_Actor(actor), m_Name(name), m_TypeID(type)
	{}

	////////////
	/// Data ///
	////////////

	//Additional, verbose identifier. Can be unique if name is unique amongst other modules held by the actor
	std::string m_Name = "Nameless Module";	
	//Type indentifer that should be defined by the derived class. See ModuleTypeIDs for defined types
	ModuleTypeID m_TypeID = ModuleTypeID::UNDEFINED;
	//Binding pointer to the actor using the module
	Actor2D_Interface* m_Actor = nullptr;
	//Module flags
	Flags m_Flags;
};
