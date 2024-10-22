//*********************************************************************************\\
// 
// Testing ground for Box2D physics module in an Actor, sprited, behaviour shell.
//
//*********************************************************************************\\

#pragma once

#include "Actors/Actor2D_Interface.h"

class Entity_PhysicsDemo : public Actor2D_Interface
{
public:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	//Internal module indexes
	enum class ModuleIndexes
	{
		SPRITE,
		RIGIDBODY
	};

	////////////////////
	/// Constructors ///
	////////////////////

	Entity_PhysicsDemo() {}
	~Entity_PhysicsDemo() {}

	/////////////////
	/// Overrides ///
	/////////////////

	//
	//Update & Rendering Stages
	//

	//Initial gameplay updates, input management etc
	void Update_Main(System& sys) override;
	void Update_PrePhysics(System& sys) override;
	void Update_PostPhysics(System& sys) override;
	void Update_PreRender(System& sys) override;
	void Render(System& sys) override;
	void Render(System& sys, DirectX::SpriteBatch* batch);

	//Main init function
	bool RunOnceInit(System& sys) override;

	//////////////////
	/// Operations ///
	//////////////////

	/////////////////
	/// Accessors ///
	/////////////////

private:

	//////////////////
	/// Operations ///
	//////////////////

	////////////
	/// Data ///
	////////////

};