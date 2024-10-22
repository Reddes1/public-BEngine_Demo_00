//*********************************************************************************\\
// 
// Manager class for providing game/operational states for program to use. Used in
// conjunction with "ModeInterface.h", which state classes should be derived from.
//
//*********************************************************************************\\

#pragma once
#include <vector>

#include "Types/ModeInterface.h" //Mode Interface

class Mgr_Modes
{
public:
	
	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	static constexpr size_t MODE_RES_COUNT = 12;

	////////////////////
	/// Constructors ///
	////////////////////

	Mgr_Modes();
	~Mgr_Modes() { Release(); }


	//////////////////
	/// Operations ///
	//////////////////

	//
	//Run-Time Behaviour
	//

	//Update current mode
	void Update(System& sys);
	//Render current mode
	void Render(System& sys);
	//Handle WH_CHAR key messages
	void ProcessKey(char key);
	//Change current mode via ID
	bool SwitchMode(size_t id);

	//
	//Setup
	//

	//Add a new mode via dynamic allocation
	void AddMode(Mode_Interface* p);

	/////////////////
	/// Accessors ///
	/////////////////

	const Mode_Interface::Flags& GetActiveModeFlags() { return m_Modes[m_CurrentModeIndex]->GetModeFlags(); }


private:

	//////////////////
	/// Operations ///
	//////////////////

	//Release resources, called in destructor
	void Release();

	////////////
	/// Data ///
	////////////

	//All created modes
	std::vector<Mode_Interface*> m_Modes;
	//Currently active mode index
	int m_CurrentModeIndex = -1;			
	//Next mode to be loaded (waits on true return from Exit() func)
	int m_NextModeIndex = -1;				
};