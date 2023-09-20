class Portal
{
public:
  struct Warp
  {
    Warp(Portal* fromPortal) : fromPortal(fromPortal), toPortal(nullptr) {
      delta.MakeIdentity();
      deltaInv.MakeIdentity();
    }

    Matrix4 delta;
    Matrix4 deltaInv;
    Portal* fromPortal;
    Portal* toPortal;
  };

  Portal();

  void DrawPink(const Camera& cam);
  Vector3 Forward() const;


  Vector3 GetBump(const Vector3& a) const;
  const Warp* Intersects(const Vector3& a, const Vector3& b, const Vector3& bump);
  float DistTo(const Vector3& pt);

  static void Connect(Portal* a, Portal* b);
  static void Connect(Warp& a, Warp& b);

  Warp front;
  Warp back;

//private:
  geometric_props Geom;
  Shader_Ptr errShader;
  FrameBuffer frameBuf[GH_MAX_RECURSION <= 1 ? 1 : GH_MAX_RECURSION - 1];
};
