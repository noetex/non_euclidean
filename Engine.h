class Engine {
public:
  ~Engine();

  void Update();
  void Render(const Camera& cam, GLuint curFBO, const Portal* skipPortal);
  void LoadScene(int ix);

  const Player& GetPlayer() const { return *player; }
  float NearestPortalDist() const;

//private:
  void DestroyGLObjects();
  void ToggleFullscreen();

  LONG iWidth;
  LONG iHeight;
  bool isFullscreen;

  HWND  hWnd;
  HDC   hDC;
  HGLRC hRC;

  GLint occlusionCullingSupported;

  std::vector<std::shared_ptr<Object>> vObjects;
  std::vector<std::shared_ptr<Portal>> vPortals;
  std::shared_ptr<Sky> sky;
  std::shared_ptr<Player> player;
  std::vector<std::shared_ptr<Scene>> vScenes;
  std::shared_ptr<Scene> curScene;
  Input input;

  Camera main_cam;
  Timer timer;
};
