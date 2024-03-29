#pragma once

#ifndef CORE_UTIL_TIMER_H_
#define CORE_UTIL_TIMER_H_

class Timer {
public:
	Timer(bool _start = true) {
		m_bRunning = false;
		if (_start) start();
	}

	void start() {
		m_bRunning = true;
		m_Start = getTime();
	}
	
	void stop() {
		m_bRunning = false;
		m_Stop = getTime();
	}

	//! returns the elapsed time in seconds
	double getElapsedTime() {
		if (m_bRunning) {
			return getTime() - m_Start;
		} else {
			return m_Stop - m_Start;
		}
	}

	double getElapsedTimeMS() {
		return getElapsedTime() * 1000.0;
	}

	void printElapsedTimeMS() {
		printf("%.2f ms\n", getElapsedTimeMS());
	}

	//! returns the time in seconds
	static double getTime();
private:
	bool m_bRunning;

	double m_Start;
	double m_Stop;
};

class FrameTimer
{
public:
	FrameTimer()
	{
		m_secondsPerFrame = 1.0 / 60.0;
	}
	void frame()
	{
		double elapsed = m_clock.getElapsedTime();
		m_clock.start();
		m_secondsPerFrame = elapsed * 0.2 + m_secondsPerFrame * 0.8;
	}
	float framesPerSecond()
	{
		return 1.0f / (float)m_secondsPerFrame;
	}
	float secondsPerFrame()
	{
		return (float)m_secondsPerFrame;
	}

    float elapsedTime()
    {
        return (float)m_clock.getElapsedTime();
    }

private:
	Timer m_clock;
	double m_secondsPerFrame;
};

#endif // CORE_UTIL_TIMER_H_