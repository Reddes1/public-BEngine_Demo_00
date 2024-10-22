//*********************************************************************************\\
// 
// UI Demo (Major WIP)
//
//*********************************************************************************\\

#pragma once

#include "Actors/Actor2D_Interface.h"	//Parent

//Utils
#include "Utils/Utils_General.h"

//Forward Declarations
class Module_UI_MouseCollider;

class UI_DemoMenu : public Actor2D_Interface
{
public:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	//Modules indexes for direct access
	enum class ModuleIndexes : unsigned
	{
		//
		//Sprites
		//

		SPRITE_DIALOG_BOX,
		SPRITE_ARROW_LEFT,
		SPRITE_ARROW_RIGHT,

		//
		//Mouse Colliders
		//

		M_COLLIDER_ARROW_LEFT,
		M_COLLIDER_ARROW_RIGHT,

		//
		//UI Strings
		//

		SFS_BOX_MSG
	};

	//Total count of modules (should reflect above)
	enum class ModuleCounts : unsigned
	{
		SPRITE_COUNT = 3,
		COLLIDER_COUNT = 2
	};

	//Function in
	enum class FuncSet_A_Indexes : unsigned
	{
		//Gets current scene name (O, Scene_Demo::GetSceneName)
		GET_SCENE_NAME,

		//NOT AN INDEX
		FUNC_PTR_COUNT
	};

	enum class FuncSet_B_Indexes : unsigned
	{
		//Manipulate current scene (I/I, Scene_Demo::SwitchScene
		CHANGE_BG_SCENE,

		//NOT AN INDEX
		FUNC_PTR_COUNT
	};

	////////////////////
	/// Constructors ///
	////////////////////

	UI_DemoMenu() {}
	~UI_DemoMenu() {}

	/////////////////
	/// Overrides ///
	/////////////////

	//
	//Updates & Rendering Stages
	//

	void Update_Main(System& sys) override;
	void Update_PrePhysics(System& sys) override { }
	void Update_PostPhysics(System& sys) override { }
	void Update_PreRender(System& sys) override;

	void Render(System& sys) override;
	void Render(System& sys, DirectX::SpriteBatch* batch) override;

	//
	//Setups & Resets
	//

	bool RunOnceInit(System& sys) override;
	void SoftReset(System& sys) override { }
	void HardReset(System& sys) override { }

	//
	//Utilities
	//

	//Move the UI and all its elements to this location (preserves offsets)
	void MoveTo(const Vec2& pos);

	//////////////////
	/// Operations ///
	//////////////////

	//Forces update of string (should only be called after all setup is done, both here and with bound class)
	void ForceSceneNameUpdate();



	/////////////////
	/// Accessors ///
	/////////////////

	//Sets the depths for all UI collider elements
	void SetDepth(float depth);

	//Binds new function to Set A (VoidFuncA)
	void BindFuncToSlot(FuncSet_A_Indexes index, VoidFuncA& func);
	//Binds new function to Set A (VoidFuncB)
	void BindFuncToSlot(FuncSet_B_Indexes index, VoidFuncB& func);

private:

	//////////////////
	/// Operations ///
	//////////////////

	//
	//Replacement calls for the above (fix comments later)
	//

	//Behaviours for when a UI collider has detected a hover
	bool OnHover(System& sys, Module_UI_MouseCollider* collider);
	//Behaviours for when a UI collider has detected an end to hovering
	bool OnHoverEnd(System& sys, Module_UI_MouseCollider* collider);
	//When desired input detected
	bool OnInput(System& sys, Module_UI_MouseCollider* collider);


	//Positional offsets for sprites being used
	Vec2 m_Offsets[(int)ModuleCounts::SPRITE_COUNT] = {
		{0.f, 0.f}, {-250.f, 0.f}, {250.f, 0.f}
	};

	////////////
	/// Data ///
	////////////

	//Single slot function pointers
	VoidFuncA m_FuncsA[(unsigned)FuncSet_A_Indexes::FUNC_PTR_COUNT];
	//Single slot function pointers
	VoidFuncB m_FuncsB[(unsigned)FuncSet_B_Indexes::FUNC_PTR_COUNT];

	//Hold on to scene name here for now (replacing later, this is just a test)
	std::string m_SceneName = "NULL";
};
