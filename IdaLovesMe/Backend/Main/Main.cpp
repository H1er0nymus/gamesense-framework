#include "Main.h"
#include "../Utilities/Utilities.h"
#include "../Hooks/EndScene.hpp"
#include "../Hooks/WndProc.hpp"
#include "../Hooks/PaintTraverse.hpp"
#include "../Hooks/LockCursor.hpp"
#include "../../Frontend/Framework/MenuFramework.h"
#include "../Interfaces/Interfaces.h"
#include "../ValveSDK/NetVar.h"

std::uintptr_t reset_pattern = *reinterpret_cast<std::uintptr_t*>(Misc::Utilities->Memory_PatternScan("gameoverlayrenderer.dll", "C7 45 ? ? ? ? ? FF 15 ? ? ? ? 8B D8") + 9);
std::uintptr_t present_pattern = *reinterpret_cast<std::uintptr_t*>(Misc::Utilities->Memory_PatternScan("gameoverlayrenderer.dll", "FF 15 ? ? ? ? 8B F0 85 FF") + 2);

bool Cheat::Initialize() 
{
	Cheat::wrapgameoverlay();
	Interfaces::InterfaceLoader->Initialize();
	Cheat::NetVarManager = new Cheat::CNetVarManager();
	Cheat::NetVarManager->Init();
	VmtHook* PanelManager = new VmtHook();
	Hooks.push_back(PanelManager);
	PanelManager->Init(Interfaces::GuiPanel);
	PanelManager->HookAt(41, &Hooked::PaintTraverse);
	oPaintTraverse = PanelManager->GetOriginal<fnPaintTraverse>(41);
	VmtHook* SurfaceManager = new VmtHook();
	Hooks.push_back(SurfaceManager);
	SurfaceManager->Init(Interfaces::Surface);
	SurfaceManager->HookAt(67, &Hooked::LockCursor);
	oLockCursor = SurfaceManager->GetOriginal<fnLockCursor>(67);
	oWndProc = (WNDPROC)SetWindowLongPtr(FindWindow(L"Valve001", 0), GWL_WNDPROC, (long)&Hooked::wndProc);
	if (!oWndProc) 
		return false;
	return true;
}

void Cheat::Unload() 
{
	if (CMenu::get()->IsMenuOpened())
		CMenu::get()->SetMenuOpened(false);

	gamesense::ui::DeleteContext(gamesense::Globals::Gui_Ctx);
	Cheat::unwrapgameoverlay();

	for (auto& hook : Hooks) {
		hook->Restore();
	}

	Misc::Utilities->Console_Close();
	Interfaces::InterfaceLoader->Free();
	SetWindowLongPtr(FindWindow(L"Valve001", 0), GWL_WNDPROC, (LONG)oWndProc);
}

void Cheat::wrapgameoverlay() 
{
	oPresent = *reinterpret_cast<decltype(oPresent)*>(present_pattern);
	*reinterpret_cast<decltype(Hooked::present)**>(present_pattern) = Hooked::present;

	oReset = *reinterpret_cast<decltype(oReset)*>(reset_pattern);
	*reinterpret_cast<decltype(Hooked::reset)**>(reset_pattern) = Hooked::reset;
}

void Cheat::unwrapgameoverlay()
{
	*reinterpret_cast<decltype(oPresent)*>(present_pattern) = oPresent;
	*reinterpret_cast<decltype(oReset)*>(reset_pattern) = oReset;
}

void __stdcall MainThread(LPVOID lpParam)
{
	Settings->LoadDefaults();
	Cheat::Initialize();

	while (true) {
		//Features::Visuals->Run();

		if (Settings->UnloadCheat)
			break;
		Sleep(1000);
	}

	Cheat::Unload();

	SetUnhandledExceptionFilter(nullptr);
	FreeLibraryAndExitThread((HMODULE)lpParam, 0);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)MainThread, hModule, NULL, NULL);
		return TRUE;
	}
	return FALSE;
}
