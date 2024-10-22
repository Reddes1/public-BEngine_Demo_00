//*********************************************************************************\\
// 
// Simple sprite module for use with actors. Utilises a shared, public sprite
// data type for uniformity
//
//*********************************************************************************\\

#pragma once

#include "Module_Interface.h"

#include "Types/BE_SharedTypes.h"		//Sprite Data Type

//
//Forward Declarations
//

class Module_Box2D_RigidBody2D;		//Used in utility calls

class Module_Sprite : public Module_Interface
{
public:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	////////////////////
	/// Constructors ///
	////////////////////

	//
	//Parent Constructors
	//

	Module_Sprite(Actor2D_Interface* actor)
		:Module_Interface(actor, std::string("Nameless Sprite"), ModuleTypeID::SPRITE)
	{}
	Module_Sprite(Actor2D_Interface* actor, std::string& name)
		:Module_Interface(actor, name, ModuleTypeID::SPRITE)
	{ }

	//
	//Module Constructors
	//
	
	Module_Sprite(Actor2D_Interface* actor, std::string& moduleName, SpriteTexture* tex, DirectX::SpriteBatch* batch);


	/////////////////
	/// Overrides ///
	/////////////////

	//
	//Main Update Cycles
	//

	void Update_Main(System& sys) override {}
	void Update_PrePhysics(System& sys) override {}
	void Update_Physics(System& sys) override {}
	void Update_PostPhysics(System& sys) override {}
	void Update_PreRender(System& sys) override {}
	void Render(System& sys) override;
	void Render(System& sys, DirectX::SpriteBatch* batch) override;

	//
	//Utilities
	//

	void SyncModulePosition(Module_Interface* otherMod) override;

	//////////////////
	/// Operations ///
	//////////////////

	//
	//Utilities
	//

	//Syncronise position and rotation of the sprite relative to rigidbody position
	void SyncWithRigidBody(Module_Box2D_RigidBody2D& rigidbody);

	/////////////////
	/// Accessors ///
	/////////////////

	SpriteData& GetSpriteData() { return m_SprData; }

private:

	//////////////////
	/// Operations ///
	//////////////////
	
	////////////
	/// Data ///
	////////////

	//Core data (this class wraps around this type)
	SpriteData m_SprData;
};