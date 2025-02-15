#include "pch.h"
#include "ECS_toolMain.h"
#include "Common\DirectXHelper.h"

using namespace ECS_tool;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;

std::unique_ptr<Renderer> g_sceneRenderer;
// The DirectX 12 Application template is documented at https://go.microsoft.com/fwlink/?LinkID=613670&clcid=0x409

// Loads and initializes application assets when the application is loaded.
ECS_toolMain::ECS_toolMain()
{
	// TODO: Change the timer settings if you want something other than the default variable timestep mode.
	// e.g. for 60 FPS fixed timestep update logic, call:
	/*
	m_timer.SetFixedTimeStep(true);
	m_timer.SetTargetElapsedSeconds(1.0 / 60);
	*/
}

// Creates and initializes the renderers.
void ECS_toolMain::CreateRenderers(const std::shared_ptr<DX::DeviceResources>& deviceResources)
{
	// TODO: Replace this with your app's content initialization.
	g_sceneRenderer = std::unique_ptr<Renderer>(new Renderer(deviceResources));
	
	OnWindowSizeChanged();
}

// Updates the application state once per frame.
void ECS_toolMain::Update()
{
	// Update scene objects.
	m_timer.Tick([&]()
	{
		// TODO: Replace this with your app's content update functions.
		g_sceneRenderer->Update(m_timer);
	});
}

// Renders the current frame according to the current application state.
// Returns true if the frame was rendered and is ready to be displayed.
bool ECS_toolMain::Render()
{
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
	{
		return false;
	}

	// Render the scene objects.
	// TODO: Replace this with your app's content rendering functions.
	return g_sceneRenderer->Render();
}

// Updates application state when the window's size changes (e.g. device orientation change)
void ECS_toolMain::OnWindowSizeChanged()
{
	// TODO: Replace this with the size-dependent initialization of your app's content.
	g_sceneRenderer->CreateWindowSizeDependentResources();
}

// Notifies the app that it is being suspended.
void ECS_toolMain::OnSuspending()
{
	// TODO: Replace this with your app's suspending logic.

	// Process lifetime management may terminate suspended apps at any time, so it is
	// good practice to save any state that will allow the app to restart where it left off.

	g_sceneRenderer->SaveState();

	// If your application uses video memory allocations that are easy to re-create,
	// consider releasing that memory to make it available to other applications.
}

// Notifes the app that it is no longer suspended.
void ECS_toolMain::OnResuming()
{
	// TODO: Replace this with your app's resuming logic.
}

// Notifies renderers that device resources need to be released.
void ECS_toolMain::OnDeviceRemoved()
{
	// TODO: Save any necessary application or renderer state and release the renderer
	// and its resources which are no longer valid.
	g_sceneRenderer->SaveState();
	g_sceneRenderer = nullptr;
}
