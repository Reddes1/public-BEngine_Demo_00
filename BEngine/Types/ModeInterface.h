//*********************************************************************************\\
// 
// Interface designed for use with "Mgr_Modes.h". Acts as interface for
// possible state classes.
//
//*********************************************************************************\\

#pragma once

//Engine Includes
#include "Types/BE_ReturnCodes.h"

//
//Foward Declarations
//

//Data passing container defined in "Game.h"
struct System;

/*
	Replacement for the above later, here just for prep
*/
class Mode_Interface
{
public:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	//Internal contextual flags that can used for additional mode behaviours and controls
	//such as run once enters/exits, pause updates/render etc.

	/*
		Internal state and control flags for granular control over models operations.
	*/
	struct Flags
	{
		Flags()
			:m_DoRunOnceEnter(true), m_DoRunOnceExit(false), m_CanUpdate(true), m_CanRender(true),
			m_DefferedRendering(false), m_Padding02(false), m_Padding01(false), m_Padding00(false)
		{}

		bool m_DoRunOnceEnter : 1;
		bool m_DoRunOnceExit : 1;
		bool m_CanUpdate : 1;
		bool m_CanRender : 1;
		//Does the mode have rendering setup (pre/post frame) deffered to it
		bool m_DefferedRendering : 1;
		bool m_Padding02 : 1;
		bool m_Padding01 : 1;
		bool m_Padding00 : 1;
	};


	////////////////////
	/// Constructors ///
	////////////////////

	Mode_Interface(size_t modeID)
		: m_ModeID(modeID)
	{ }
	virtual ~Mode_Interface() { }

	////////////////
	/// Virtuals ///
	////////////////

	//
	//Updates & Rendering
	//

	/*
		These update and render functions mirror the setup found in Actor_Interface, and should be appropriately fulfilled
		to their descriptions in the derrived modes (e.g. calling appropriate actor calls in appropriate modes) so as behaviours
		are predictable. The Mgr_Modes will call each of these calls in the correct sequence automatically.

		All updates are hosted under the Update() function and just Render() in Render() (PreRender is still an update stage).
	*/

	/*
		Primary gameplay update stage. Manage gameplay logic, inputs, AI etc.
	*/
	virtual RetCode Update_Main(System& sys) = 0;
	/*
		Prep stage for physics related tasks.
	*/
	virtual RetCode Update_PrePhysics(System& sys) = 0;
	/*
		Main physics stage. As the actors do not themselves have a matching call for this (other than CollisionEvent calls),
		the mode should appropriately submit data to physics engine. This process will call CollisionEvent calls at this time.
	*/
	virtual RetCode Update_Physics(System& sys) = 0;
	/*
		Reaction stage for actors and gameplay logic to respond to results of the previous update stage.
	*/
	virtual RetCode Update_PostPhysics(System& sys) = 0;
	/*
		Final update window for setting any actor animations or render settings before commiting to render.
	*/
	virtual RetCode Update_PreRender(System& sys) = 0;
	virtual RetCode Render(System& sys) = 0;

	//
	//Inputs & Management
	//

	/*
		Called on switching to the new moce once to allow for one-time on enter behaviours to execute. 
		See Flags for additional control on enabling one-time ever behaviour on entry.
	*/
	virtual RetCode EnterMode() = 0;
	/*
		Like EnterMode, inversly allows behaviour when leaving the mode. Must return true for manager to proceed to switching
		modes. See Flags for additional control on enabling one-time ever behaviour on exit.
	*/
	virtual bool ExitMode() = 0;
	/*
		Allows for WM_CHAR messages to be processed by the active mode
	*/
	virtual RetCode ProcessKey(char key) = 0;
	/*
		Release call that must be included in deconstructor call to release any assets (typically on call on game end or complete mode resetting)
	*/
	virtual RetCode Release() = 0;


	/////////////////
	/// Accessors ///
	/////////////////

	size_t GetModeID() { return m_ModeID; }
	const Flags& GetModeFlags() { return m_Flags; }

protected:

	////////////
	/// Data ///
	////////////

	/*
		Modes primary, unique, indentifier (describable via enum).
		Must remain unique for mode manager search to function properly
	*/
	size_t m_ModeID;

	//Current state flags
	Flags m_Flags;
};