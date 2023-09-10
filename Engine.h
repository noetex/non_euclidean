class Engine {
public:
  Engine();
  ~Engine();

  void Update();
  void Render(const Camera& cam, GLuint curFBO, const Portal* skipPortal);
  void LoadScene(int ix);

  LRESULT WindowProc(HWND hCurWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  const Player& GetPlayer() const { return *player; }
  float NearestPortalDist() const;

//private:
  HWND CreateGLWindow();
  void DestroyGLObjects();
  void SetupInputs();
  void ConfineCursor();
  void ToggleFullscreen();

  HWND  hWnd;				   // window
  HDC   hDC;           // device context
  HGLRC hRC;				   // opengl context

  GLint occlusionCullingSupported;

  std::vector<std::shared_ptr<Object>> vObjects;
  std::vector<std::shared_ptr<Portal>> vPortals;
  std::shared_ptr<Sky> sky;
  std::shared_ptr<Player> player;
  std::vector<std::shared_ptr<Scene>> vScenes;
  std::shared_ptr<Scene> curScene;
  Input input;
  bool isFullscreen;   // fullscreen state

  Camera main_cam;
  Timer timer;

  LONG iWidth;         // window width
  LONG iHeight;        // window height
};
