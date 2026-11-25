/*------------------------------
| File: EditorGUI.h
| Author: Chandler Mays
------------------------------*/
#pragma once

namespace CaledonEditor
{
	/*----------------------------------------------------------------------------------------------
	| --- EditorGUI: Owns the ImGui context and its SDL3 backends. The rest of the Editor only --- |
	| ---            talks to ImGui widgets; this is the one place that knows about SDL        --- |
	----------------------------------------------------------------------------------------------*/
	class EditorGUI
	{
	private:
		void* m_pNativeRenderer;													// The SDL_Renderer ImGui draws with (kept as void* so SDL stays out of this header)
		bool m_isInitialized;														// Flag to indicate ImGui and its backends are ready

	public:
		EditorGUI();																// Constructor
		~EditorGUI();																// Destructor
		EditorGUI(const EditorGUI&) = delete;										// Prevent copy-construction
		EditorGUI& operator=(const EditorGUI&) = delete;							// Prevent copy-assignment
		EditorGUI(EditorGUI&&) = delete;											// Prevent move-construction
		EditorGUI& operator=(EditorGUI&&) = delete;									// Prevent move-assignment

		bool Initialize(void* pNativeWindow, void* pNativeRenderer);				// Creates the ImGui context and starts the SDL3 backends
		void Shutdown() noexcept;													// Stops the backends and destroys the ImGui context

		void ProcessEvent(const void* pEvent);										// Passes an SDL_Event to ImGui
		void BeginFrame();															// Starts a new ImGui frame
		void EndFrame();															// Finishes the ImGui frame and draws it onto the window
		void DrawDockspace();														// Makes the whole window a dockspace the panels can dock into

		bool WantsKeyboard() const;													// Checks if ImGui is using the keyboard (e.g. a text field has focus)
		bool WantsMouse() const;													// Checks if ImGui is using the mouse (e.g. it is over a panel)
		bool IsInitialized() const noexcept;										// Checks if ImGui and its backends are ready
	};
}
