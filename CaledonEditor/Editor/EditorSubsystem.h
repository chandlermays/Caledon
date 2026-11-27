/*------------------------------
| File: EditorSubsystem.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "Editor/EditorGUI.h"

#include "CaledonEngine/Systems/Engine/IRenderable.h"
#include "CaledonEngine/Systems/Engine/ISubsystem.h"

#include <string_view>

namespace CaledonEngine
{
	class EngineContext;
	class InputSubsystem;
}

namespace CaledonEditor
{
	/*-------------------------------------------------------------------------------------------------
	| --- EditorSubsystem: The Editor as one more engine subsystem. It is registered after the    --- |
	| ---                  RenderingSubsystem, so each frame the scene is drawn first and the     --- |
	| ---                  Editor's ImGui windows are drawn on top, before the frame is presented --- |
	-------------------------------------------------------------------------------------------------*/
	class EditorSubsystem final : public CaledonEngine::ISubsystem, public CaledonEngine::IRenderable
	{
	private:
		CaledonEngine::EngineContext* m_pContext;									// The context used to reach the other subsystems
		CaledonEngine::InputSubsystem* m_pInput;									// The InputSubsystem that forwards SDL events to ImGui
		EditorGUI m_gui;															// The ImGui context and its SDL3 backends
		bool m_showImGuiDemo;														// Flag to show the ImGui demo window (View menu)

		void DrawMainMenuBar();														// Draws the File and View menus along the top of the window

	public:
		static constexpr std::string_view kName = "Editor";							// Unique name used for registration and lookup

		EditorSubsystem();															// Constructor
		~EditorSubsystem() override;												// Destructor
		EditorSubsystem(const EditorSubsystem&) = delete;							// Prevent copy-construction
		EditorSubsystem& operator=(const EditorSubsystem&) = delete;				// Prevent copy-assignment
		EditorSubsystem(EditorSubsystem&&) = delete;								// Prevent move-construction
		EditorSubsystem& operator=(EditorSubsystem&&) = delete;						// Prevent move-assignment

		std::string_view Name() const noexcept override;							// Returns the Subsystem's name
		bool Initialize(CaledonEngine::EngineContext& context) override;			// Starts ImGui on the engine's window and renderer
		void Shutdown() noexcept override;											// Stops forwarding events and shuts ImGui down

		void BeginFrame() override;													// Starts the Editor's ImGui frame
		void Render() override;														// Draws the Editor's windows and hands them to the renderer
	};
}
