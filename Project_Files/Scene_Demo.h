//*********************************************************************************\\
// 
// Managed scene for use in BEngine Demo.
//
//*********************************************************************************\\

#pragma once

#include "Actors/Actor2D_Interface.h"

//Modules
#include "Modules/Module_Sprite.h"

//Simple derivations of Actor2D_Interface for the purposes of this demo
class DemoEnt_Scroller : public Actor2D_Interface
{
public:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	//Internal behaviour slightly different based on setup type
	enum class ObjectID : unsigned
	{
		TRAIN,
		CLOUD
	};
	enum class SceneID : unsigned
	{
		DAYTIME,
		NIGHTTIME
	};

	////////////////////
	/// Constructors ///
	////////////////////

	DemoEnt_Scroller()
	{
		//Create required modules
		AddNewModule<Module_Sprite>(std::string("Main_Sprite"));
	}

	/////////////////
	/// Overrides ///
	/////////////////

	void Update_Main(System& sys) override;
	void Update_PreRender(System& sys) override;
	void Render(System& sys, DirectX::SpriteBatch* batch) override;
	void SoftReset(System& sys) override;

	//////////////////
	/// Operations ///
	//////////////////

	ObjectID m_ObjectID = ObjectID::TRAIN;
	SceneID m_SceneID = SceneID::DAYTIME;
	Vec2 m_ScrollSpeed = { -400.f, 0.f };
	//X = target boundary, Y = Where to place after hitting boundary (Only using X axis for scrollers)
	Vec2 m_Limits = { -1000.f, 1000.f };

private:

};

class DemoEnt_Static : public Actor2D_Interface
{
public:
	////////////////////
	/// Constructors ///
	////////////////////

	DemoEnt_Static()
	{
		//Create required modules
		AddNewModule<Module_Sprite>(std::string("Main_Sprite"));
	}

	/////////////////
	/// Overrides ///
	/////////////////

	void Update_PreRender(System& sys) override;
	void Render(System& sys, DirectX::SpriteBatch* batch) override;
	void SoftReset(System& sys) override;

private:

};

class Scene_Demo
{
public:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	enum class SceneID
	{
		//ABANDONED,
		DAYTIME,
		NIGHT,
		//SNOWRAIN,
		//SUNSET,
		COUNT
	};

	enum class SceneSwitchCode : unsigned
	{
		SCENE_LEFT,
		SCENE_RIGHT
	};

	////////////////////
	/// Constructors ///
	////////////////////

	Scene_Demo() {}
	~Scene_Demo() {}

	//////////////////
	/// Operations ///
	//////////////////

	//
	//Setup
	//

	bool RunOnceInit(System& sys, SceneID id);

	//
	//Updating
	//

	bool Update_Main(System& sys);
	void Update_PreRender(System& sys);


	//Directly switches scene via given ID
	void SwitchScene(System& sys, SceneID id);

	/*
		Manages scene switch by shifting code up/down. See SceneSwitchCode for safe casting of code,
		however 0 == Left, 1 == Right if just passing int.
	*/
	void SwitchScene(System& sys, int code);
	


	//
	//Bindables
	//

	//Bindable indirection of the SwitchScene function. See original for details.
	void Bindable_SwitchScene(System* sys, int* sceneCode);
	//VoidFunc suitable version of GetSceneName
	void Bindable_GetSceneName(std::string* inboundStr);


	/////////////////
	/// Accessors ///
	/////////////////

	SceneID GetSceneID() { return m_SceneID; }
	const std::string& GetSceneName() { return m_SceneName; }

private:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	enum ArrayDefs : unsigned
	{
		IDLE_SPRITES = 0,
		ANIMATED_SPRITES = 1,
		ARRAY_COUNT = 2,

		IDLE_COUNT = 32,
		ANIMATED_COUNT = 16
	};

	//////////////////
	/// Operations ///
	//////////////////
	
	//Resets all scene elements to an unused state
	void ResetScene();

	//Abandoned city setup function
	bool Setup_Abandoned(System& sys);
	bool Setup_Daytime(System& sys);
	bool Setup_Night(System& sys);

	////////////
	/// Data ///
	////////////

	//Objects used in scene(s)
	DemoEnt_Scroller m_Scrollers[ArrayDefs::ANIMATED_COUNT];
	DemoEnt_Static m_Statics[ArrayDefs::IDLE_COUNT];

	//Track internal scene
	SceneID m_SceneID = SceneID::DAYTIME;
	//Name of the current scene (changed on scene change)
	std::string m_SceneName = "Daytime";


	//Flag to track that setup has already been done once (to smooth transition between some elements)
	bool m_InitSetupDone = false;
};



