void Physical::OnCollide(const Vector3& push) {
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
