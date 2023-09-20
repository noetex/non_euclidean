class PillarRoom : public Rigid {
public:
  PillarRoom()
  {
    Geom.Obj.reset();
    Geom.mesh = AquireMesh("pillar_room.obj");
    Geom.shader = AquireShader("texture");
    texture = AquireTexture("three_room.bmp");
    Geom.Obj.scale = Vector3(1.1f);
  }

  void SetPortal(Portal& portal)
  {
    Matrix4 ltw = Geom.Obj.local_to_world();
    portal.Geom.Obj.pos = ltw.MulPoint(Vector3(0, 1.5f, -1));
    portal.Geom.Obj.euler = Geom.Obj.euler;
    portal.Geom.Obj.euler.y -= GH_PI / 2;
    portal.Geom.Obj.scale = Vector3(1, 1.5f, 1) * Geom.Obj.scale;
  }
};
