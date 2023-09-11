static HGLRC
create_opengl_context(HDC WindowDC)
{
  PIXELFORMATDESCRIPTOR PFD = {0};
  PFD.nSize = sizeof(PFD);
  PFD.nVersion = 1;
  PFD.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  PFD.iPixelType = PFD_TYPE_RGBA;
  //PFD.cStencilBits = 8;
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
opengl_init(void)
{
  Assert(glewInit() == GLEW_OK);
  glClearColor(0.6f, 0.9f, 1.0f, 1.0f);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glDepthMask(GL_TRUE);
  wglSwapIntervalEXT(1);
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

static void
ToggleFullscreen(Engine* engine)
{
  //int WindowX = engine->isFullscreen ? CW_USEDEFAULT : 0;
  //int WindowY = engine->isFullscreen ? CW_USEDEFAULT : 0;
  //int WindowWidth = engine->isFullscreen ? engine->iWidth : GetSystemMetrics(SM_CXSCREEN);
  //int WindowHeight = engine->isFullscreen ? engine->iHeight : GetSystemMetrics(SM_CYSCREEN);
  //DWORD NewStyle = (WS_CLIPSIBLINGS | WS_CLIPCHILDREN) | engine->isFullscreen ? WS_OVERLAPPEDWINDOW : 0;
  //DWORD NewStyleEx = WS_EX_APPWINDOW | (engine->isFullscreen ? WS_EX_WINDOWEDGE : 0);
  engine->isFullscreen = !engine->isFullscreen;
  if(engine->isFullscreen)
  {
    engine->iWidth = GH_SCREEN_WIDTH;
    engine->iHeight = GH_SCREEN_HEIGHT;
    SetWindowLongPtrW(engine->hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
    SetWindowLongPtrW(engine->hWnd, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_WINDOWEDGE);
    SetWindowPos(engine->hWnd, HWND_TOP, GH_SCREEN_X, GH_SCREEN_Y, engine->iWidth, engine->iHeight, SWP_SHOWWINDOW);
  }
  else
  {
    engine->iWidth = GetSystemMetrics(SM_CXSCREEN);
    engine->iHeight = GetSystemMetrics(SM_CYSCREEN);
    SetWindowLongPtrW(engine->hWnd, GWL_STYLE, WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
    SetWindowLongPtrW(engine->hWnd, GWL_EXSTYLE, WS_EX_APPWINDOW);
    SetWindowPos(engine->hWnd, HWND_TOP, 0, 0, engine->iWidth, engine->iHeight, SWP_SHOWWINDOW);
  }
}

static bool_t
process_windows_messages(Engine* engine)
{
	MSG Msg;
	while(PeekMessageW(&Msg, 0, 0, 0, PM_REMOVE))
	{
		UINT Message = Msg.message;
		if(Message == WM_QUIT)
		{
			return true;
		}
		HWND Window = engine->hWnd;
		WPARAM wParam = Msg.wParam;
		LPARAM lParam = Msg.lParam;
		switch(Message)
		{
		  case WM_SIZE:
	    {
	      engine->iWidth = LOWORD(lParam);
	      engine->iHeight = HIWORD(lParam);
	      PostMessageW(Window, WM_PAINT, 0, 0);
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
	        engine->input.key[wParam & 0xFF] = true;
	        engine->input.key_press[wParam & 0xFF] = true;
	      }
	    } break;
	    case WM_SYSKEYDOWN:
	    {
	      if (wParam == VK_RETURN)
	      {
	        ToggleFullscreen(engine);
	      }
	    } break;
	    case WM_KEYUP:
	    {
	      engine->input.key[wParam & 0xFF] = false;
	    } break;
	    case WM_INPUT:
	    {
	      RAWINPUT RawInput;
	      UINT InputSize = sizeof(RawInput);
	      HRAWINPUT RawInputHandle = (HRAWINPUT)lParam;
	      GetRawInputData(RawInputHandle, RID_INPUT, (void*)&RawInput, &InputSize, sizeof(RAWINPUTHEADER));
	      engine->input.UpdateRaw(&RawInput);
	    } break;
	  }
	}
	return false;
}

static DWORD WINAPI
thread_render(LPVOID Parameter)
{
	HWND Window = (HWND)Parameter;
	setup_raw_input(Window);
	HDC WindowDC = GetDC(Window);
	HGLRC ContextGL = create_opengl_context(WindowDC);
	opengl_init();
	Engine engine;
  engine.iWidth = GH_SCREEN_WIDTH;
  engine.iHeight = GH_SCREEN_HEIGHT;
  engine.isFullscreen = GH_START_FULLSCREEN;
  engine.hWnd = Window;
  ToggleFullscreen(&engine);

  ShowCursor(!GH_HIDE_MOUSE);

  //ShowWindow(engine.hWnd, SW_SHOW);
  //SetForegroundWindow(engine.hWnd);
  //SetFocus(engine.hWnd);

  glGetQueryiv(GL_SAMPLES_PASSED_ARB, GL_QUERY_COUNTER_BITS_ARB, &engine.occlusionCullingSupported);
  engine.vObjects = std::vector<std::shared_ptr<Object>>();
  engine.vPortals = std::vector<std::shared_ptr<Portal>>();
  engine.sky = std::shared_ptr<Sky>(new Sky);
  engine.player = std::shared_ptr<Player>(new Player);
  engine.vScenes = std::vector<std::shared_ptr<Scene>>();
  engine.curScene = std::shared_ptr<Scene>();
  engine.input = {0};
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
  //SetWindowLongPtrW(engine.hWnd, GWLP_USERDATA, (LONG_PTR)&engine);

  //Setup the timer
  const int64_t ticks_per_step = engine.timer.SecondsToTicks(GH_DT);
  int64_t cur_ticks = engine.timer.GetTicks();
  GH_FRAME = 0;

  while(process_windows_messages(&engine))
  {
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
    SwapBuffers(WindowDC);
  }
  engine.DestroyGLObjects();
  ClipCursor(0);
  wglMakeCurrent(WindowDC, 0);
  wglDeleteContext(ContextGL);
  ReleaseDC(engine.hWnd, WindowDC);
  //DestroyWindow(hWnd);
  return 0;
}
