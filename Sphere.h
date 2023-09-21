class Sphere {
public:
  Sphere(float r=1.0f) : center(0.0f), radius(r) {}
  Sphere(const Vector3& pos, float r) : center(pos), radius(r) {}

  Matrix4 LocalToUnit() const {
    Assert(radius > 0.0f);
    return Matrix4::Scale(1.0f / radius) * Matrix4::Trans(-center);
  }

  Vector3 center;
  float radius;
};
