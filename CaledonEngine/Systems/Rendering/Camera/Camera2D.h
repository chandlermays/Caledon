#pragma once

// A Camera is a device through which the player views the world

// A screen space point is defined in pixels.
// The bottom left of the screen is (0,0)
// The top-right is (pixelWidth,pixelHeight)
// The Z position is in world units from the Camera

// A viewport space point is normalized and relative to the Camera.
// The bottom-left of the Camera is (0,0)
// The top-right is (1,1)
// The z position is in world units from the Camera

// A world space point is defined in global coordinates (i.e. Transform::GetPosition())

//--------
// Static Properties
//--------
// main															:						pointer/reference to the active primary Camera2D in the current scene (tagged or set as main)
// allCameras													:						collection/vector of all currently enabled Camera2D components active in the scene
// allCamerasCount												:						the number of currently active Camera2D instances
//
//--------
// Properties
//--------
// orthographicSize												:						half the vertical size of the camera view in world units; controls zoom (defaults 5.0f)
// aspect														:						aspect ratio of the target viewport (viewportWidth / viewportHeight)
// backgroundColor												:						the 'Color' used to clear the render target background before rendering a frame
// nearClipPlane												:						near depth plane distance (useful for layer ordering and sorting, once implemented)
// farClipPlane													:						far depth plane distanec
// pixelWidth													:						width of the camera's render viewport in piels (read-only)
// pixelHeight													:						height of the camera's render viewport in pixels (read-only)
// pixelRect													:						screen space rectangle definingwhere on the render target/window the camera draws
// rect															:						normalized viewport coordinates (0.0f to 1.0f) defining render bounds relative to the render target
// targetTexture												:						pointer to off-screen Texture render target (used by ViewportPanel to render scene output to an ImGui image)
// cullingMask													:						bitmask to selectively exclude or include specific GameObject layers from rendering
// 
//--------
// Public Methods
//--------
// WorldToScreenPoint(Vector2f worldPos)						:						converts global world coordinates into screen pixel coordinates (where 0,0 is bottom-left or top-left depending on the renderer setup)
// ScreenToWorldPoint(Vector2f screenPos)						:						converts screen pixel coordinates (i.e. mouse cursor clicks in CaledonEditor) into global world coordinates
// WorldToViewportPoint(Vector2f worldPos)						:						converts world space to normalized viewport space ( (0,0) to (1,1) )
// ViewportToWorldPoint(Vector2f viewportPos)					:						converts normalized viewport coordinates (0.0f to 1.0f) back into world space
// ScreenToViewportPoin(Vector2f screenPos)						:						converts screen pixel coordinates directly into normalized (0..1) viewport space
// ViewportToScreenPoint(Vector2f viewportPos)					:						converts normalized (0..1) viewport coordinates into screen pixel coordinates
// WorldToScreenSize(Vector2f worldSize)						:						calculates how many screen piels a given world unit dimension occupies
// ScreenToWorldSize(Vector2f pixelSize)						:						calculates how many world units a given screen pixel size represents
// GetOrthographicBounds()										:						returns a RectFloat representing the full world-space bounding box visible by the camera
// 
//--------
// Static Methods
//--------
// GetAllCameras(std::vector<Camera2D*>& outCameras)			:						fills a provided list with all currently active cameras in the scene
// 
//--------
// Messages (Event Callbacks)
//--------
// OnPreCull													:						called right before the camera evaluates scene objects against its cullingMask or orthographic bounds
// OnPreRender													:						called immediately before the camera begins rendering scene components to its target buffer
// OnPostRender													:						called immediately after the camera finishes rendering all objects to its target buffer
// OnRenderObject												:						callback sent to scene objects/components when the camera renders them
//
//--------
// Delegates
//--------
// CameraCallback												:						function signature/delegate type used to hook custom callbacks into lifecycle events (i.e. std::function<void(Camera2D*)>)