class Floorplan : public Rigid {
public:
  Floorplan() {
    object_reset(&Geom.Obj);
    Geom.mesh = AquireMesh("floorplan.obj");
    Geom.shader = AquireShader("texture_array");
    texture = AquireTexture("floorplan_textures.bmp", 4, 4);
    Geom.Obj.scale = Vector3(0.1524f); //6-inches to meters
  }
};
