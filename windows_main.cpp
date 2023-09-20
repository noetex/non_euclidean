static Engine* GH_ENGINE;

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

static void
confine_cursor(HWND Window)
{
  if(GH_HIDE_MOUSE)
  {
    RECT WindowRect;
    GetWindowRect(Window, &WindowRect);
    int CursorX = (WindowRect.left + WindowRect.right) / 2;
    int CursorY = (WindowRect.top + WindowRect.bottom) / 2;
    SetCursorPos(CursorX, CursorY);
  }
}

static LRESULT CALLBACK
window_proc(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
{
  LRESULT Result = 0;
  switch(Message)
  {
    case WM_SYSCOMMAND:
    {
      if (wParam == SC_SCREENSAVE || wParam == SC_MONITORPOWER)
      {
        return 0;
      }
    } break;
    case WM_PAINT:
    {
      PAINTSTRUCT PaintStruct;
      BeginPaint(Window, &PaintStruct);
      EndPaint(Window, &PaintStruct);
    } break;
    case WM_SIZE:
    {
      //engine->iWidth = LOWORD(lParam);
      //engine->iHeight = HIWORD(lParam);
      //PostMessageW(Window, WM_PAINT, 0, 0);
    } break;
    case WM_KEYDOWN:
    {
      bool_t AlreadyPressed = (lParam >> 30) & 0x1;
      if(!AlreadyPressed)
      {
        switch(wParam)
        {
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          {
            size_t SceneIndex = wParam - '1';
            GH_ENGINE->load_scene(SceneIndex);
          } break;
          case VK_ESCAPE:
          {
            PostQuitMessage(0);
          } break;
        }
        GH_ENGINE->input.key[wParam & 0xFF] = true;
        GH_ENGINE->input.key_press[wParam & 0xFF] = true;
      }
    } break;
    case WM_SYSKEYDOWN:
    {
      if (wParam == VK_RETURN)
      {
        //ToggleFullscreen(engine);
      }
    } break;
    case WM_KEYUP:
    {
      GH_ENGINE->input.key[wParam & 0xFF] = false;
    } break;
    case WM_INPUT:
    {
      RAWINPUT RawInput;
      UINT InputSize = sizeof(RawInput);
      HRAWINPUT RawInputHandle = (HRAWINPUT)lParam;
      GetRawInputData(RawInputHandle, RID_INPUT, (void*)&RawInput, &InputSize, sizeof(RAWINPUTHEADER));
      if(RawInput.data.mouse.usFlags == MOUSE_MOVE_RELATIVE)
      {
        GH_ENGINE->input.mouse_ddx += RawInput.data.mouse.lLastX;
        GH_ENGINE->input.mouse_ddy += RawInput.data.mouse.lLastY;
      }
    } break;
    case WM_CLOSE:
    {
      PostQuitMessage(0);
    } break;
    default:
    {
      Result = DefWindowProcW(Window, Message, wParam, lParam);
    }
  }
  return Result;
}

static void
setup_raw_input(HWND Window)
{
  RAWINPUTDEVICE Mouse = {0};
  Mouse.usUsagePage = HID_USAGE_PAGE_GENERIC;
  Mouse.usUsage = HID_USAGE_GENERIC_MOUSE;
  Mouse.dwFlags = RIDEV_INPUTSINK;
  Mouse.hwndTarget = Window;
  RegisterRawInputDevices(&Mouse, 1, sizeof(Mouse));
}

static HGLRC
create_opengl_context(HDC WindowDC)
{
  PIXELFORMATDESCRIPTOR PFD = {0};
  PFD.nSize = sizeof(PFD);
  PFD.nVersion = 1;
  PFD.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  PFD.iPixelType = PFD_TYPE_RGBA;
  PFD.cColorBits = 32;
  PFD.cDepthBits = 32;
  int PixelFormat = ChoosePixelFormat(WindowDC, &PFD);
  SetPixelFormat(WindowDC, PixelFormat, &PFD);
  DescribePixelFormat(WindowDC, PixelFormat, sizeof(PFD), &PFD);
  HGLRC Result = wglCreateContext(WindowDC);
  wglMakeCurrent(WindowDC, Result);
  return Result;
}

static HWND
create_the_window(void)
{
  WNDCLASSEXW WindowClass = {0};
  WindowClass.cbSize = sizeof(WindowClass);
  WindowClass.lpfnWndProc = DefWindowProcW;
  WindowClass.lpszClassName = GH_CLASS;
  RegisterClassExW(&WindowClass);
  HWND Result = CreateWindowExW(0, GH_CLASS, GH_TITLE, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  return Result;
}


int APIENTRY
WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
  enable_dpi_awareness();
  HWND Window = create_the_window();
  HDC WindowDC = GetDC(Window);
  HGLRC ContextGL = create_opengl_context(WindowDC);
  setup_raw_input(Window);

  LARGE_INTEGER Frequency = {0};
  QueryPerformanceFrequency(&Frequency);
  Engine engine(Frequency.QuadPart);
  GH_ENGINE = &engine;

  engine.load_scene(0);

  LARGE_INTEGER Ticks = {0};
  QueryPerformanceCounter(&Ticks);
  int64_t cur_ticks = Ticks.QuadPart;

  SetWindowLongPtrW(Window, GWL_STYLE, WS_OVERLAPPEDWINDOW);
  SetWindowLongPtrW(Window, GWL_EXSTYLE, WS_EX_APPWINDOW);
  SetWindowLongPtrW(Window, GWLP_USERDATA, (LONG_PTR)&engine);
  SetWindowLongPtrW(Window, GWLP_WNDPROC, (LONG_PTR)window_proc);
  SetWindowPos(Window, HWND_TOP, GH_SCREEN_X, GH_SCREEN_Y, 1280, 720, SWP_SHOWWINDOW);
  ShowCursor(!GH_HIDE_MOUSE);

  for(;;)
  {
    MSG Msg;
    while(PeekMessageW(&Msg, 0, 0, 0, PM_REMOVE))
    {
      if(Msg.message == WM_QUIT)
      {
        goto label_loop_exit;
      }
      TranslateMessage(&Msg);
      DispatchMessageW(&Msg);
    }
    confine_cursor(Window);
    QueryPerformanceCounter(&Ticks);
    int64_t new_ticks = Ticks.QuadPart;
    engine.do_frame(cur_ticks, new_ticks);
    SwapBuffers(WindowDC);
  }
label_loop_exit:
  engine.cleanup();
  ClipCursor(NULL);
  wglMakeCurrent(WindowDC, NULL);
  wglDeleteContext(ContextGL);
  //ReleaseDC(Window, WindowDC);
  //DestroyWindow(hWnd);
  return 0;
}
