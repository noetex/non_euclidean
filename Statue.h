class Statue : public Rigid {
public:
  Statue(const char* model) {
    Geom.Obj.reset();
    Geom.mesh = AquireMesh(model);
    Geom.shader = AquireShader("texture");
    texture = AquireTexture("gold.bmp");
  }
};
