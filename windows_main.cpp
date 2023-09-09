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

#if 0
static HWND
create_the_window(void)
{
  HINSTANCE Instance = GetModuleHandleW(0);
  WNDCLASSEXW WindowClass = {0};
  WindowClass.cbSize = sizeof(WindowClass);
  WindowClass.lpfnWndProc = window_proc;
  WindowClass.lpszClassName = GH_CLASS;
  WindowClass.style = CS_OWNDC;
  WindowClass.hInstance = Instance;
  //WindowClass.hIcon = LoadIconW(0, (WCHAR*)IDI_WINLOGO);
  //WindowClass.hCursor = LoadCursorW(0, (WCHAR*)IDC_ARROW);
  RegisterClassExW(&WindowClass);
  DWORD WindowStyleEx = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
  DWORD WindowStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
  HWND Result = CreateWindowExW(WindowStyleEx, GH_CLASS, GH_TITLE, WindowStyle, GH_SCREEN_X, GH_SCREEN_Y, GH_SCREEN_WIDTH, GH_SCREEN_HEIGHT, 0, 0, Instance, 0);
  return Result;
}
#endif

int APIENTRY
WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
  create_debug_console();
  enable_dpi_awareness();
  //HWND Window = create_the_window();
  Engine engine;
  return engine.Run();
}
