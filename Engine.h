class Engine {
public:
  Engine(int64_t);
  void cleanup(void);
  void do_frame(int64_t&, int64_t);
  void Update();
  void Render(const Camera& cam, GLuint curFBO, const Portal* skipPortal);
  void load_scene(size_t);
  float NearestPortalDist() const;

  std::vector<Rigid*> vObjects;
  std::vector<Portal*> vPortals;
  Player player;
  int64_t TicksPerStep;
  GLint occlusionCullingSupported;
  Input input;
  int64_t GH_FRAME;
  Camera main_cam;
  Sky sky;
};
