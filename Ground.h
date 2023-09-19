class Ground : public Rigid {
public:
  Ground(bool slope=false) {
    Object::Reset();
    if (slope) {
      mesh = AquireMesh("ground_slope.obj");
    } else {
      mesh = AquireMesh("ground.obj");
    }
    shader = AquireShader("texture");
    texture = AquireTexture("checker_green.bmp");
    scale = Vector3(10, 1, 10);
  }
  virtual ~Ground() {}
};
