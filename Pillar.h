class Pillar : public Rigid {
public:
  Pillar() {
    Object::Reset();
    mesh = AquireMesh("pillar.obj");
    shader = AquireShader("texture");
    texture = AquireTexture("white.bmp");
    scale = Vector3(0.1f);
  }
  virtual ~Pillar() {}
};
