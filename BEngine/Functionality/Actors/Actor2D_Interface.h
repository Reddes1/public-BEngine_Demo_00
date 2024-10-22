//*********************************************************************************\\
// 
// Core engine interface class that all (or nearly all) 2D game objects should be
// derived from. Provides basic universal data needed for basic actor functionality
// through Sprite, Animator, IDs and other general data.
// See Actor2D_Interface_Extensions for adding additional behaviour to this class
// specific to game requirements.
//
//*********************************************************************************\\

#pragma once

//Utilities
#include "Utils/Utils_D3D.h"		//D3D Types + Utils

//Engine Includes
#include "Types/BE_ReturnCodes.h"

//Module Includes
#include "Modules/Module_Interface.h"

//Foward Declarations
struct System;			//Must define this type (game + target managers) in .cpp files (See Include_SystemTypes.h)

//From Box2D library, use in collision callbacks
class b2Body;
class b2Fixture;
class b2Joint;
class b2Contact;


class Actor2D_Interface
{
public:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	/*
		Provides granular control over aspects of the actor. These don't directly plug into modules
		that might match the description, but the derived class can use them as control mechanisms.
	*/
	struct Flags
	{
		Flags()
			:m_IsActive(true), m_CanUpdate(true), m_CanRender(true), m_UseRenderGrouping(true),
			m_CanRunOnceInit(true), m_CanAcceptInputs(true)
		{}

		//
		//General Flags
		//

		//Is the object active (broad state flag, no actionable behaviour allowed)?
		bool m_IsActive : 1;
		//Broad update enable flag
		bool m_CanUpdate : 1;
		//Broad render enable flag
		bool m_CanRender : 1;
		//Control flag for submitting to render group(s), typically done in PreRender()
		bool m_UseRenderGrouping : 1;
		//Optional flag to use alongside RunOnceInit for control purposes
		bool m_CanRunOnceInit : 1;
		//For when the object may be active, renderable, but shouldn't act on inputs
		bool m_CanAcceptInputs : 1;
		
		//Padding
		bool m_Padding00 : 1;

		bool m_Padding17 : 1;
		bool m_Padding16 : 1;
		bool m_Padding15 : 1;
		bool m_Padding14 : 1;
		bool m_Padding13 : 1;
		bool m_Padding12 : 1;
		bool m_Padding11 : 1;
		bool m_Padding10 : 1;
	};

	/*
		Some modules may need to signal back to the actor for specific behaviours without knowing
		what type the actor is, so define some common module communication signals and flags here
	*/
	struct ModuleSignals
	{
		ModuleSignals()
			:m_Padding07(false)
		{}

		//Padding
		bool m_Padding07 : 1;
		bool m_Padding06 : 1;
		bool m_Padding05 : 1;
		bool m_Padding04 : 1;
		bool m_Padding03 : 1;
		bool m_Padding02 : 1;
		bool m_Padding01 : 1;
		bool m_Padding00 : 1;

		//Padding
		bool m_Padding17 : 1;
		bool m_Padding16 : 1;
		bool m_Padding15 : 1;
		bool m_Padding14 : 1;
		bool m_Padding13 : 1;
		bool m_Padding12 : 1;
		bool m_Padding11 : 1;
		bool m_Padding10 : 1;

	};

	////////////////////
	/// Constructors ///
	////////////////////

	Actor2D_Interface();
	virtual ~Actor2D_Interface()
	{
		m_Modules.clear();
	}

	////////////////
	/// Virtuals ///
	////////////////

	//
	//Update & Rendering Stages
	//

	/*
		This is the first stage of the update phase. This should typically be reserved game logic updates, AI updates,
		input handling, actor/entity/object updates. Any transformational changes (like position) should be recorded here
		as a predicted location (not as the original).
	*/
	virtual void Update_Main(System& sys) { }
	/*
		Preperation stage for pre-physic/collision checking stage.
	*/
	virtual void Update_PrePhysics(System& sys) { }
	/*
		Post-Physics update, allowing actors to respond to the sim results, update positions, states etc.
	*/
	virtual void Update_PostPhysics(System& sys) { }
	/*
		Final update window before rendering begins, allowing for animation management or final responses
		to the frame before finalisation.
	*/
	virtual void Update_PreRender(System& sys) { }
	/*
		Primary rendering window. Has two calls:
		- Call with spritebatch included so any rendering done will use that
		- Call without spritebatch included where a spritebatch is found for use via internal index from graphics manager
	*/
	virtual void Render(System& sys) { }
	/*
		Alternative rendering window that uses a given SpriteBatch to draw with over any innate spritebatch
	*/
	virtual void Render(System& sys, DirectX::SpriteBatch* batch) { }

