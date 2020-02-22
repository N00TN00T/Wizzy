#pragma once

namespace Wizzy {
    typedef std::chrono::high_resolution_clock::time_point TimePoint;

    	class Stopwatch {
    	private:
    		TimePoint m_start;
    		TimePoint m_pauseTime;
    		bool m_isActive;
    	public:
    		Stopwatch(bool active = true);

    		double ElapsedSeconds() const;
    		double ElapsedMilliseconds() const;

    		void SetElapsedSeconds(double value);

    		void Reset();
    		void Pause();
    		void Start();
    		void Stop();
    };
}
