//*********************************************************************************\\
//
// Manager class dedicated to managing game UI elements. Like similar pooling 
// classes/managers, this manager pools pointers to existing UI elements for 
// several different utility and functionality purposes. Namely works with WIP
// mouse collider module for ensuring depth is respected in UI functionality
// whilst delgating collision calls the module and fulfilment of behaviour to
// the attached actor.
// 
//*********************************************************************************\\

#pragma once

//Module Includes
#include "Modules/Module_UI_MouseCollider.h"

/*
	Or smaller class definitions here!
*/
class Mgr_UI
{
public:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	////////////////////
	/// Constructors ///
	////////////////////

	Mgr_UI();
	~Mgr_UI();

	//////////////////
	/// Operations ///
	//////////////////

	/*
		Pre-sorts current colliders based on internal depth level, then calls main update window (which should check for collision).
		This update window on natural end or first positive collision event confirmation.
		Consequently, only ONE element will ever have mouse collision detected via this method to use accordingly.
		This should be called generally at the start of the Main Update Window.
	*/
	void Update_MouseColliders(System& sys);

	/////////////////
	/// Accessors ///
	/////////////////

	//Add new mouse collider
	bool AddCollider(Module_UI_MouseCollider* collider);

	//Searches container for given collider, removing if found
	bool RemoveCollider(Module_UI_MouseCollider* collider);

private:

	//////////////////
	/// Operations ///
	//////////////////

	//Container of mouse-based colliders (must be submitted to)
	std::vector<Module_UI_MouseCollider*> m_MouseColliders;

	////////////
	/// Data ///
	////////////

};