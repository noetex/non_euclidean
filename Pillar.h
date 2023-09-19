class Pillar : public Rigid {
public:
  Pillar()
  {
    object_reset(&Geom.Obj);
    Geom.mesh = AquireMesh("pillar.obj");
    Geom.shader = AquireShader("texture");
    texture = AquireTexture("white.bmp");
    Geom.Obj.scale = Vector3(0.1f);
  }
};
