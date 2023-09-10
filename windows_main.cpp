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
  enable_dpi_awareness();
  create_debug_console();
  //HWND Window = create_the_window();
  Engine engine;

#if 0
  engine.hWnd = this->CreateGLWindow();
  engine.hDC = GetDC(this->hWnd);
  engine.hRC = create_opengl_context(this->hDC);
  engine.occlusionCullingSupported = InitGLObjects();
  engine.vObjects = std::vector<std::shared_ptr<Object>>();
  engine.vPortals = std::vector<std::shared_ptr<Portal>>();
  engine.sky = new Sky;
  engine.player = new Player;
  engine.vScenes = std::vector<std::shared_ptr<Scene>>();
  engine.curScene = std::shared_ptr<Scene>();
  engine.input = {0};
  engine.isFullscreen = false;
  setup_raw_input(engine.hWnd);
{
  this->main_cam.width = 256;
  this->main_cam.height = 256;
  this->main_cam.worldView.MakeIdentity();
  this->main_cam.projection.MakeIdentity();
  QueryPerformanceFrequency(&this->timer.frequency);
  SetupInputs();

  vScenes.push_back(std::shared_ptr<Scene>(new Level1));
  vScenes.push_back(std::shared_ptr<Scene>(new Level2(3)));
  vScenes.push_back(std::shared_ptr<Scene>(new Level2(6)));
  vScenes.push_back(std::shared_ptr<Scene>(new Level3));
  vScenes.push_back(std::shared_ptr<Scene>(new Level4));
  vScenes.push_back(std::shared_ptr<Scene>(new Level5));
  vScenes.push_back(std::shared_ptr<Scene>(new Level6));
  GH_ENGINE = this;
  GH_INPUT = &this->input;
  GH_PLAYER = this->player.get();
}
#endif





  engine.LoadScene(0);
  SetWindowLongPtr(engine.hWnd, GWLP_USERDATA, (LONG_PTR)&engine);

  //Setup the timer
  const int64_t ticks_per_step = engine.timer.SecondsToTicks(GH_DT);
  int64_t cur_ticks = engine.timer.GetTicks();
  GH_FRAME = 0;

  //Game loop
  MSG msg;
  for(;;)
  {
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      //Handle windows messages
      if (msg.message == WM_QUIT) {
        break;
      } else {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    } else {
      //Confine the cursor
      engine.ConfineCursor();

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
  }

  engine.DestroyGLObjects();
  return 0;
}
