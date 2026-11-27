/*------------------------------
| File: Application.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include <memory>

namespace CaledonEngine
{
	class EngineManager;
}

namespace Game
{
	/*----------------------------------------------------------------------------------
	| --- Application: The PacMan executable's entry point into the engine. It     --- |
	| ---              registers the subsystems the game needs (in initialization  --- |
	| ---              order), loads PacManModule.dll through the ModuleSubsystem, --- |
	| ---              then runs the engine until a quit is requested              --- |
	----------------------------------------------------------------------------------*/
	class Application
	{
	private:
		std::unique_ptr<CaledonEngine::EngineManager> m_pEngineManager;							// The engine that owns and runs every subsystem

	public:
		Application();																			// Constructor
		~Application();																			// Destructor
		Application(const Application&) = delete;												// Prevent copy-construction
		Application& operator=(const Application&) = delete;									// Prevent copy-assignment
		Application(Application&&) = delete;													// Prevent move-construction
		Application& operator=(Application&&) = delete;											// Prevent move-assignment

		bool Initialize();																		// Registers the game's subsystems and initializes the engine
		void Run();																				// Runs the engine's main loop
		void Shutdown();																		// Shuts down and destroys the engine
	};
}