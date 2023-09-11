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
create_debug_console(void)
{
#if _DEBUG
  AllocConsole();
  HWND Console = GetConsoleWindow();
  //SetWindowPos(Console, 0, 1920, 200, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
  DWORD ProcessID = GetCurrentProcessId();
  AttachConsole(ProcessID);
  freopen("CON", "w", stdout);
#endif
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
  Engine* engine = (Engine*)GetWindowLongPtrW(Window, GWLP_USERDATA);
  if(!engine)
  {
    return DefWindowProcW(Window, Message, wParam, lParam);
  }
  LRESULT Result = 0;
  switch(Message)
  {
    //case WM_CREATE:
    //{
    //  CREATESTRUCT* Struct = (CREATESTRUCT*)lParam;
    //  Engine* engine = (Engine*)Struct->lpCreateParams;
    //  SetWindowLongPtrW(Window, GWLP_USERDATA, (LONG_PTR)engine);
    //} break;
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
      engine->iWidth = LOWORD(lParam);
      engine->iHeight = HIWORD(lParam);
      PostMessageW(Window, WM_PAINT, 0, 0);
    } break;
    case WM_KEYDOWN:
    {
      if (lParam & 0x40000000) { return 0; }
      engine->input.key[wParam & 0xFF] = true;
      engine->input.key_press[wParam & 0xFF] = true;
      if (wParam == VK_ESCAPE)
      {
        PostQuitMessage(0);
      }
    } break;
    case WM_SYSKEYDOWN:
    {
      if (wParam == VK_RETURN)
      {
        engine->ToggleFullscreen();
      }
    } break;
    case WM_KEYUP:
    {
      engine->input.key[wParam & 0xFF] = false;
    } break;
    case WM_INPUT:
    {
      BYTE lpb[256];
      UINT dwSize = sizeof(lpb);
      dwSize = sizeof(lpb);
      GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
      engine->input.UpdateRaw((const RAWINPUT*)lpb);
    } break;
    case WM_CLOSE:
    {
      PostQuitMessage(0);
    } break;
    default:
    {
      Result = DefWindowProc(Window, Message, wParam, lParam);
    }
  }
  return Result;
}