	//
	//Collisions & Callbacks
	//

	/*
		These are typically called during the physics stage when the appropriate collision event occurs,
		such as via a ContactListener instance setup to call these functions.
	*/
	virtual void OnCollisionEnter(Actor2D_Interface* otherActor) { }
	virtual void OnContinuedCollision(Actor2D_Interface* otherActor) { }
	virtual void OnCollisonExit(Actor2D_Interface* otherActor) { }

	/*
		These are adjacent collsion calls that include the b2fixture element in the collision callback
		for more granular details on what type of collider hit the actor alongside the actor owning it
	*/
	virtual void OnCollisionEnter(Actor2D_Interface* otherActor, b2Fixture* fixture) { }
	virtual void OnContinuedCollision(Actor2D_Interface* otherActor, b2Fixture* fixture) { }
	virtual void OnCollisonExit(Actor2D_Interface* otherActor, b2Fixture* fixture) { }

	/*
		These calls allow for much more information about the collision event and objects involved.
		This lets you have much more details responses to events are ideally used events that need the 
		detail (such as collision normals, contact points etc)
	*/
	virtual void OnCollisionEnter(Actor2D_Interface* otherActor, b2Fixture* fixture, b2Contact* contact) { }
	virtual void OnContinuedCollision(Actor2D_Interface* otherActor, b2Fixture* fixture, b2Contact* contact) { }
	virtual void OnCollisonExit(Actor2D_Interface* otherActor, b2Fixture* fixture, b2Contact* contact) { }


	/*
		These would be called in the case of a bodies fixture or joint being destroyed
		These are callbacks that can be used by the appropriate b2 type to inform the actor that the
		event occured, allow information extraction and game logic just before it is destroyed.
	*/
	virtual void OnBodyDestroyed(b2Body* body) { }
	virtual void OnFixtureDestroyed(b2Fixture* fixture) { }
	virtual void OnJointDestroyed(b2Joint* joint) { }

	//
	//Setups & Resets
	//

	/*
		One time initialisation window that is free for the actor to define. Ideal for self-contained
		actors that can define their setup in one go with none-to-minimal other calls required.
	*/
	virtual bool RunOnceInit(System& sys) { return false; }
	/*
		Reserved function for providing a soft reset to actor. Definable by the actor, but should typically
		be limited to resetting values, states, position etc, not releasing of resources like texture.
	*/
	virtual void SoftReset(System& sys) { }
	/*
		Inversely to SoftReset(), this should be reserved for released/reallocating resources or dumping
		any existing data. In otherwords, a fresh slate. This would fit in the niche slot between soft
		resetting and outright deleting and reallocating the actor (where there may be a reason not to
		do so).
	*/
	virtual void HardReset(System& sys) { }

	//
	//Utilities
	//

	//////////////////
	/// Operations ///
	//////////////////
	
	//
	//Utilities
	//

	/*
		As each module holds a pointer to its actor, in some cases that address may become invalid
		(like in copy operations, or memory movement), so this calls a re-sync function with
		each owned module to clear that issue.
	*/
	void ReSyncModules();

	//
	//Helpers
	//

	/*
		Module loading utility that takes in a set of argument alongside a valid derived class to
		insert a new module of that type. Args should match target modules constructor(s).
		Actor2D_Interface pointer is inserted into args list by default (as such each module should have
		this as its	first parameter).
	*/
	template<class MODULE, class... Args>
	MODULE* AddNewModule(Args&&... args)
	{
		//Create new module
		auto mod = std::make_shared<MODULE>(this, std::forward<Args>(args)...);
		//Get hold of the pointer
		MODULE* ptr = mod.get();
		//Store the module
		m_Modules.push_back(std::move(mod));

		//Return with pointer
		return ptr;
	}

	//
	//Debug
	//

	//Appends debug information to given string
	void GetDebugStr_CoreIDs(std::string& str);

	/////////////////
	/// Accessors ///
	/////////////////

	//
	//Module Accessors
	//

