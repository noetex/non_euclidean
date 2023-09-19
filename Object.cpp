void Object::Reset() {
  pos.SetZero();
  euler.SetZero();
  scale.SetOnes();
}

Matrix4 Object::LocalToWorld() const {
  return Matrix4::Trans(pos) * Matrix4::RotY(euler.y) * Matrix4::RotX(euler.x) * Matrix4::RotZ(euler.z) * Matrix4::Scale(scale);
}

Matrix4 Object::WorldToLocal() const {
  return Matrix4::Scale(1.0f / (scale)) * Matrix4::RotZ(-euler.z) * Matrix4::RotX(-euler.x) * Matrix4::RotY(-euler.y) * Matrix4::Trans(-pos);
}
