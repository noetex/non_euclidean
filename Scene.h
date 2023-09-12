class Scene {
public:
  virtual void Load(PObjectVec& objs, PPortalVec& portals, Player& player)=0;
  virtual void Unload() {};
};

typedef std::shared_ptr<Scene> Scene_Ptr;
