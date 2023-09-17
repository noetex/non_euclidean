class Portal : public Object
{
public:
  struct Warp {
    Warp(const Portal* fromPortal) : fromPortal(fromPortal), toPortal(nullptr) {
      delta.MakeIdentity();
      deltaInv.MakeIdentity();
    }

    Matrix4 delta;
    Matrix4 deltaInv;
    const Portal* fromPortal;
    const Portal* toPortal;
  };

  Portal();

  virtual void Draw(const Camera& cam, GLuint curFBO) override;
  void DrawPink(const Camera& cam);

  Vector3 GetBump(const Vector3& a) const;
  const Warp* Intersects(const Vector3& a, const Vector3& b, const Vector3& bump) const;
  float DistTo(const Vector3& pt) const;

  static void Connect(Portal* a, Portal* b);
  static void Connect(Warp& a, Warp& b);

  Warp front;
  Warp back;

private:
  Shader_Ptr errShader;
  FrameBuffer frameBuf[GH_MAX_RECURSION <= 1 ? 1 : GH_MAX_RECURSION - 1];
};
