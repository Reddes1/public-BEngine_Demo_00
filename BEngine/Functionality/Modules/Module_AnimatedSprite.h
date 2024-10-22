//*********************************************************************************\\
// 
// Animating sprite module that serves as an evolution of Module_Sprite.
// Uses animating class that binds with SpriteData to access animation data via
// texture data to update the sprite according to animation state.
//
//*********************************************************************************\\

#pragma once

#include "Module_Interface.h"

#include "Types/BE_SharedTypes.h"		//Sprite+Animator

//
//Forward Declarations
//

class Module_Box2D_RigidBody2D;		//Used in utility calls

class Module_AnimatedSprite : public Module_Interface
{
public:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	////////////////////
	/// Constructors ///
	////////////////////

	//Typical initialising constructor
	Module_AnimatedSprite(Actor2D_Interface* actor, std::string& name)
		:Module_Interface(actor, name, ModuleTypeID::ANIMATED_SPRITE), m_Animator(&m_SprData)
	{ }
	Module_AnimatedSprite(Actor2D_Interface* actor, std::string& moduleName,
		std::shared_ptr<SpriteTexture>& tex, DirectX::SpriteBatch* batch)
		:Module_Interface(actor, moduleName, ModuleTypeID::ANIMATED_SPRITE), m_Animator(&m_SprData)
	{
		//Set batch
		m_SprData.m_Batch = batch;
		//Set texture
		m_SprData.SetTexture(tex);
	}


	/////////////////
	/// Overrides ///
	/////////////////

	//
	//Main Update Cycles
	//
	
	//Updates the animation timer
	void Update_Main(System& sys) override;
	void Render(System& sys) override;
	void Render(System& sys, DirectX::SpriteBatch* batch) override;

	//NOT USED CURRENTLY
	void Update_PrePhysics(System& sys) override {}
	void Update_Physics(System& sys) override {}
	void Update_PostPhysics(System& sys) override {}
	void Update_PreRender(System& sys) override {}

	//////////////////
	/// Operations ///
	//////////////////

	//
	//Utilities
	//

	//Syncronise position and rotation of the sprite relative to rigidbody position
	void SyncWithRigidBody(Module_Box2D_RigidBody2D& rigidbody);

	void SyncModulePosition(Module_Interface* otherMod) override;


	/////////////////
	/// Accessors ///
	/////////////////

	SpriteAnimator& GetAnimator() { return m_Animator; }
	SpriteData& GetSpriteData() { return m_SprData; }

private:

	//////////////////
	/// Operations ///
	//////////////////

	////////////
	/// Data ///
	////////////
	
	//Animator data and behaviours (binds sprite data to it in constructor)
	SpriteAnimator m_Animator;
	//Core sprite data and behaviours
	SpriteData m_SprData;
};