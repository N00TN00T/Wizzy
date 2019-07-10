#include "wzpch.h"

#include "Wizzy/Stopwatch.h"

namespace Wizzy {
	Stopwatch::Stopwatch(bool active) {
		m_start = std::chrono::high_resolution_clock::now();
		m_pauseTime = m_start;
		m_isActive = active;
	}
	double Stopwatch::ElapsedSeconds() {
		return ElapsedMilliseconds() / 1000.0;
	}
	double Stopwatch::ElapsedMilliseconds() {
		return m_isActive ? std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_start).count() / 1000000.0
			: std::chrono::duration_cast<std::chrono::nanoseconds>(m_pauseTime - m_start).count() / 1000000.0;
	}
	void Stopwatch::SetElapsedSeconds(double value) {
		m_start = std::chrono::high_resolution_clock::now() - std::chrono::duration_cast<TimePoint::clock::duration>(std::chrono::duration<double>(value * 1000.0));
	}
	void Stopwatch::Reset() {
		m_start = std::chrono::high_resolution_clock::now();
	}
	void Stopwatch::Pause() {
		m_isActive = false;
		m_pauseTime = std::chrono::high_resolution_clock::now();
	}
	void Stopwatch::Start() {
		m_isActive = true;
		SetElapsedSeconds(ElapsedSeconds());
	}
	void Stopwatch::Stop() {
		Reset();
		Pause();
	}
}
