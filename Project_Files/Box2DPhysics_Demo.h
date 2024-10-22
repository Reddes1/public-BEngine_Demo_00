//*********************************************************************************\\
// 
// Class/File defintions here!
//
//*********************************************************************************\\

#pragma once

//Project Includes
#include "Entity_DemoProp.h"

class Box2DPhysics_Demo
{
public:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	//Demo modes (different scenes and simulation setups)
	enum class ModeID : unsigned
	{
		BALLPIT
	};


	////////////////////
	/// Constructors ///
	////////////////////

	//////////////////
	/// Operations ///
	//////////////////
	
	//
	//Updates & Rendering
	//

	void Update_Main(System& sys);
	void Update_PrePhysics(System& sys);
	void Update_Physics(System& sys);
	void Update_PostPhysics(System& sys);
	void Update_PreRender(System& sys);

	void Render(System& sys);

	//
	//Inits
	//

	//Setup the demo according to given ID (does completely reset the sim, allocations, everything)
	//SetWorld() must be called before this point
	void SetupDemo(System& sys, ModeID id);

	/////////////////
	/// Accessors ///
	/////////////////

	//Set world that simulations are setup in
	void SetWorld(b2World* world) { m_World = world; }

private:
	
	/////////////
	/// Enums ///
	/////////////

	//Internal configation data
	enum class ConfigData : unsigned
	{
		DEFAULT_RESERVE_COUNT = 500,
		DEFAULT_BOUNDARIES_COUNT = 4, 
		BALLPIT_DEFAULT_PROP_COUNT = 175
	};


	//////////////////
	/// Operations ///
	//////////////////

	//Releases current resources
	void Release();

	//
	//Setups
	//

	void Setup_Ballpit(System& sys);

	////////////
	/// Data ///
	////////////

	//Props
	std::vector<Entity_DemoProp> m_Props;

	//World be using for simulations
	b2World* m_World = nullptr;
};