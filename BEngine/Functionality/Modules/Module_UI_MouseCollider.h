//*********************************************************************************\\
// 
// Mouse-focused UI module that uses DirectX colliders to provide a simple platform
// for detecting mouseover collision and signalling to the owning actor of these
// events.
// 
// Best used with "Mgr_UI" alongside internal DepthLevel to manage overlapping UI
// scenarios and better effiecency and accuracy of UI behaviour.
//
//*********************************************************************************\\

#pragma once

#include "Module_Interface.h"

//Library Inclusions
#include <variant>			//Type safe union class
#include <DirectXCollision.h>

//Engine Includes
#include "Types/BE_SharedTypes.h"

class Module_UI_MouseCollider : public Module_Interface
{
public:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	//What type of collider to use
	enum class ColliderType
	{
		BOX,
		CIRCLE,
		UNDEFINED
	};

	////////////////////
	/// Constructors ///
	////////////////////

	//
	//Parent Constructors
	//

	Module_UI_MouseCollider(Actor2D_Interface* actor)
		:Module_Interface(actor, std::string("Nameless Mouse Collider"), ModuleTypeID::UI_MOUSE_COLLIDER)
	{}
	Module_UI_MouseCollider(Actor2D_Interface* actor, std::string& name)
		:Module_Interface(actor, name, ModuleTypeID::UI_MOUSE_COLLIDER)
	{ }

	//
	//Module Constructors
	//

	/////////////////
	/// Overrides ///
	/////////////////

	//
	//Updates & Rendering Stages
	//

	void Update_Main(System& sys) override;

	//
	//Utilities
	//

	void SyncModulePosition(Module_Interface* otherMod) override;

	//////////////////
	/// Operations ///
	//////////////////

	//Condition checks
	bool IsMouseHovering() { return m_IsHovering; }
	bool WasMouseHovering() { return m_WasHovered; }

	//Forcibly clear the current elapsed time (typically left update logic)
	void ResetElapsed() { m_HoverElapsed = 0.f; }

	//
	//Utilties
	//

	/*
		Syncs the position of the mouse collider with given sprite data. This assumes the position of the
		sprite data as the center, otherwise see SetPosition to do this directly.
	*/
	void SyncPositionWithSpriteData(const SpriteData& sprData);
	/*
		Syncs position of collider with given SFString data.
	*/
	void SyncPositionWithStringData(const SFString& strData);


	/////////////////
	/// Accessors ///
	/////////////////

	//
	//General Behaviour
	//

	//Forcibly set hover state (this should typically left to logic however)
	void SetHoverState(bool hover) { m_IsHovering = hover; }
	//This is the z-depth (1.f = highest, 0.f lowest)
	void SetDepthLevel(float depth) { m_DepthLevel = depth; }
	void SetAsBoundingBox(const DirectX::BoundingBox& box);
	void SetAsCircle(const DirectX::BoundingSphere& circle);

	//Sets X/Y position (Z automatically set to 1.f as un-used axis)
	void SetPosition(float x, float y);

	float GetDepthLevel() { return m_DepthLevel; }
	float GetHoverTime() { return m_HoverElapsed; }

	//Returns reference for direct manipulation, see GetType to determine what function to call (if not automatically known)
	DirectX::BoundingBox& GetBoxCollider() { return std::get<DirectX::BoundingBox>(m_Collider); }
	DirectX::BoundingSphere& GetCircleCollider() { return std::get<DirectX::BoundingSphere>(m_Collider); }

	//Alternative call that determines type via std::visit. Caller should be aware of what type to expect.
	template<typename T>
	T& GetCollider();

	//
	//Paired Objects
	//

	//This is the class/struct ID of the object (which is ideally unique, or unique in its context)
	void SetPairedObjectTypeID(int id) { m_PairedObjectData.m_ObjectTypeID = id; }
	//This is the contextually unique ID that helps ID the object amongst others of the same type
	void SetPairedObjectID(int id) { m_PairedObjectData.m_ObjectID = id; }
	//Stores the paired object as void*
	template<class CLASS>
	void SetPairedObjectPointer(CLASS* obj);

	int GetPairedObjectTypeID() { return m_PairedObjectData.m_ObjectTypeID; }
	int GetPairedObjectID() { return m_PairedObjectData.m_ObjectID; }
	//Returns the stored void* as given type. Up to the user to ensure correct class is returned.
	//Note: syntax = GetAssociatedObject<CLASS>();
	template<class CLASS>
	CLASS* GetAssociatedObject();

protected:

	//////////////////
	/// Operations ///
	//////////////////

	////////////
	/// Data ///
	////////////

	//Colliders packed as a union for efficency, see m_Type for this is configured
	std::variant<DirectX::BoundingBox, DirectX::BoundingSphere> m_Collider;
	ColliderType m_Type = ColliderType::UNDEFINED;

	/*
		Defines the layer & priority of the UI collider relative to other colliders. This is used in
		the Mgr_UI class to sort by order of depth (highest first) to solve for overlapping elements.
		By default, the manager sorts highest > lowest.
	*/
	float m_DepthLevel = 1.f;
	//Tracking flag indicating that the element is currently hovered
	bool m_IsHovering = false;
	//Buffer flag for tracking if the element was previously hovered
	bool m_WasHovered = false;
	//Tracking timer for how long the mouse has hovered over this collider, reseting when de-hovered
	float m_HoverElapsed = 0.f;

	struct
	{
		/*
			This is generic ID value that used to help determine the type of object this collider is
			associated with. If it were a module, this value could related to the modules type IDs
			(See ModuleTypeID). Its for the user to determine when to use this and in the right contexts
			(Just like when doing any sort of casting). -1 set by default for unknown/undefined.
		*/
		int m_ObjectTypeID = -1;
		
		/*
			This is another generic ID that can be used to signify what the object is beyond its type,
			such as what sub-type of object it is, or a simple way of identifying it uniquely if no
			other method is present. -1 set by default for unknown/undefined.
		*/
		int m_ObjectID = -1;
		/*
			Another supportive mechanism that can be used in conjunction with the above data to provide
			direct access via pointer to the object this collider relates to, allowing for a more direct
			and self-contained method of acting on the paired object. Is stored as void* so must know
			type when accessing to prevent undefined behaviour.
		*/
		void* m_ObjectPtr = nullptr;

	} m_PairedObjectData;
};

template<typename T>
inline T& Module_UI_MouseCollider::GetCollider()
{
	//Use visit to determine the active type and return the collider as that type (caller must know what to expect for proper behaviour)
	return std::visit([](auto& collider) -> T& { return collider; }, m_Collider);
}

template<class CLASS>
inline void Module_UI_MouseCollider::SetPairedObjectPointer(CLASS* obj)
{
	assert(obj);
	m_PairedObjectData.m_ObjectPtr = static_cast<void*>(obj);
}

template<class CLASS>
inline CLASS* Module_UI_MouseCollider::GetAssociatedObject()
{
	return static_cast<CLASS*>(m_PairedObjectData.m_ObjectPtr);
}
