class Engine {
public:
  //void init_opengl(void);
  void process_input(void);
  void Update();
  void Render(const Camera& cam, GLuint curFBO, const Portal* skipPortal);
  void LoadScene(int ix);

  float NearestPortalDist() const;

  GLint occlusionCullingSupported;

  std::vector<Object_Ptr> vObjects;
  std::vector<Portal_Ptr> vPortals;
  Sky sky;
  Player_Ptr player;
  std::vector<Scene_Ptr> vScenes;
  Scene* curScene;
  Input input;

  Camera main_cam;
};
