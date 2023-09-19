class Statue : public Rigid {
public:
  Statue(const char* model) {
    Object::Reset();
    mesh = AquireMesh(model);
    shader = AquireShader("texture");
    texture = AquireTexture("gold.bmp");
  }
  virtual ~Statue() {}
};
