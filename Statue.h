class Statue : public Rigid {
public:
  Statue(const char* model) {
    object_reset(&Geom.Obj);
    Geom.mesh = AquireMesh(model);
    Geom.shader = AquireShader("texture");
    texture = AquireTexture("gold.bmp");
  }
};
