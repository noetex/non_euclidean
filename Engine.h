class Engine {
public:
  //Engine(GLint, int64_t);
  //void init_opengl(void);
  void do_frame(int64_t&, int64_t);
  void process_input(void);
  void Update();
  void Render(const Camera& cam, GLuint curFBO, const Portal* skipPortal);
  void LoadScene(int ix);

  float NearestPortalDist() const;

  GLint occlusionCullingSupported;

  std::vector<Object_Ptr> vObjects;
  std::vector<Portal_Ptr> vPortals;
  Sky sky;
  int64_t TicksPerStep;
  Player_Ptr player;
  std::vector<Scene_Ptr> vScenes;
  size_t CurrentSceneIndex;
  Input input;
  int64_t GH_FRAME;
  Camera main_cam;
};
