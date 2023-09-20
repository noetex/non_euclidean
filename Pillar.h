class Pillar : public Rigid {
public:
  Pillar()
  {
    Geom.Obj.reset();
    Geom.mesh = AquireMesh("pillar.obj");
    Geom.shader = AquireShader("texture");
    texture = AquireTexture("white.bmp");
    Geom.Obj.scale = Vector3(0.1f);
  }
};
