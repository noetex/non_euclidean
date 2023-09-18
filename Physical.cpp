Physical::Physical() {
  Reset();
}

void Physical::Reset() {
  Object::Reset();
  velocity.SetZero();
  gravity.Set(0.0f, GH_GRAVITY, 0.0f);
  bounce = 0.0f;
  friction = 0.0f;
  high_friction = 0.0f;
  drag = 0.0f;
  prev_pos.SetZero();
  p_scale = 1;
}

void Physical::Update() {
  prev_pos = pos;
  velocity += gravity * p_scale * GH_DT;
  velocity *= (1.0f - drag);
  pos += velocity * GH_DT;
}

Matrix4 Physical::LocalToWorld() const {
  return Matrix4::Trans(pos) * Matrix4::RotY(euler.y) * Matrix4::RotX(euler.x) * Matrix4::RotZ(euler.z) * Matrix4::Scale(scale * p_scale);
}

Matrix4 Physical::WorldToLocal() const {
  return Matrix4::Scale(1.0f / (scale * p_scale)) * Matrix4::RotZ(-euler.z) * Matrix4::RotX(-euler.x) * Matrix4::RotY(-euler.y) * Matrix4::Trans(-pos);
}

void Physical::OnCollide(Object& other, const Vector3& push) {
  //Update position to avoid collision
  pos += push;

  //Ignore push if delta is too small
  if (push.MagSq() < 1e-8f * p_scale) {
    return;
  }

  //Calculate kinetic friction
  float kinetic_friction = friction;
  if (high_friction > 0.0f) {
    const float vel_ratio = velocity.Mag() / (high_friction * p_scale);
    kinetic_friction = GH_MIN(friction * (vel_ratio + 5.0f) / (vel_ratio + 1.0f), 1.0f);
  }

  //Update velocity to react to collision
  const Vector3 push_proj = push * (velocity.Dot(push) / push.Dot(push));
  velocity = (velocity - push_proj) * (1.0f - kinetic_friction) - push_proj * bounce;
}
