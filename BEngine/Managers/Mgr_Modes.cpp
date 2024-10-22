#include "Mgr_Modes.h"

#include "Utils/Utils_Debug.h"

Mgr_Modes::Mgr_Modes()
{
	//Reserve a starting amount of modes
	m_Modes.reserve(MODE_RES_COUNT);
}

bool Mgr_Modes::SwitchMode(size_t id)
{
	//Check there are modes loaded
	msg_assert(!m_Modes.empty(), "SwitchMode(): No modes loaded!");
	//Check through the modes to find a valid mode index
	for (auto& a : m_Modes)
	{
		if (a->GetModeID() == id)
		{
			m_NextModeIndex = static_cast<int>(id);
			return true;
		}
	}
	//Failed to find mode
	msg_assert(false, "SwitchMode(): Failed to find mode!");
	return false;
}

void Mgr_Modes::Update(System& sys)
{
	//Check to see if we have a pending mode shift
	if (m_NextModeIndex != m_CurrentModeIndex)
	{
		//If the current index is in valid or if the current mode has returned a true from its Exit Call
		if (m_CurrentModeIndex == -1 || m_Modes[m_CurrentModeIndex]->ExitMode())
		{
			//Shift to the new mode index and call its Enter Function
			m_CurrentModeIndex = m_NextModeIndex;
			m_Modes[m_CurrentModeIndex]->EnterMode();
		}
	}

	//Call each update stage in order
	m_Modes[m_CurrentModeIndex]->Update_Main(sys);
	m_Modes[m_CurrentModeIndex]->Update_PrePhysics(sys);
	m_Modes[m_CurrentModeIndex]->Update_Physics(sys);
	m_Modes[m_CurrentModeIndex]->Update_PostPhysics(sys);
	m_Modes[m_CurrentModeIndex]->Update_PreRender(sys);

}

void Mgr_Modes::Render(System& sys)
{
	m_Modes[m_CurrentModeIndex]->Render(sys);
}

void Mgr_Modes::ProcessKey(char key)
{
	m_Modes[m_CurrentModeIndex]->ProcessKey(key);
}

void Mgr_Modes::AddMode(Mode_Interface* ptr)
{
	msg_assert(ptr != nullptr, "AddMode(): No Mode Passed!");
	m_Modes.push_back(ptr);
}

void Mgr_Modes::Release()
{
	for (size_t i = 0; i < m_Modes.size(); ++i)
		delete m_Modes[i];
	m_Modes.clear();
}