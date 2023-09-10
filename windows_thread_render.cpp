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

static GLint
InitGLObjects(void)
{
  assert(glewInit() == GLEW_OK);
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
process_windows_message(Engine* engine, UINT Message, WPARAM wParam, LPARAM lParam)
{
	HWND Window = engine->hWnd;
  switch(Message)
  {
  	case WM_PAINT:
  	{
  		PAINTSTRUCT PaintStruct;
  	  HDC WindowDC = BeginPaint(Window, &PaintStruct);
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
	    if (lParam & 0x40000000) { return; }
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
  }
}

static DWORD WINAPI
render_thread(LPVOID Parameter)
{
	wglMakeCurrent(0, 0);
	HWND Window = (HWND)Parameter;
	HDC WindowDC = GetDC(Window);
	HGLRC OpenGLContext = create_opengl_context(WindowDC);
	GLint Supported = InitGLObjects();
	Engine engine(Window, WindowDC, OpenGLContext, Supported);
	setup_raw_input(Window);

	if (GH_START_FULLSCREEN)
	{
    engine.ToggleFullscreen();
  }
  if (GH_HIDE_MOUSE)
  {
    ShowCursor(FALSE);
  }

  ShowWindow(Window, SW_SHOW);
  SetForegroundWindow(Window);
  SetFocus(Window);
  int ExitCode = engine.Run();
  return ExitCode;
#if 0
	for(;;)
	{
		MSG Message;
		while(PeekMessageW(&Message, 0, 0, 0, PM_REMOVE))
		{
			if(Message.message == WM_QUIT)
			{
				goto label_exit;
			}
			process_windows_message(&engine, Message.message, Message.wParam, Message.lParam);
		}
	}
label_exit:
	return 0;
#endif
}
