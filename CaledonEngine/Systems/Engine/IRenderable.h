/*------------------------------
| File: IRenderable.h
| Author: Chandler Mays
------------------------------*/
#pragma once

namespace CaledonEngine
{
	/*-----------------------------------------------------------------------------------
	| --- IRenderable: Implemented by a subsystem that draws each frame. Every      --- |
	| ---              renderable gets BeginFrame, then every one gets Render, then --- |
	| ---              every one gets EndFrame                                      --- |
	-----------------------------------------------------------------------------------*/
	class IRenderable
	{
	public:
		virtual ~IRenderable() = default;											// Destructor
		virtual void BeginFrame() {}												// Called before any subsystem renders (optional)
		virtual void Render() = 0;													// Draws the subsystem's content
		virtual void EndFrame() {}													// Called after every subsystem has rendered (optional)
	};
}