/*------------------------------
| File: Main.cpp
| Author: Chandler Mays
------------------------------*/
/*-------------------------------------------------------------------------------------------------
| Entry Point: CaledonEditor
| ------------------------------------
| The Editor runs the same engine as the Game, with one more subsystem (EditorSubsystem) on top.
|
| Responsibilities:
| - Registers the engine subsystems the Editor needs, then the EditorSubsystem last, so its
|   ImGui windows are drawn over the scene each frame.
| - The ModuleSubsystem is not registered yet: from Stage 5d the chosen project supplies the
|   game module's DLL path, so it is created after the Project Hub.
-------------------------------------------------------------------------------------------------*/
#include "Editor/EditorSubsystem.h"

#include "CaledonEngine/Systems/Engine/EngineManager.h"
#include "CaledonEngine/Systems/Engine/LoggingSubsystem.h"
#include "CaledonEngine/Systems/Input/InputManager.h"
#include "CaledonEngine/Systems/Module/ComponentRegistry.h"
#include "CaledonEngine/Systems/Physics/Physics2DSubsystem.h"
#include "CaledonEngine/Systems/Rendering/RenderingSubsystem.h"
#include "CaledonEngine/Systems/Scene/SceneSubsystem.h"
#include "CaledonEngine/Systems/Window/WindowConfig.h"
#include "CaledonEngine/Systems/Window/WindowSubsystem.h"

#include <memory>

/*-----------------------------------------
| --- Main: Entry point of the Editor --- |
-----------------------------------------*/
int main()
{
	using namespace CaledonEngine;

	EngineConfig engineConfig;
	engineConfig.logFilePath = "CaledonEditor.txt";

	WindowConfig windowConfig;
	windowConfig.title = "Caledon Editor";
	windowConfig.width = 1600;
	windowConfig.height = 900;
	windowConfig.isResizable = true;

	EngineManager engine{ engineConfig };

	if (!engine.RegisterSubsystem(std::make_unique<LoggingSubsystem>()))							return -1;
	if (!engine.RegisterSubsystem(std::make_unique<WindowSubsystem>(windowConfig)))					return -1;
	if (!engine.RegisterSubsystem(std::make_unique<InputSubsystem>()))								return -1;
	if (!engine.RegisterSubsystem(std::make_unique<RenderingSubsystem>()))							return -1;
	if (!engine.RegisterSubsystem(std::make_unique<Physics2DSubsystem>()))							return -1;
	if (!engine.RegisterSubsystem(std::make_unique<ComponentRegistry>()))							return -1;
	if (!engine.RegisterSubsystem(std::make_unique<SceneSubsystem>()))								return -1;
	if (!engine.RegisterSubsystem(std::make_unique<CaledonEditor::EditorSubsystem>()))				return -1;

	if (!engine.Initialize())
		return -1;

	engine.Run();
	engine.Shutdown();
	return 0;
}
