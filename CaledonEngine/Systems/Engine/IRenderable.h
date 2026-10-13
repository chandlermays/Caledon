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
		virtual void BeginFrame() {}
		virtual void Render() = 0;
		virtual void EndFrame() {}
	};
}