/*------------------------------
| File: FixedTimestep.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include <algorithm>
#include <cmath>

// The accumulator behind IFixedUpdatable. Pure arithmetic (no clock, no engine types) so it is trivially testable.
//
// Each frame, feed it the real frame time; it answers "how many fixed steps should run now?".
//   - Time that doesn't fill a whole step carries over to the next frame (no drift).
//   - A frame that would need more than maxStepsPerFrame steps (after a hitch, a breakpoint, a dragged window) runs the
//     cap and DISCARDS the rest. Otherwise every slow frame would demand more steps, making the next frame slower:
//     the "spiral of death". The simulation briefly runs slower than real time instead.
//   - Alpha() is the leftover fraction of a step (0..1), reserved for render interpolation (BodyInterpolation2D).

namespace CaledonEngine
{
	class FixedTimestep
	{
	private:
		double m_step;																	// Seconds per fixed step (0 = disabled)
		int m_maxSteps;																	// Cap on steps returned per frame (>= 1)
		double m_accumulator;															// Unspent time carried between frames

	public:
		FixedTimestep(double stepSeconds, int maxStepsPerFrame) noexcept
			: m_step{ stepSeconds > 0.0 ? stepSeconds : 0.0 }
			, m_maxSteps{ std::max(maxStepsPerFrame, 1) }
			, m_accumulator{ 0.0 }
		{ }

		bool IsEnabled() const noexcept { return m_step > 0.0; }
		double Step() const noexcept { return m_step; }

		int Advance(double frameSeconds) noexcept											// Returns the number of fixed steps to run this frame
		{
			if (m_step <= 0.0 || frameSeconds <= 0.0)
				return 0;

			m_accumulator += frameSeconds;

			int steps = static_cast<int>(m_accumulator / m_step);
			if (steps > m_maxSteps)
			{
				steps = m_maxSteps;
				m_accumulator = std::fmod(m_accumulator, m_step);							// Drop the backlog, keep the partial step
			}
			else
			{
				m_accumulator -= steps * m_step;
			}

			return steps;
		}

		double Alpha() const noexcept { return (m_step > 0.0) ? m_accumulator / m_step : 0.0; }
		void Reset() noexcept { m_accumulator = 0.0; }
	};
}
