/* Demo */

#include<Windows.h>
#include<iostream>


#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_win32.h"
#include "ImguiHook.h"

void foo_callback()
{

	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, { 800.f,600.f });
	ImGui::Begin("SimHook", nullptr, ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowFontScale(4);
	ImGui::PopStyleVar();
	ImGui::Text("Skibidi Toilet");
	ImGui::End();
};


DWORD WINAPI MainThread(HMODULE hmodule)
{
	FILE* fstream;
	AllocConsole();
	freopen_s(&fstream, "CONOUT$", "w", stdout);
	while (!GetAsyncKeyState(VK_END));

	FreeConsole();
	FreeLibraryAndExitThread(hmodule, NULL);
	return 0;
}


BOOL WINAPI DllMain(HMODULE hmodule,DWORD reason,LPVOID lpReserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MainThread,hmodule, 0,NULL);
		Hook_Imgui::BindHook(foo_callback);
		break;

	case DLL_PROCESS_DETACH:
		Hook_Imgui::DetachHook();
		Sleep(100);
		break;
	}

	return 1;
}