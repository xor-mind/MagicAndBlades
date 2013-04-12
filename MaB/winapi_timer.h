#ifndef TIMER_H
#define TIMER_H

#include <Windows.h>

struct HighPerformanceTimer
{
	double m_LogicFrameTime; // the time length of a logicFrame, measured in units of the HPT. Divide this by the frequency to get the amount of logic frames a second   		 
	
	__int64 m_Frequency;     // gives the resolution of the computer's high performance timer		
	__int64 m_Time0;         // time0 and time1 form a delta of time in HPT units.
	__int64 m_Time1; 

	void InitTimer(unsigned int targetLogicFps) // targetLogicFPS is the amount of logic frames per a second
	{
		QueryPerformanceFrequency((LARGE_INTEGER *) &m_Frequency);

		m_LogicFrameTime = (m_Frequency)/(double)targetLogicFps;
	}
	void UpdateTime0()// this function gets the current time in HPT units and stores it in m_Time0.
	{
		QueryPerformanceCounter((LARGE_INTEGER *) &m_Time0);
	}

	void UpdateTime1() // this function gets the current time in HPT units and stores it in m_Time1.
	{
		QueryPerformanceCounter((LARGE_INTEGER *) &m_Time1);
	}

	double LogicTime() const { return m_LogicFrameTime; }
	__int64 DeltaTime() const { return m_Time1 - m_Time0; }// the difference between time1 and time0

	void UpdateFixedStep() { m_Time0 += (long long)m_LogicFrameTime; }
};

#endif