/*------------------------------
| File: FixedTimestep.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include <algorithm>
#include <cmath>

namespace CaledonEngine
{
	/*-----------------------------------------------------------------------------------------------
	| --- FixedTimestep: Accumulates frame time and determines how many fixed simulation steps  --- |
	| ---                to run, carrying fractional time between frames. A step cap drops      --- |
	| ---                excess backlog to avoid the spiral of death; Alpha provides the        --- |
	| ---                remaining fraction for render interpolation.                           --- |
	-----------------------------------------------------------------------------------------------*/
	class FixedTimestep
	{
	private:
		double m_step;														// Seconds per fixed step (0 = disabled)
		int m_maxSteps;														// Cap on steps returned per frame (>= 1)
		double m_accumulator;												// Unspent time carried between frames

	public:
		/*---------------------
		| --- Constructor --- |
		---------------------*/
		FixedTimestep(double stepSeconds, int maxStepsPerFrame) noexcept
			: m_step{ stepSeconds > 0.0 ? stepSeconds : 0.0 }
			, m_maxSteps{ std::max(maxStepsPerFrame, 1) }
			, m_accumulator{ 0.0 }
		{ }

		/*---------------------------------------------------------
		| --- IsEnabled: Returns whether fixed stepping is on --- |
		---------------------------------------------------------*/
		bool IsEnabled() const noexcept
		{
			return m_step > 0.0;
		}

		/*--------------------------------------------------
		| --- Step: Returns the seconds per fixed step --- |
		--------------------------------------------------*/
		double Step() const noexcept
		{
			return m_step; 
		}
		
		/*----------------------------------------------------------------------
		| --- Advance: Returns the number of fixed steps to run this frame --- |
		----------------------------------------------------------------------*/
		int Advance(double frameSeconds) noexcept
		{
			if (m_step <= 0.0 || frameSeconds <= 0.0)
				return 0;

			m_accumulator += frameSeconds;

			int steps = static_cast<int>(m_accumulator / m_step);
			if (steps > m_maxSteps)
			{
				steps = m_maxSteps;

				// Drop the backlog, keep the partial step
				m_accumulator = std::fmod(m_accumulator, m_step);
			}
			else
			{
				m_accumulator -= steps * m_step;
			}

			return steps;
		}

		/*----------------------------------------------------------------
		| --- Alpha: Returns the leftover fraction of a step (0...1) --- |
		----------------------------------------------------------------*/
		double Alpha() const noexcept
		{
			return (m_step > 0.0) ? m_accumulator / m_step : 0.0;
		}

		/*-----------------------------------------------
		| --- Reset: Discards any carried-over time --- |
		-----------------------------------------------*/
		void Reset() noexcept
		{
			m_accumulator = 0.0;
		}
	};
}
