Engine* GH_ENGINE = nullptr;
Player* GH_PLAYER = nullptr;
const Input* GH_INPUT = nullptr;
int GH_REC_LEVEL = 0;
int64_t GH_FRAME = 0;

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

static LRESULT WINAPI
StaticWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  Engine* eng = (Engine*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
  if (eng) {
    return eng->WindowProc(hWnd, uMsg, wParam, lParam);
  }
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

Engine::~Engine() {
  ClipCursor(NULL);
  wglMakeCurrent(NULL, NULL);
  ReleaseDC(hWnd, hDC);
  wglDeleteContext(hRC);
  DestroyWindow(hWnd);
}

void Engine::LoadScene(int ix) {
  //Clear out old scene
  if (curScene) { curScene->Unload(); }
  vObjects.clear();
  vPortals.clear();
  player->Reset();

  //Create new scene
  curScene = vScenes[ix];
  curScene->Load(vObjects, vPortals, *player);
  vObjects.push_back(player);
}

void Engine::Update() {
  //Update
  for (size_t i = 0; i < vObjects.size(); ++i) {
    assert(vObjects[i].get());
    vObjects[i]->Update();
  }

  //Collisions
  //For each physics object
  for (size_t i = 0; i < vObjects.size(); ++i) {
    Physical* physical = vObjects[i]->AsPhysical();
    if (!physical) { continue; }
    Matrix4 worldToLocal = physical->WorldToLocal();

    //For each object to collide with
    for (size_t j = 0; j < vObjects.size(); ++j) {
      if (i == j) { continue; }
      Object& obj = *vObjects[j];
      if (!obj.mesh) { continue; }

      //For each hit sphere
      for (size_t s = 0; s < physical->hitSpheres.size(); ++s) {
        //Brings point from collider's local coordinates to hits's local coordinates.
        const Sphere& sphere = physical->hitSpheres[s];
        Matrix4 worldToUnit = sphere.LocalToUnit() * worldToLocal;
        Matrix4 localToUnit = worldToUnit * obj.LocalToWorld();
        Matrix4 unitToWorld = worldToUnit.Inverse();

        //For each collider
        for (size_t c = 0; c < obj.mesh->colliders.size(); ++c) {
          Vector3 push;
          const Collider& collider = obj.mesh->colliders[c];
          if (collider.Collide(localToUnit, push)) {
            //If push is too small, just ignore
            push = unitToWorld.MulDirection(push);
            vObjects[j]->OnHit(*physical, push);
            physical->OnCollide(*vObjects[j], push);

            worldToLocal = physical->WorldToLocal();
            worldToUnit = sphere.LocalToUnit() * worldToLocal;
            localToUnit = worldToUnit * obj.LocalToWorld();
            unitToWorld = worldToUnit.Inverse();
          }
        }
      }
    }
  }

  //Portals
  for (size_t i = 0; i < vObjects.size(); ++i) {
    Physical* physical = vObjects[i]->AsPhysical();
    if (physical) {
      for (size_t j = 0; j < vPortals.size(); ++j) {
        if (physical->TryPortal(*vPortals[j])) {
          break;
        }
      }
    }
  }
}

void Engine::Render(const Camera& cam, GLuint curFBO, const Portal* skipPortal) {
  //Clear buffers
  if (GH_USE_SKY) {
    glClear(GL_DEPTH_BUFFER_BIT);
    sky->Draw(cam);
  } else {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  //Create queries (if applicable)
  GLuint queries[GH_MAX_PORTALS];
  GLuint drawTest[GH_MAX_PORTALS];
  assert(vPortals.size() <= GH_MAX_PORTALS);
  if (occlusionCullingSupported) {
    glGenQueriesARB((GLsizei)vPortals.size(), queries);
  }

  //Draw scene
  for (size_t i = 0; i < vObjects.size(); ++i) {
    vObjects[i]->Draw(cam, curFBO);
  }

  //Draw portals if possible
  if (GH_REC_LEVEL > 0) {
    //Draw portals
    GH_REC_LEVEL -= 1;
    if (occlusionCullingSupported && GH_REC_LEVEL > 0) {
      glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
      glDepthMask(GL_FALSE);
      for (size_t i = 0; i < vPortals.size(); ++i) {
        if (vPortals[i].get() != skipPortal) {
          glBeginQueryARB(GL_SAMPLES_PASSED_ARB, queries[i]);
          vPortals[i]->DrawPink(cam);
          glEndQueryARB(GL_SAMPLES_PASSED_ARB);
        }
      }
      for (size_t i = 0; i < vPortals.size(); ++i) {
        if (vPortals[i].get() != skipPortal) {
          glGetQueryObjectuivARB(queries[i], GL_QUERY_RESULT_ARB, &drawTest[i]);
        }
      };
      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
      glDepthMask(GL_TRUE);
      glDeleteQueriesARB((GLsizei)vPortals.size(), queries);
    }
    for (size_t i = 0; i < vPortals.size(); ++i) {
      if (vPortals[i].get() != skipPortal) {
        if (occlusionCullingSupported && (GH_REC_LEVEL > 0) && (drawTest[i] == 0)) {
          continue;
        } else {
          vPortals[i]->Draw(cam, curFBO);
        }
      }
    }
    GH_REC_LEVEL += 1;
  }
  
#if 0
  //Debug draw colliders
  for (size_t i = 0; i < vObjects.size(); ++i) {
    vObjects[i]->DebugDraw(cam);
  }
#endif
}

LRESULT Engine::WindowProc(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
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
      iWidth = LOWORD(lParam);
      iHeight = HIWORD(lParam);
      PostMessage(Window, WM_PAINT, 0, 0);
    } break;
    case WM_KEYDOWN:
    {
      if (lParam & 0x40000000) { return 0; }
      input.key[wParam & 0xFF] = true;
      input.key_press[wParam & 0xFF] = true;
      if (wParam == VK_ESCAPE)
      {
        PostQuitMessage(0);
      }
    } break;
    case WM_SYSKEYDOWN:
    {
      if (wParam == VK_RETURN)
      {
        ToggleFullscreen();
      }
    } break;
    case WM_KEYUP:
    {
      input.key[wParam & 0xFF] = false;

    } break;
    case WM_INPUT:
    {
      BYTE lpb[256];
      UINT dwSize = sizeof(lpb);
      dwSize = sizeof(lpb);
      GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
      input.UpdateRaw((const RAWINPUT*)lpb);
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

HWND Engine::CreateGLWindow(void)
{
  HINSTANCE Instance = GetModuleHandle(NULL);
  WNDCLASSEX wc;
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = CS_OWNDC;
  wc.lpfnWndProc = (WNDPROC)StaticWindowProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = Instance;
  wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = NULL;
  wc.lpszMenuName = NULL;
  wc.lpszClassName = GH_CLASS;
  wc.hIconSm = NULL;
  RegisterClassEx(&wc);

  //Always start in windowed mode
  iWidth = GH_SCREEN_WIDTH;
  iHeight = GH_SCREEN_HEIGHT;

  //Create the window
  HWND Result = CreateWindowEx(
    WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,
    GH_CLASS,
    GH_TITLE,
    WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
    GH_SCREEN_X,
    GH_SCREEN_Y,
    iWidth,
    iHeight,
    NULL,
    NULL,
    Instance,
    NULL);

  if (GH_START_FULLSCREEN) {
    ToggleFullscreen();
  }
  if (GH_HIDE_MOUSE) {
    ShowCursor(FALSE);
  }

  ShowWindow(Result, SW_SHOW);
  SetForegroundWindow(Result);
  SetFocus(Result);
  return Result;
}

void Engine::DestroyGLObjects() {
  curScene->Unload();
  vObjects.clear();
  vPortals.clear();
}

float Engine::NearestPortalDist() const {
  float dist = FLT_MAX;
  for (size_t i = 0; i < vPortals.size(); ++i) {
    dist = GH_MIN(dist, vPortals[i]->DistTo(player->pos));
  }
  return dist;
}

void Engine::ToggleFullscreen() {
  isFullscreen = !isFullscreen;
  if (isFullscreen) {
    iWidth = GetSystemMetrics(SM_CXSCREEN);
    iHeight = GetSystemMetrics(SM_CYSCREEN);
    SetWindowLong(hWnd, GWL_STYLE, WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
    SetWindowLong(hWnd, GWL_EXSTYLE, WS_EX_APPWINDOW);
    SetWindowPos(hWnd, HWND_TOPMOST, 0, 0,
      iWidth, iHeight, SWP_SHOWWINDOW);
  } else {
    iWidth = GH_SCREEN_WIDTH;
    iHeight = GH_SCREEN_HEIGHT;
    SetWindowLong(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
    SetWindowLong(hWnd, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_WINDOWEDGE);
    SetWindowPos(hWnd, HWND_TOP, GH_SCREEN_X, GH_SCREEN_Y,
      iWidth, iHeight, SWP_SHOWWINDOW);
  }
}
