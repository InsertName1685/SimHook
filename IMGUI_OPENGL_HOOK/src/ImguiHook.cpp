#include "ImguiHook.h"
#include "Hook.h"

#include <GL/GL.h>
#include <tchar.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_win32.h"

namespace values
{
    bool detached = false;

    bool isInitialised = false;
    HWND window = NULL;
    static HGLRC handle_GL_render_context;
    void(*Callback)();
}


BOOL __stdcall h_wglSwapBuffers(HDC hdc);

typedef BOOL(__stdcall* t_SwapBuffers)(HDC hdc);
t_SwapBuffers o_wglSwapBuffers;
SimHook wglSwapBuffers_Hook((BYTE*)GetProcAddress(GetModuleHandle(L"opengl32.dll"), "wglSwapBuffers"), (BYTE*)h_wglSwapBuffers, (BYTE*)&o_wglSwapBuffers, 5);

WNDPROC o_wndproc;
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


bool progress2;
LRESULT WINAPI h_wndproc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    progress2 = true;
    if (!values::detached)
    {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
            return true;
    }
    progress2 = false;
    return CallWindowProc(o_wndproc, hWnd, msg, wParam, lParam);
    
}


void InitImgui(bool *initiated,HDC hdc)
{
    values::window = WindowFromDC(hdc);

    if (WindowFromDC(hdc) == values::window && *initiated) {
        return;
    }
    else {
        if (*initiated) {
            ImGui_ImplWin32_Init(values::window);
            ImGui_ImplOpenGL3_Init();
        }
    }
    o_wndproc = (WNDPROC)SetWindowLongPtr(values::window, GWL_WNDPROC, (LONG_PTR)h_wndproc);

    values::handle_GL_render_context = wglCreateContext(hdc);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplWin32_Init(values::window);
    ImGui_ImplOpenGL3_Init();

    *initiated = true;
}


void RenderImgui(HDC hdc)
{
    HGLRC wglcontext = wglGetCurrentContext();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    values::Callback();

    ImGui::EndFrame();
    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    wglMakeCurrent(hdc,wglcontext);
}



bool inProgress;
BOOL __stdcall h_wglSwapBuffers(HDC hdc)
{
    inProgress = true;

    if (!values::detached)
    {
        InitImgui(&values::isInitialised, hdc);
        RenderImgui(hdc);
    }

    inProgress = false;
	return o_wglSwapBuffers(hdc);
}



void Hook_Imgui::BindHook(void (*callback)())
{
    values::Callback = callback;
	wglSwapBuffers_Hook.Enable();
}

void Hook_Imgui::DetachHook()
{
    Sleep(100);
    values::detached = true;
    while (inProgress && progress2);
    
    try
    {
        wglSwapBuffers_Hook.Disable();
        SetWindowLongPtr(values::window, GWL_WNDPROC, (LONG_PTR)o_wndproc);
    }
    catch (const std::exception&) 
    {
        return;
    };
}