static void
setup_raw_input(HWND Window)
{
  RAWINPUTDEVICE Devices[3];
  Devices[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
  Devices[0].usUsage = HID_USAGE_GENERIC_MOUSE;
  Devices[0].dwFlags = RIDEV_INPUTSINK;
  Devices[0].hwndTarget = Window;

  Devices[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
  Devices[1].usUsage = HID_USAGE_GENERIC_JOYSTICK;
  Devices[1].dwFlags = 0;
  Devices[1].hwndTarget = 0;

  Devices[2].usUsagePage = HID_USAGE_PAGE_GENERIC;
  Devices[2].usUsage = HID_USAGE_GENERIC_GAMEPAD;
  Devices[2].dwFlags = 0;
  Devices[2].hwndTarget = 0;

  RegisterRawInputDevices(Devices, 3, sizeof(*Devices));
}

static GLint
InitGLObjects(void)
{
  glewInit();
  glClearColor(0.6f, 0.9f, 1.0f, 1.0f);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glDepthMask(GL_TRUE);
  GLint Result;
  glGetQueryiv(GL_SAMPLES_PASSED_ARB, GL_QUERY_COUNTER_BITS_ARB, &Result);
  wglSwapIntervalEXT(1);
  return Result;
}

static HWND
create_the_window(Engine* engine)
{
  HINSTANCE Instance = GetModuleHandleW(0);
  WNDCLASSEXW WindowClass = {0};
  WindowClass.cbSize = sizeof(WindowClass);
  WindowClass.style = CS_OWNDC;
  WindowClass.lpfnWndProc = window_proc;
  WindowClass.hInstance = Instance;
  WindowClass.lpszClassName = GH_CLASS;
  RegisterClassExW(&WindowClass);
  engine->iWidth = GH_SCREEN_WIDTH;
  engine->iHeight = GH_SCREEN_HEIGHT;
  DWORD WindowStyleEx = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
  DWORD WindowStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
  HWND Result = CreateWindowExW(WindowStyleEx, GH_CLASS, GH_TITLE, WindowStyle, GH_SCREEN_X, GH_SCREEN_Y, engine->iWidth, engine->iHeight, 0, 0, Instance, 0);
  return Result;
}

int APIENTRY
WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
  enable_dpi_awareness();
  create_debug_console();

  Engine engine;
  HWND Window = create_the_window(&engine);

  //engine.hWnd = engine.CreateGLWindow();
  engine.hWnd = Window;
  if (GH_START_FULLSCREEN) {
    engine.ToggleFullscreen();
  }
  if (GH_HIDE_MOUSE) {
    ShowCursor(FALSE);
  }

  ShowWindow(Window, SW_SHOW);
  SetForegroundWindow(Window);
  SetFocus(Window);

  engine.hDC = GetDC(engine.hWnd);
  engine.hRC = create_opengl_context(engine.hDC);
  engine.occlusionCullingSupported = InitGLObjects();
  engine.vObjects = std::vector<std::shared_ptr<Object>>();
  engine.vPortals = std::vector<std::shared_ptr<Portal>>();
  engine.sky = std::shared_ptr<Sky>(new Sky);
  engine.player = std::shared_ptr<Player>(new Player);
  engine.vScenes = std::vector<std::shared_ptr<Scene>>();
  engine.curScene = std::shared_ptr<Scene>();
  engine.input = {0};
  engine.isFullscreen = false;
  setup_raw_input(engine.hWnd);
  engine.main_cam.width = 256;
  engine.main_cam.height = 256;
  engine.main_cam.worldView.MakeIdentity();
  engine.main_cam.projection.MakeIdentity();
  QueryPerformanceFrequency(&engine.timer.frequency);

  engine.vScenes.push_back(std::shared_ptr<Scene>(new Level1));
  engine.vScenes.push_back(std::shared_ptr<Scene>(new Level2(3)));
  engine.vScenes.push_back(std::shared_ptr<Scene>(new Level2(6)));
  engine.vScenes.push_back(std::shared_ptr<Scene>(new Level3));
  engine.vScenes.push_back(std::shared_ptr<Scene>(new Level4));
  engine.vScenes.push_back(std::shared_ptr<Scene>(new Level5));
  engine.vScenes.push_back(std::shared_ptr<Scene>(new Level6));
  GH_ENGINE = &engine;
  GH_INPUT = &engine.input;
  GH_PLAYER = engine.player.get();





  engine.LoadScene(0);
  SetWindowLongPtrW(engine.hWnd, GWLP_USERDATA, (LONG_PTR)&engine);

  //Setup the timer
  const int64_t ticks_per_step = engine.timer.SecondsToTicks(GH_DT);
  int64_t cur_ticks = engine.timer.GetTicks();
  GH_FRAME = 0;

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
    confine_cursor(engine.hWnd);

    if (engine.input.key_press['1']) {
      engine.LoadScene(0);
    } else if (engine.input.key_press['2']) {
      engine.LoadScene(1);
    } else if (engine.input.key_press['3']) {
      engine.LoadScene(2);
    } else if (engine.input.key_press['4']) {
      engine.LoadScene(3);
    } else if (engine.input.key_press['5']) {
      engine.LoadScene(4);
    } else if (engine.input.key_press['6']) {
      engine.LoadScene(5);
    } else if (engine.input.key_press['7']) {
      engine.LoadScene(6);
    }

    //Used fixed time steps for updates
    const int64_t new_ticks = engine.timer.GetTicks();
    for (int i = 0; cur_ticks < new_ticks && i < GH_MAX_STEPS; ++i) {
      engine.Update();
      cur_ticks += ticks_per_step;
      GH_FRAME += 1;
      engine.input.EndFrame();
    }
    cur_ticks = (cur_ticks < new_ticks ? new_ticks: cur_ticks);

    //Setup camera for rendering
    const float n = GH_CLAMP(engine.NearestPortalDist() * 0.5f, GH_NEAR_MIN, GH_NEAR_MAX);
    engine.main_cam.worldView = engine.player->WorldToCam();
    engine.main_cam.SetSize(engine.iWidth, engine.iHeight, n, GH_FAR);
    engine.main_cam.UseViewport();

    //Render scene
    GH_REC_LEVEL = GH_MAX_RECURSION;
    engine.Render(engine.main_cam, 0, nullptr);
    SwapBuffers(engine.hDC);
  }
label_loop_exit:
  engine.DestroyGLObjects();
  return 0;
}