	//Returns module via index
	Module_Interface* GetModule(unsigned index);
	//Finds first module with name given, best used with unique naming schemes
	Module_Interface* GetModule(const std::string& name);
	//Finds first module by type, best used with single-only module usage
	Module_Interface* GetModule(Module_Interface::ModuleTypeID typeID);
	//Finds first module with matching string and type
	Module_Interface* GetModule(Module_Interface::ModuleTypeID typeID, const std::string& name);

	/*
		Returns vector of modules matching the given type as the interface type. Preserves order of entry from main module container.
	*/
	std::vector<Module_Interface*> GetModuleList(Module_Interface::ModuleTypeID typeID);
	/*
		Returns vector of modules matching the given type, and returned as the appropriate type via template. Preserves order in main
		module container.
		For performance, static casting is used so ensure that class type and typeID match appropriately to avoid undefined behaviour).
	*/
	template<class TYPE>
	std::vector<TYPE*> GetTModuleList(Module_Interface::ModuleTypeID typeID);

	//
	//ID & Indexes
	//

	int GetID() { return m_CoreIDs.m_ActorID; }
	int GetCategoryID() { return m_CoreIDs.m_CategoryID; }
	unsigned GetUniqueGameID() { return m_CoreIDs.m_UniqueGameID; }
	int GetClassTypeID() { return m_CoreIDs.m_ClassTypeID; }

	void SetID(int id) { m_CoreIDs.m_ActorID = id; }
	void SetCategoryID(int id) { m_CoreIDs.m_CategoryID = id; }
	void SetUniqueGameID(unsigned id) { m_CoreIDs.m_UniqueGameID = id; }
	void SetClassTypeID(int id) { m_CoreIDs.m_ClassTypeID = id; }

	//
	//Other
	//

	float GetActorDepth() { return m_ActorDepth; }

	Flags& GetFlags() { return m_Flags; }
	ModuleSignals& GetSignals() { return m_Signals; }
	//
	//Modules
	//



protected:

	////////////
	/// Data ///
	////////////

	//
	//Modules
	//

	std::vector<std::shared_ptr<Module_Interface>> m_Modules;

	//
	//General
	//

	/*
		This is a depth value that describes the draw level of the actor in totality against all
		other actors. This should be updated as required by the derived (either by informing by
		sprite depths or other logic). This value is especially used in Immediate_Mode render
		grouping as a sorting metric to respect depth even when drawing without batching.
		Not strictly required, but restrain between 0.f to 1.f to mirror sprite depth constraints.
	*/
	float m_ActorDepth = 1.f;

	//Flags and signals (see containers for details)
	Flags m_Flags;
	ModuleSignals m_Signals;

	//
	//IDs and Indexes
	//

	/*
		Collection of utility IDs and indexes for the use with modules behaviours.
		Typically in the fashion of simple, safe defaults.
	*/
	struct
	{
		/*
			When configured to submit to a render group, this ID can be used as a general default when
			not manually controlling for it through logic.
		*/
		unsigned m_RenderGroupIndex = 0;
	} m_UtilityIDs;

	/*
		These are core essential IDs that help define the actor in different ways.
	*/
	struct
	{
		/*
			Defines the primary ID for the actor. This should be a unique identifier for the object,
			even relative to its class type (e.g. Class = Player, ID = 0).
		*/
		int m_ActorID = -1;
		/*
			Defines the broad category ID for the actor. This could be a category like Player, Enemy,
			Terrain etc.
		*/
		int m_CategoryID = -1;
		/*
			Defines the class of the actor without the need for runtime checks. This should be uniform and
			defined elsewhere so that down-casting can be done based on this value (e.g. player_class = 0,
			enemy_class = 1).
		*/
		int m_ClassTypeID = -1;
		/*
			A unique, non-descript ID that should be unique to each game object. Not to be relied on
			for fixed, known ID checking/manipulation. This is automatically called when an actor is
			instantiated.
		*/
		unsigned m_UniqueGameID = 0xFFFFFFFF;
	} m_CoreIDs;
};

//
//Template Funcs
//

template<class TYPE>
std::vector<TYPE*> Actor2D_Interface::GetTModuleList(Module_Interface::ModuleTypeID typeID)
{
	std::vector<TYPE*> mods;

	mods.reserve(m_Modules.size());
	//Cycle container and type check, storing pointers on matches
	for (auto& a : m_Modules)
		if (a->GetType() == typeID)
			mods.push_back(static_cast<TYPE*>(a.get()));

	//Return container
	return mods;
}
