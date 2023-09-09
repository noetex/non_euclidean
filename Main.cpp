#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include<windows.h>
#undef near
#undef far

#include<map>
#include<cmath>
#include<vector>
#include<string>
#include<cassert>
#include<cstdint>
#include<fstream>
#include<sstream>
#include<iostream>
#include<unordered_map>

#pragma warning(disable : 4100) // Unreferenced formal parameter
#pragma warning(disable : 4099) // Missing PDB file

#define GLEW_STATIC
#include"glew-2.2.0/include/GL/glew.h"
#include"glew-2.2.0/include/GL/wglew.h"

#include"gameheader.h"
#include"Input.h"
#include"vector.h"
#include"Shader.h"
#include"Texture.h"
#include"Camera.h"
#include"FrameBuffer.h"
#include"Collider.h"
#include"Mesh.h"
#include"Resources.h"
#include"Sky.h"
#include"Timer.h"
#include"Sphere.h"

#include"Object.h"
#include"Statue.h"
#include"House.h"
#include"Ground.h"
#include"Pillar.h"
#include"PillarRoom.h"
#include"Portal.h"
#include"Floorplan.h"
#include"Tunnel.h"
#include"Physical.h"
#include"Player.h"

#include"Scene.h"
#include"level1.h"
#include"level2.h"
#include"level3.h"
#include"level4.h"
#include"level5.h"
#include"level6.h"

#include"Engine.h"

#include"engine.cpp"
#include"framebuffer.cpp"
#include"portal.cpp"
#include"player.cpp"
#include"camera.cpp"
#include"collider.cpp"
#include"input.cpp"
#include"level1.cpp"
#include"level2.cpp"
#include"level3.cpp"
#include"level4.cpp"
#include"level5.cpp"
#include"level6.cpp"
#include"mesh.cpp"
#include"object.cpp"
#include"physical.cpp"
#include"resources.cpp"
#include"shader.cpp"
#include"texture.cpp"

static void
create_debug_console(void)
{
  #if 0
  AllocConsole();
  HWND Console = GetConsoleWindow();
  //SetWindowPos(Console, 0, 1920, 200, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
  DWORD ProcessID = GetCurrentProcessId();
  AttachConsole(ProcessID);
  freopen("CON", "w", stdout);
  #endif
}

static void
enable_dpi_awareness(void)
{
  HMODULE Winuser = LoadLibraryA("user32.dll");
  void* Function = GetProcAddress(Winuser, "SetProcessDpiAwarenessContext");
  if(Function)
  {
    typedef BOOL dpi_function(DPI_AWARENESS_CONTEXT);
    dpi_function* SetProcessDpiAwarenessContext = (dpi_function*)Function;
    if(!SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2))
    {
      SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
    }
    return;
  }
  HMODULE ShellCore = LoadLibraryA("shcore.dll");
  Function = GetProcAddress(ShellCore, "SetProcessDpiAwareness");
  if(Function)
  {
    typedef HRESULT dpi_function(PROCESS_DPI_AWARENESS);
    dpi_function* SetProcessDpiAwareness = (dpi_function*)Function;
    SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
    return;
  }
  Function = GetProcAddress(Winuser, "SetProcessDPIAware");
  if(Function)
  {
    typedef BOOL dpi_function(void);
    dpi_function* SetProcessDPIAware = (dpi_function*)Function;
    SetProcessDPIAware();
  }
}

int APIENTRY
WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
  enable_dpi_awareness();
  Engine engine;
  return engine.Run();
}
