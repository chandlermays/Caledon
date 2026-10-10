/*------------------------------
| File: Main.cpp
| Author: Chandler Mays
------------------------------*/
/*-------------------------------------------------------------------------------------------------
| Entry Point: Application (Standalone Player)
| ------------------------------------
| Lightweight runtime executable for running compiled CaledonEngine games (similar to Unity Player).
|
| Responsibilities:
| - Serves as a Visual Studio startup project for testing standalone performance and release builds.
| - Dynamically loads the project's game assembly and executes the main loop.
| - 
| - 
-------------------------------------------------------------------------------------------------*/
#include "Application.h"

/*----------------------------------------------
| --- Main: Entry point of the application --- |
----------------------------------------------*/
int main()
{
	Game::Application app;

	if (!app.Initialize())
		return -1;

	app.Run();

	return 0;
}