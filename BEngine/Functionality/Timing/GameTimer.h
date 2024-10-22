//***************************************************************************************
// 
// GameTimer.h by Frank Luna (C) 2011 All Rights Reserved. 
// Modified by Benjamin Kimberley 2023-
//
//***************************************************************************************

#ifndef GAMETIMER_H
#define GAMETIMER_H

class GameTimer
{
public:
	GameTimer();

	float TotalTime()const; // in seconds
	float DeltaTime()const; // in seconds

	void Reset(); // Call before message loop.
	void Start(); // Call when unpaused.
	void Stop();  // Call when paused.
	void Tick();  // Call every frame.

	//Forcably set delta time (should only be done under specific circumstances and should be left up to Tick typically)
	void SetDeltaTime(float delta) { m_DeltaTime = delta; }

private:
	double m_SecondsPerCount;
	double m_DeltaTime;

	__int64 m_BaseTime;
	__int64 m_PausedTime;
	__int64 m_StopTime;
	__int64 m_PrevTime;
	__int64 m_CurrTime;

	bool m_Stopped;
};

#endif // GAMETIMER_H