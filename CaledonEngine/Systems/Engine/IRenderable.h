/*------------------------------
| File: IRenderable.h
| Author: Chandler Mays
------------------------------*/
#pragma once

namespace CaledonEngine
{
	class IRenderable
	{
	public:
		virtual ~IRenderable() = default;
		virtual void Render() = 0;
	};
}