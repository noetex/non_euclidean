Portal::Portal() : front(this), back(this) {
  object_reset(&Geom.Obj);
  Geom.mesh = AquireMesh("double_quad.obj");
  Geom.shader = AquireShader("portal");
  errShader = AquireShader("pink");
}

Vector3 Portal::Forward() const {
  return -(Matrix4::RotZ(Geom.Obj.euler.z) * Matrix4::RotX(Geom.Obj.euler.x) * Matrix4::RotY(Geom.Obj.euler.y)).ZAxis();
}

void Portal::DrawPink(const Camera& cam) {
  const Matrix4 mv = object_local_to_world(&Geom.Obj);
  const Matrix4 mvp = cam.Matrix() * mv;
  errShader->Use();
  errShader->SetMVP(mvp.m, mv.m);
  Geom.mesh->Draw();
}

Vector3 Portal::GetBump(const Vector3& a) const {
  const Vector3 n = Forward();
  return n * ((a - Geom.Obj.pos).Dot(n) > 0 ? 1.0f : -1.0f);
}

const Portal::Warp* Portal::Intersects(const Vector3& a, const Vector3& b, const Vector3& bump)
{
  const Vector3 n = Forward();
  const Vector3 p = Geom.Obj.pos + bump;
  const float da = n.Dot(a - p);
  const float db = n.Dot(b - p);
  if (da * db > 0.0f) {
    return nullptr;
  }
  const Matrix4 m = object_local_to_world(&Geom.Obj);
  const Vector3 d = a + (b - a) * (da / (da - db)) - p;
  const Vector3 x = (m * Vector4(1, 0, 0, 0)).XYZ();
  if (std::abs(d.Dot(x)) >= x.Dot(x)) {
    return nullptr;
  }
  const Vector3 y = (m * Vector4(0, 1, 0, 0)).XYZ();
  if (std::abs(d.Dot(y)) >= y.Dot(y)) {
    return nullptr;
  }
  return (da > 0.0f ? &front : &back);
}

float Portal::DistTo(const Vector3& pt)
{
  //Get world delta
  const Matrix4 localToWorld = object_local_to_world(&Geom.Obj);
  const Vector3 v = pt - localToWorld.Translation();

  //Get axes
  const Vector3 x = localToWorld.XAxis();
  const Vector3 y = localToWorld.YAxis();

  //Find closest point
  const float px = GH_CLAMP(v.Dot(x) / x.MagSq(), -1.0f, 1.0f);
  const float py = GH_CLAMP(v.Dot(y) / y.MagSq(), -1.0f, 1.0f);
  const Vector3 closest = x*px + y*py;

  //Calculate distance to closest point
  return (v - closest).Mag();
}

void Portal::Connect(Portal* a, Portal* b) {
  Connect(a->front, b->back);
  Connect(b->front, a->back);
}

void Portal::Connect(Warp& a, Warp& b) {
  a.toPortal = b.fromPortal;
  b.toPortal = a.fromPortal;
  Matrix4 a_ltw = object_local_to_world(&a.fromPortal->Geom.Obj);
  Matrix4 a_wtl = object_world_to_local(&a.fromPortal->Geom.Obj);
  Matrix4 b_ltw = object_local_to_world(&b.fromPortal->Geom.Obj);
  Matrix4 b_wtl = object_world_to_local(&b.fromPortal->Geom.Obj);
  a.delta = a_ltw * b_wtl;
  b.delta = b_ltw * a_wtl;
  a.deltaInv = b.delta;
  b.deltaInv = a.delta;
}
