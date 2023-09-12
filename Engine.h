class Engine {
public:
  void process_input(void);
  void Update();
  void Render(const Camera& cam, GLuint curFBO, const Portal* skipPortal);
  void LoadScene(int ix);

  float NearestPortalDist() const;

  GLint occlusionCullingSupported;

  std::vector<std::shared_ptr<Object>> vObjects;
  std::vector<std::shared_ptr<Portal>> vPortals;
  Sky* sky;
  std::shared_ptr<Player> player;
  std::vector<std::shared_ptr<Scene>> vScenes;
  std::shared_ptr<Scene> curScene;
  Input input;

  Camera main_cam;
};
