#pragma once
#include <Windows.h>
#include "ClientApp.h"

/*=====================================================================

	Here, an entry point is decided for the application to start. 
	*Note: If you decide to compile and run on mac, you must define IE_PLATFORM_MAC
	in premake file as well as change 'kind' under project "Application" to 'ConsoleApp'
	becasue mac does not recognize wWinMain as a valid entry point

 ======================================================================*/

extern Insight::Application* Insight::CreateApplication();

#ifdef IE_PLATFORM_WINDOWS
	int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
	{
		Insight::Log::Init();

		auto app = Insight::CreateApplication();

		if (!app->InitializeAppForWindows(hInstance, nCmdShow))
		{
			IE_CORE_FATAL(L"Failed to initialize engine. Exiting.");
			return 0;
		}

		app->Run();

		app->Shutdown();

		delete app;

		return 0;
	}
#elif IE_PLATFORM_MAC
	int main(int argc, char** argv)
	{
		auto app = Insight::CreateApplication();
		app->Init();
		app->Run();
		delete app;
		return 0;
	}
#else
	#error No valid entry point found for engine to begin execution.
#endif