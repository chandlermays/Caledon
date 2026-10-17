/*------------------------------
| File: IFixedUpdatable.h
| Author: Chandler Mays
------------------------------*/
#pragma once

namespace CaledonEngine
{
	class IFixedUpdatable
	{
	public:
		virtual ~IFixedUpdatable() = default;
		virtual void FixedUpdate(float deltaTime) = 0;
	};
}