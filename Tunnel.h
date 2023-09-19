class Tunnel : public Rigid {
public:
  enum Type {
    NORMAL = 0,
    SCALE = 1,
    SLOPE = 2,
  };

  Tunnel(Type type) : type(type) {
    object_reset(&Geom.Obj);
    if (type == SCALE) {
      Geom.mesh = AquireMesh("tunnel_scale.obj");
    } else if (type == SLOPE) {
      Geom.mesh = AquireMesh("tunnel_slope.obj");
    } else {
      Geom.mesh = AquireMesh("tunnel.obj");
    }
    Geom.shader = AquireShader("texture");
    texture = AquireTexture("checker_gray.bmp");
  }

  void SetDoor1(Portal& portal)
  {
    Matrix4 ltw = object_local_to_world(&Geom.Obj);
    portal.Geom.Obj.pos = ltw.MulPoint(Vector3(0, 1, 1));
    portal.Geom.Obj.euler = Geom.Obj.euler;
    portal.Geom.Obj.scale = Vector3(0.6f, 0.999f, 1) * Geom.Obj.scale.x;
  }
  void SetDoor2(Portal& portal)
  {
    Matrix4 ltw = object_local_to_world(&Geom.Obj);
    portal.Geom.Obj.euler = Geom.Obj.euler;
    if (type == SCALE) {
      portal.Geom.Obj.pos = ltw.MulPoint(Vector3(0, 0.5f, -1));
      portal.Geom.Obj.scale = Vector3(0.3f, 0.499f, 0.5f) * Geom.Obj.scale.x;
    } else if (type == SLOPE) {
      portal.Geom.Obj.pos = ltw.MulPoint(Vector3(0, -1, -1));
      portal.Geom.Obj.scale = Vector3(0.6f, 0.999f, 1) * Geom.Obj.scale.x;
    } else {
      portal.Geom.Obj.pos = ltw.MulPoint(Vector3(0, 1, -1));
      portal.Geom.Obj.scale = Vector3(0.6f, 0.999f, 1) * Geom.Obj.scale.x;
    }
  }

private:
  Type type;
};
