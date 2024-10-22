#include "Mgr_Audio.h"

#include "Utils/Utils_Debug.h"

using namespace DirectX;

Mgr_Audio::Mgr_Audio()
{
	m_WaveBanks.reserve(WAVEBANK_RESERVATION_COUNT);
	m_SFXInstances.reserve(SFX_INSTANCE_RESERVATION_COUNT);
}

Mgr_Audio::~Mgr_Audio()
{
	SuspendAudioEngine();

	for (auto& a : m_SFXInstances)
		a.m_SFXInstance.reset();
	for (auto& a : m_WaveBanks)
		a.reset();

	m_SFXInstances.clear();
	m_WaveBanks.clear();
}

void Mgr_Audio::Update()
{
	//Check for new device
	if (m_NewAudio)
	{
		UnregisterDeviceNotification(m_NewAudio);
		m_NewAudio = nullptr;
	}

	if (m_RetryAudio)
	{
		//Reset flag
		m_RetryAudio = false;
		if (m_AudioEngine->Reset())
		{
			//Restart loops here
		}
	}
	else if (!m_AudioEngine->Update())
	{
		//Check if the audio device has been lost
		if (m_AudioEngine->IsCriticalError())
		{
			//Device Lost, flag for reset
			m_RetryAudio = true;
		}

		//Volumes may have changed so update existing SFXs volumes
		for (auto& a : m_SFXInstances)
			a.m_SFXInstance->SetVolume(m_Volumes.GetAdjustedVolumeByIndex(a.m_VolID));
	}
}

void Mgr_Audio::PlayOneShotFromWaveBank(unsigned wbIndex, unsigned sfxIndex, unsigned short volumeTypeID)
{
	assert(wbIndex <= m_WaveBanks.size());
	m_WaveBanks[wbIndex]->Play(sfxIndex, m_Volumes.GetAdjustedVolumeByIndex(volumeTypeID), 0.f, 0.f);
}

void Mgr_Audio::PlayOneShotFromWaveBank(unsigned wbIndex, unsigned sfxIndex, float pitch, float pan, unsigned short volumeTypeID)
{
	assert(wbIndex <= m_WaveBanks.size());
	m_WaveBanks[wbIndex]->Play(sfxIndex, m_Volumes.GetAdjustedVolumeByIndex(volumeTypeID), pitch, pan);
}

void Mgr_Audio::InitAudioEngine(HWND& window)
{
	// Listen for new audio devices
	DEV_BROADCAST_DEVICEINTERFACE filter = {};
	filter.dbcc_size = sizeof(filter);
	filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	filter.dbcc_classguid = KSCATEGORY_AUDIO;

	m_NewAudio = RegisterDeviceNotification(window, &filter,
		DEVICE_NOTIFY_WINDOW_HANDLE);


	//Init audio engine to default spec, with extra debug checking
	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
	eflags |= AudioEngine_Debug;
#endif
	m_AudioEngine = std::make_unique<AudioEngine>(eflags);

	//Check if the audo device is present and signal one way or another (signal use of subtitles here maybe)
	if (!m_AudioEngine->IsAudioDevicePresent())
		DBOUT(">> No Audio Device found. Running program in Silent Mode.")
	else
		DBOUT("Audio Device found.");
}

size_t Mgr_Audio::LoadNewWavebankFromFile(const std::wstring& fp)
{
	std::shared_ptr<DirectX::WaveBank> newWB = std::make_shared<DirectX::WaveBank>(m_AudioEngine.get(), fp.c_str());
	m_WaveBanks.push_back(newWB);
	return m_WaveBanks.size() - 1;
}

size_t Mgr_Audio::CreateNewSFXInstanceFromWavebank(unsigned wavebankIndex, unsigned sfxIndex, unsigned short volID)
{
	assert(wavebankIndex <= m_WaveBanks.size());

	//Create new data
	SoundInstance newInst;
	newInst.m_SFXInstance = m_WaveBanks[wavebankIndex]->CreateInstance(sfxIndex);
	newInst.m_VolID = volID;
	newInst.m_SFXInstance->SetVolume(m_Volumes.GetAdjustedVolumeByIndex(volID));

	//Store and return index
	m_SFXInstances.push_back(newInst);
	return m_SFXInstances.size() - 1;
}


void Mgr_Audio::StopAllSFXInstances()
{
	for (auto& a : m_SFXInstances)
		a.m_SFXInstance->Stop();
}

void Mgr_Audio::PauseAllSFXInstances()
{
	for (auto& a : m_SFXInstances)
		a.m_SFXInstance->Pause();
}

