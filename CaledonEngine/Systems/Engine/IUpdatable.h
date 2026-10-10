/*------------------------------
| File: IUpdatable.h
| Author: Chandler Mays
------------------------------*/
#pragma once

namespace CaledonEngine
{
	class IUpdatable
	{
	public:
		virtual ~IUpdatable() = default;
		virtual void Update(float deltaTime) = 0;
	};
}