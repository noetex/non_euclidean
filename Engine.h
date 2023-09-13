class Engine {
public:
  Engine(int64_t);
  //void init_opengl(void);
  void cleanup(void);
  void do_frame(int64_t&, int64_t);
  void process_input(void);
  void Update();
  void Render(const Camera& cam, GLuint curFBO, const Portal* skipPortal);
  void LoadScene(size_t);

  float NearestPortalDist() const;


  std::vector<Object_Ptr> vObjects;
  std::vector<Portal_Ptr> vPortals;
  std::vector<Scene*> vScenes;
  size_t CurrentSceneIndex;
  Player_Ptr player;
  int64_t TicksPerStep;
  GLint occlusionCullingSupported;
  Input input;
  int64_t GH_FRAME;
  Camera main_cam;
  Sky sky;
};
