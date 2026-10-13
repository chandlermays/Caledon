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
	class Application
	{
	private:
		std::unique_ptr<CaledonEngine::EngineManager> m_pEngineManager;							//

	public:
		Application();																			// Constructor
		~Application();																			// Destructor
		Application(const Application&) = delete;												// Prevent copy-construction
		Application& operator=(const Application&) = delete;									// Prevent copy-assignment
		Application(Application&&) = delete;													// Prevent move-construction
		Application& operator=(Application&&) = delete;											// Prevent move-assignment

		bool Initialize();																		//
		void Run();																				//
		void Shutdown();																		//
	};
}