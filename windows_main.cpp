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

static void
process_raw_input(Input* input, RAWINPUT* RawInput)
{
  if (RawInput->header.dwType == RIM_TYPEMOUSE)
  {
    if (RawInput->data.mouse.usFlags == MOUSE_MOVE_RELATIVE)
    {
      input->mouse_ddx += RawInput->data.mouse.lLastX;
      input->mouse_ddy += RawInput->data.mouse.lLastY;
    }
    if (RawInput->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
    {
      input->mouse_button[0] = true;
      input->mouse_button_press[0] = true;
    }
    if (RawInput->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
    {
      input->mouse_button[1] = true;
      input->mouse_button_press[1] = true;
    }
    if (RawInput->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
    {
      input->mouse_button[2] = true;
      input->mouse_button_press[2] = true;
    }
    if (RawInput->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)
    {
      input->mouse_button[0] = false;
    }
    if (RawInput->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP)
    {
      input->mouse_button[1] = false;
    }
    if (RawInput->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP)
    {
      input->mouse_button[2] = false;
    }
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
        if (wParam == VK_ESCAPE)
        {
          PostQuitMessage(0);
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
      process_raw_input(&GH_ENGINE->input, &RawInput);
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

static void
InitGLObjects(void)
{
  glewInit();
  glClearColor(0.6f, 0.9f, 1.0f, 1.0f);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glDepthMask(GL_TRUE);
  //wglSwapIntervalEXT(1);
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
  InitGLObjects();
  setup_raw_input(Window);
  ShowCursor(!GH_HIDE_MOUSE);
  Engine engine = {0};

  glGetQueryiv(GL_SAMPLES_PASSED_ARB, GL_QUERY_COUNTER_BITS_ARB, &engine.occlusionCullingSupported);
  //engine.sky = new Sky;
  engine.sky = Sky();
  engine.player = Player_Ptr(new Player);
  engine.curScene = 0;
  engine.input = {0};
  engine.main_cam.width = 256;
  engine.main_cam.height = 256;
  engine.main_cam.worldView.MakeIdentity();
  engine.main_cam.projection.MakeIdentity();

  engine.vScenes.push_back(Scene_Ptr(new Level1));
  engine.vScenes.push_back(Scene_Ptr(new Level2(3)));
  engine.vScenes.push_back(Scene_Ptr(new Level2(6)));
  engine.vScenes.push_back(Scene_Ptr(new Level3));
  engine.vScenes.push_back(Scene_Ptr(new Level4));
  engine.vScenes.push_back(Scene_Ptr(new Level5));
  engine.vScenes.push_back(Scene_Ptr(new Level6));
  GH_ENGINE = &engine;
  GH_INPUT = &engine.input;
  engine.LoadScene(0);

  LARGE_INTEGER Ticks = {0};
  LARGE_INTEGER Frequency = {0};
  QueryPerformanceFrequency(&Frequency);
  const int64_t ticks_per_step = (int64_t)(Frequency.QuadPart * GH_DT);
  QueryPerformanceCounter(&Ticks);
  int64_t cur_ticks = Ticks.QuadPart;
  int64_t GH_FRAME = 0;

  SetWindowLongPtrW(Window, GWL_STYLE, WS_OVERLAPPEDWINDOW);
  SetWindowLongPtrW(Window, GWL_EXSTYLE, WS_EX_APPWINDOW);
  SetWindowLongPtrW(Window, GWLP_USERDATA, (LONG_PTR)&engine);
  SetWindowLongPtrW(Window, GWLP_WNDPROC, (LONG_PTR)window_proc);
  SetWindowPos(Window, HWND_TOP, GH_SCREEN_X, GH_SCREEN_Y, 1280, 720, SWP_SHOWWINDOW);

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
    engine.process_input();

    //Used fixed time steps for updates
    QueryPerformanceCounter(&Ticks);
    const int64_t new_ticks = Ticks.QuadPart;
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
    engine.main_cam.SetSize(GH_SCREEN_WIDTH, GH_SCREEN_HEIGHT, n, GH_FAR);
    engine.main_cam.UseViewport();

    //Render scene
    GH_REC_LEVEL = GH_MAX_RECURSION;
    engine.Render(engine.main_cam, 0, nullptr);
    SwapBuffers(WindowDC);
  }
label_loop_exit:
  engine.curScene->Unload();
  engine.vObjects.clear();
  engine.vPortals.clear();
  ClipCursor(NULL);
  wglMakeCurrent(WindowDC, NULL);
  wglDeleteContext(ContextGL);
  //ReleaseDC(Window, WindowDC);
  //DestroyWindow(hWnd);
  return 0;
}
