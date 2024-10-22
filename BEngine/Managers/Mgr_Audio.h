//*********************************************************************************\\
// 
// Wrapper manager class that uses DirectXTK audio functionality as a basis. Uses
// XAudio 2.0, providing basic audio functionality (pending new updates).
//
//*********************************************************************************\\

#pragma once

#include <Audio.h>
#include <Dbt.h>
#include <ksmedia.h>

#include <winsdkver.h>
#define _WIN32_WINNT 0x0A00
#include <sdkddkver.h>

#define WAVEBANK_RESERVATION_COUNT 16
#define SFX_INSTANCE_RESERVATION_COUNT 64

class Mgr_Audio
{
public:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	//Handle struct for specific sound instance
	/*
		Struct for holding a SFX instance and additional data used alongside it (like volume categories)
	*/
	struct SoundInstance
	{
		//The SFX resource itself
		std::shared_ptr<DirectX::SoundEffectInstance> m_SFXInstance;
		//Volume category
		unsigned short m_VolID = 0;
	};

	//Support struct for game volumes (defined types like music, sfx etc) + assisting behaviours
	struct GameVolumes
	{

		////////////
		/// Data ///
		////////////
		
		//All the sound types wrapped up
		struct Volumes
		{
			float m_Master = 1.0f;
			float m_Music = 1.0f;
			float m_SFX = 1.0f;
			float m_Ambient = 1.0f;
			float m_Dialog = 1.0f;
		};

		enum VolumeIDs : unsigned short
		{
			MASTER,
			MUSIC,
			SFX,
			AMBIENT,
			DIALOG
		};

		////////////////////
		/// Constructors ///
		////////////////////

		GameVolumes() {}
		GameVolumes(float masterVol, float musicVol, float sfxVol, float ambVol, float dialogVol);
		~GameVolumes() {}


		//////////////////
		/// Operations ///
		//////////////////



		///////////
		/// Get ///
		///////////

		float GetMasterVolume() { return m_Volumes.m_Master; }
		float GetMusicVolume() { return m_Volumes.m_Music; }
		float GetSFXVolume() { return m_Volumes.m_SFX; }
		float GetAmbientVolume() { return m_Volumes.m_Ambient; }
		float GetDialogVolume() { return m_Volumes.m_Dialog; }

		float GetMusicAdjustedVolume() { return m_Volumes.m_Master * m_Volumes.m_Music; }
		float GetSFXAdjustedVolume() { return m_Volumes.m_Master * m_Volumes.m_SFX; }
		float GetAmbientAdjustedVolume() { return m_Volumes.m_Master * m_Volumes.m_Ambient; }
		float GetDialogAdjustedVolume() { return m_Volumes.m_Master * m_Volumes.m_Dialog; }

		//Gets a volume by index
		float GetVolumeByIndex(unsigned short volID);
		float GetAdjustedVolumeByIndex(unsigned short volID);

		///////////
		/// Set ///
		///////////

		//Set volume to number (clamps to 0-1)
		void SetMasterVolume(float vol);
		void SetMusicVolume(float vol);
		void SetSFXVolume(float vol);
		void SetAmbientVolume(float vol);
		void SetDialogVolume(float vol);

		void SetAllVolumes(float masterVol, float musicVol, float sfxVol, float ambVol, float dialogVol);

		//Increments by value (clamps to 0-1)
		void IncMasterVolume(float vol);
		void IncMusicVolume(float vol);
		void IncSFXVolume(float vol);
		void IncAmbientVolume(float vol);
		void IncDialogVolume(float vol);

		//////////////////
		/// Operations ///
		//////////////////

		void ClampNumber(float& vol);

		////////////
		/// Data ///
		////////////

		Volumes m_Volumes;
	};

	////////////////////
	/// Constructors ///
	////////////////////

	Mgr_Audio();
	~Mgr_Audio();

	//////////////////
	/// Operations ///
	//////////////////

	//
	//Core
	//

	//Main update window
	void Update();

	//Plays a single sfx from a given wavebank (See "GameVolumes::Volumes for audio categories)
	void PlayOneShotFromWaveBank(unsigned wbIndex, unsigned sfxIndex, unsigned short volumeTypeID = 0);
	void PlayOneShotFromWaveBank(unsigned wbIndex, unsigned sfxIndex, float pitch = 0.f, float pan = 0.f, unsigned short volumeTypeID = 0);

	//
	//Init + Loading
	//

	//Initialise engine to existing window for game
	void InitAudioEngine(HWND& window);

	//Loads a new wavebank from file, store it, and then returns container index
	size_t LoadNewWavebankFromFile(const std::wstring& fp);
	/*
		Extracts an SFX from an already loaded wavebank and creates new SFX instance from it, storing it
		in the dedicated container, and returning its container index.
	*/
	size_t CreateNewSFXInstanceFromWavebank(unsigned wavebankIndex, unsigned sfxIndex, unsigned short volID = 0);

	//
	//Other
	//

	//Call these as required on event where the window isn't focused and audio needs to be stopped/resumed
	void ResumeAudioEngine() { m_AudioEngine->Resume(); }
	void SuspendAudioEngine() { m_AudioEngine->Suspend(); }
	//Called when a new device is found (if prev is lost or currently in silent mode)
	void OnNewAudioDevice() { m_RetryAudio = true; }

	//Stop or pause all existing SFX instances
	void StopAllSFXInstances();
	void PauseAllSFXInstances();

	//Clear all existing SFX instances from container (Call this BEFORE clearing any wavebanks due to dependency)
	void ClearAllSFXInstances();
	//Clear all existing Wavebanks
	void ClearAllWavebanks();


	/////////////////
	/// Accessors ///
	/////////////////

	GameVolumes& GetVolumeControls() { return m_Volumes; }
	SoundInstance& GetSFXInstance(unsigned index);


private:

	////////////
	/// Data ///
	////////////

	/*
		These are packed banks of SFXs, organised by index over several individual SoundEffects.
		Single SFX instances can be pulled from these to have better control over individual instances of
		said audio via SoundEffectInstances.

		These are best used for repetitve, fire and forget SFXs like gun fire, repetive background noise etc.,
		accessable by index.
	*/
	std::vector<std::shared_ptr<DirectX::WaveBank>> m_WaveBanks;
	/*
		These are single SFX instances that are sourced from an exist SFX(SoundEffect) or a Wavebank. This 
		allows for my dynamic control over a single instance of the SFX, allowing for start/stops, looping, 
		volume control etc. 

		These are best used singular, non-repetative SFXs like background music, specific dialog etc.
		Wrapped up in custom container to keep associated data together.
	*/
	std::vector<SoundInstance> m_SFXInstances;

	//Volumes wrapped container + behaviours
	GameVolumes m_Volumes;

	//Used for detecting new audio devices found during "silent mode"
	HDEVNOTIFY m_NewAudio = nullptr;
	//Check flag if device lost and need to look it again
	bool m_RetryAudio = false;
	//Mute audio flag on minimise/tab out etc
	bool m_MuteWhenUnfocused = false;

	//Core audio class
	std::unique_ptr<DirectX::AudioEngine> m_AudioEngine;
};

