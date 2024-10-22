//*********************************************************************************\\
// 
// Wrapping module for SFString type. Used to draw strings with DirectX Spritefont
// and Spritebatch. Provides some extra utilities for syncronising on top of 
// baseline functionality.
//
//*********************************************************************************\\

#pragma once

#include "Module_Interface.h"

//Engine Includes
#include "Types/BE_SharedTypes.h"

class Module_UI_SFString : public Module_Interface
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

	Module_UI_SFString(Actor2D_Interface* actor)
		:Module_Interface(actor, std::string("Nameless String"), ModuleTypeID::UI_SF_STRING)
	{}
	Module_UI_SFString(Actor2D_Interface* actor, std::string& name)
		:Module_Interface(actor, name, ModuleTypeID::UI_SF_STRING)
	{ }

	//
	//Module Constructors
	//
	
	//Sets just font
	Module_UI_SFString(Actor2D_Interface* actor, DirectX::SpriteFont* font);
	//Sets font + name
	Module_UI_SFString(Actor2D_Interface* actor, std::string& name, DirectX::SpriteFont* font);
	//Sets font + batch
	Module_UI_SFString(Actor2D_Interface* actor, DirectX::SpriteFont* font, DirectX::SpriteBatch* batch);
	//Sets font + batch in addition to other data
	Module_UI_SFString(Actor2D_Interface* actor, std::string& name,	DirectX::SpriteFont* font, DirectX::SpriteBatch* batch);

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
	
	//Calls the default draw (batch and font must be set)
	void Render(System& sys) override;
	//Calls with given batch (font must be set)
	void Render(System& sys, DirectX::SpriteBatch* batch) override;

	//
	//Utilities
	//

	void SyncModulePosition(Module_Interface* otherMod) override;

	//////////////////
	/// Operations ///
	//////////////////
	
	//
	//Data Syncing
	//

	/////////////////
	/// Accessors ///
	/////////////////

	SFString& GetStringData() { return m_StringData; }

private:

	//////////////////
	/// Operations ///
	//////////////////

	////////////
	/// Data ///
	////////////

	//Core data (this class wraps around this type)
	SFString m_StringData;
};
