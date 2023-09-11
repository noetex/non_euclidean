class Engine {
public:
  void Update();
  void Render(const Camera& cam, GLuint curFBO, const Portal* skipPortal);
  void LoadScene(int ix);

  const Player& GetPlayer() const { return *player; }
  float NearestPortalDist() const;

//private:
  void DestroyGLObjects();

  LONG iWidth;
  LONG iHeight;
  //LONG FullscreenWidth;
  //LONG FullscreenHeight;
  bool isFullscreen;

  HWND  hWnd;

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
