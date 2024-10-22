//*********************************************************************************\\
// 
// Mode demo for use in BEngine demo. Ties together and manages several different
// demo classes and aspects for use in demo.
//
//*********************************************************************************\\

#pragma once

#include "Types/ModeInterface.h"

//Utilities
#include "Utils/MathHelper.h"

//Engine Includes

//Project Includes
#include "Entity_DemoPlayer.h"
#include "Entity_DemoProp.h"

#include "Scene_Demo.h"
#include "Box2DPhysics_Demo.h"

#include "UI_DemoMenu.h"

class Mode_Demo : public Mode_Interface
{
public:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	////////////////////
	/// Constructors ///
	////////////////////

	Mode_Demo(size_t id) :Mode_Interface(id) { }
	~Mode_Demo() { Release(); }

	/////////////////
	/// Overrides ///
	/////////////////

	//
	//Updates & Rendering
	//

	RetCode Update_Main(System& sys) override;
	RetCode Update_PrePhysics(System& sys) override;
	RetCode Update_Physics(System& sys) override;
	RetCode Update_PostPhysics(System& sys) override;
	RetCode Update_PreRender(System& sys) override;

	RetCode Render(System& sys) override;

	//
	//Inputs & Management
	//

	RetCode ProcessKey(char key) override;
	RetCode EnterMode() override;
	bool ExitMode() override;
	RetCode Release() override;


	//////////////////
	/// Operations ///
	//////////////////


private:

	/////////////
	/// Enums ///
	/////////////

	//Defines Menu Indexes/Purpose (Currently one 1, expanding later)
	enum class MenuIndexes
	{
		SCENE_SWITCHER,

		UI_COUNT
	};

	//////////////////
	/// Operations ///
	//////////////////

	//
	//Inits
	//

	//
	//Main sub-updates
	//

	////////////
	/// Data ///
	////////////

	//
	//Composite Demo Classes
	//

	Box2DPhysics_Demo m_PhysicsDemo;
	Scene_Demo m_SceneDemo;

	//
	//Entities
	//

	Entity_DemoPlayer m_Player;

	//
	//UIs
	//

	UI_DemoMenu m_UIs[(unsigned)MenuIndexes::UI_COUNT];
};