/*------------------------------
| File: IUpdatable.h
| Author: Chandler Mays
------------------------------*/
#pragma once

namespace CaledonEngine
{
	/*--------------------------------------------------------------------------
	| --- IUpdatable: Implemented by a subsystem that ticks once per frame --- |
	--------------------------------------------------------------------------*/
	class IUpdatable
	{
	public:
		virtual ~IUpdatable() = default;															// Destructor
		virtual void Update(float deltaTime) = 0;													// Called once per frame with the frame's delta time
	};
}