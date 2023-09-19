class House : public Rigid {
public:
  House(const char* tex) {
    object_reset(&Geom.Obj);
    Geom.mesh = AquireMesh("square_rooms.obj");
    Geom.shader = AquireShader("texture");
    texture = AquireTexture(tex);
    Geom.Obj.scale = Vector3(1.0f, 3.0f, 1.0f);
  }
  virtual ~House() {}

  void SetDoor1(Portal& portal)
  {
    Matrix4 ltw = object_local_to_world(&Geom.Obj);
    portal.Geom.Obj.pos = ltw.MulPoint(Vector3(4.0f, 0.5f, 10.0f));
    portal.Geom.Obj.euler = Geom.Obj.euler;
    portal.Geom.Obj.scale = Vector3(2, 0.5f, 1) * Geom.Obj.scale;
  }
  void SetDoor2(Portal& portal)
  {
    Matrix4 ltw = object_local_to_world(&Geom.Obj);
    portal.Geom.Obj.pos = ltw.MulPoint(Vector3(10.0f, 0.5f, 4.0f));
    portal.Geom.Obj.euler = Geom.Obj.euler;
    portal.Geom.Obj.euler.y -= GH_PI/2;
    portal.Geom.Obj.scale = Vector3(2, 0.5f, 1) * Geom.Obj.scale;
  }
  void SetDoor3(Portal& portal)
  {
    Matrix4 ltw = object_local_to_world(&Geom.Obj);
    portal.Geom.Obj.pos = ltw.MulPoint(Vector3(16.0f, 0.5f, 10.0f));
    portal.Geom.Obj.euler = Geom.Obj.euler;
    portal.Geom.Obj.euler.y -= GH_PI;
    portal.Geom.Obj.scale = Vector3(2, 0.5f, 1) * Geom.Obj.scale;
  }
  void SetDoor4(Portal& portal)
  {
    Matrix4 ltw = object_local_to_world(&Geom.Obj);
    portal.Geom.Obj.pos = ltw.MulPoint(Vector3(10.0f, 0.5f, 16.0f));
    portal.Geom.Obj.euler = Geom.Obj.euler;
    portal.Geom.Obj.euler.y -= GH_PI*3/2;
    portal.Geom.Obj.scale = Vector3(2, 0.5f, 1) * Geom.Obj.scale;
  }
};
