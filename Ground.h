class Ground : public Rigid {
public:
  Ground(bool slope=false)
  {
    Geom.Obj.reset();
    if (slope)
    {
      Geom.mesh = AquireMesh("ground_slope.obj");
    }
    else
    {
      Geom.mesh = AquireMesh("ground.obj");
    }
    Geom.shader = AquireShader("texture");
    texture = AquireTexture("checker_green.bmp");
    Geom.Obj.scale = Vector3(10, 1, 10);
  }
};