void Mgr_Audio::ClearAllSFXInstances()
{
	SuspendAudioEngine();

	for (auto& a : m_SFXInstances)
		a.m_SFXInstance.reset();
	m_SFXInstances.clear();

	ResumeAudioEngine();
}

void Mgr_Audio::ClearAllWavebanks()
{
	SuspendAudioEngine();

	for (auto& a : m_WaveBanks)
		a.reset();
	m_WaveBanks.clear();

	ResumeAudioEngine();
}

Mgr_Audio::SoundInstance& Mgr_Audio::GetSFXInstance(unsigned index)
{
	assert(index <= m_SFXInstances.size());
	return m_SFXInstances[index];
}





Mgr_Audio::GameVolumes::GameVolumes(float masterVol, float musicVol, float sfxVol, float ambVol, float dialogVol)
{
	SetAllVolumes(masterVol, musicVol, sfxVol, ambVol, dialogVol);
}

float Mgr_Audio::GameVolumes::GetVolumeByIndex(unsigned short volID)
{
	switch (volID)
	{
	case MUSIC:
		return GetMusicVolume();
		break;
	case SFX:
		return GetSFXVolume();
		break;
	case AMBIENT:
		return GetAmbientVolume();
		break;
	case DIALOG:
		return GetDialogVolume();
		break;
	default:
		return GetMasterVolume();
		break;
	}

	//Should never hit this
	return 0.0f;
}

float Mgr_Audio::GameVolumes::GetAdjustedVolumeByIndex(unsigned short volID)
{
	switch (volID)
	{
	case MUSIC:
		return GetMusicAdjustedVolume();
		break;
	case SFX:
		return GetSFXAdjustedVolume();
		break;
	case AMBIENT:
		return GetAmbientAdjustedVolume();
		break;
	case DIALOG:
		return GetDialogAdjustedVolume();
		break;
	default:
		return GetMasterVolume();
		break;
	}

	//Should never hit this
	return 0.0f;
}

void Mgr_Audio::GameVolumes::SetMasterVolume(float vol)
{
	m_Volumes.m_Master += vol;
	ClampNumber(m_Volumes.m_Master);
}

void Mgr_Audio::GameVolumes::SetMusicVolume(float vol)
{
	m_Volumes.m_Music = vol;
	ClampNumber(m_Volumes.m_Music);
}

void Mgr_Audio::GameVolumes::SetSFXVolume(float vol)
{
	m_Volumes.m_SFX = vol;
	ClampNumber(m_Volumes.m_SFX);
}

void Mgr_Audio::GameVolumes::SetAmbientVolume(float vol)
{
	m_Volumes.m_Ambient = vol;
	ClampNumber(m_Volumes.m_Ambient);
}

void Mgr_Audio::GameVolumes::SetDialogVolume(float vol)
{
	m_Volumes.m_Dialog = vol;
	ClampNumber(m_Volumes.m_Dialog);
}

void Mgr_Audio::GameVolumes::SetAllVolumes(float masterVol, float musicVol, float sfxVol, float ambVol, float dialogVol)
{
	m_Volumes.m_Master = masterVol;
	m_Volumes.m_Music = musicVol;
	m_Volumes.m_SFX = sfxVol;
	m_Volumes.m_Ambient = ambVol;
	m_Volumes.m_Dialog = dialogVol;
}

void Mgr_Audio::GameVolumes::IncMasterVolume(float vol)
{
	m_Volumes.m_Master += vol;
	ClampNumber(m_Volumes.m_Master);
}

void Mgr_Audio::GameVolumes::IncMusicVolume(float vol)
{
	m_Volumes.m_Music += vol;
	ClampNumber(m_Volumes.m_Music);
}

void Mgr_Audio::GameVolumes::IncSFXVolume(float vol)
{
	m_Volumes.m_SFX += vol;
	ClampNumber(m_Volumes.m_SFX);
}

void Mgr_Audio::GameVolumes::IncAmbientVolume(float vol)
{
	m_Volumes.m_Ambient += vol;
	ClampNumber(m_Volumes.m_Ambient);
}

void Mgr_Audio::GameVolumes::IncDialogVolume(float vol)
{
	m_Volumes.m_Dialog += vol;
	ClampNumber(m_Volumes.m_Dialog);
}

void Mgr_Audio::GameVolumes::ClampNumber(float& vol)
{
	if (vol > 1.0f)
		vol = 1.0f;
	else if (vol < 0.0f)
		vol = 0.0f;
}
