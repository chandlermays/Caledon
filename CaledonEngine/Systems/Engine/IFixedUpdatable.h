/*------------------------------
| File: IFixedUpdatable.h
| Author: Chandler Mays
------------------------------*/
#pragma once

namespace CaledonEngine
{
	/*----------------------------------------------------------------------------------
	| --- IFixedUpdatable: Implemented by a subsystem that ticks at the fixed rate --- |
	----------------------------------------------------------------------------------*/
	class IFixedUpdatable
	{
	public:
		virtual ~IFixedUpdatable() = default;									// Destructor
		virtual void FixedUpdate(float deltaTime) = 0;							// Called once per fixed step with the fixed delta time
	};
}