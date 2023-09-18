Player::Player() {
  Reset();
  hitSpheres.push_back(Sphere(Vector3(0, 0, 0), GH_PLAYER_RADIUS));
  hitSpheres.push_back(Sphere(Vector3(0, GH_PLAYER_RADIUS - GH_PLAYER_HEIGHT, 0), GH_PLAYER_RADIUS));
}

void Player::Reset() {
  Object::Reset();
  velocity.SetZero();
  gravity.Set(0.0f, GH_GRAVITY, 0.0f);
  bounce = 0.0f;
  high_friction = 0.0f;
  prev_pos.SetZero();
  p_scale = 1;
  cam_rx = 0.0f;
  cam_ry = 0.0f;
  bob_mag = 0.0f;
  bob_phi = 0.0f;
  friction = 0.04f;
  drag = 0.002f;
  onGround = true;
}

void Player::Update() {
  //Update bobbing motion
  float magT = (prev_pos - pos).Mag() / (GH_DT * p_scale);
  if (!onGround) { magT = 0.0f; }
  bob_mag = bob_mag*(1.0f - GH_BOB_DAMP) + magT*GH_BOB_DAMP;
  if (bob_mag < GH_BOB_MIN) {
    bob_phi = 0.0f;
  } else {
    bob_phi += GH_BOB_FREQ * GH_DT;
    if (bob_phi > 2 * GH_PI) {
      bob_phi -= 2 * GH_PI;
    }
  }

  //Physics
  Input* input = &GH_ENGINE->input;
  prev_pos = pos;
  velocity += gravity * p_scale * GH_DT;
  velocity *= (1.0f - drag);
  pos += velocity * GH_DT;

  //Looking
  Look(input->mouse_dx, input->mouse_dy);

  //Movement
  float moveF = 0.0f;
  float moveL = 0.0f;
  if (input->key['W']) {
    moveF += 1.0f;
  }
  if (input->key['S']) {
    moveF -= 1.0f;
  }
  if (input->key['A']) {
    moveL += 1.0f;
  }
  if (input->key['D']) {
    moveL -= 1.0f;
  }
  Move(moveF, moveL);

#if 1
  //Jumping
  if (onGround && input->key[VK_SPACE]) {
    velocity.y += 2.0f * p_scale;
  }
#endif

  //Reset ground state after update finishes
  onGround = false;
}

void Player::Look(float mouseDx, float mouseDy) {
  //Adjust x-axis rotation
  cam_rx -= mouseDy * GH_MOUSE_SENSITIVITY;
  if (cam_rx > GH_PI / 2) {
    cam_rx = GH_PI / 2;
  } else if (cam_rx < -GH_PI / 2) {
    cam_rx = -GH_PI / 2;
  }

  //Adjust y-axis rotation
  cam_ry -= mouseDx * GH_MOUSE_SENSITIVITY;
  if (cam_ry > GH_PI) {
    cam_ry -= GH_PI * 2;
  } else if (cam_ry < -GH_PI) {
    cam_ry += GH_PI * 2;
  }
}

void Player::Move(float moveF, float moveL) {
  //Make sure movement is not too fast
  const float mag = std::sqrt(moveF*moveF + moveL*moveL);
  if (mag > 1.0f) {
    moveF /= mag;
    moveL /= mag;
  }

  //Movement
  const Matrix4 camToWorld = LocalToWorld() * Matrix4::RotY(cam_ry);
  velocity += camToWorld.MulDirection(Vector3(-moveL, 0, -moveF)) * (GH_WALK_ACCEL * GH_DT);

  //Don't allow non-falling speeds above the player's max speed
  const float tempY = velocity.y;
  velocity.y = 0.0f;
  velocity.ClipMag(p_scale * GH_WALK_SPEED);
  velocity.y = tempY;
}

void Player::OnCollide(Vector3 push) {
  //Prevent player from rolling down hills if they're not too steep
  if (push.Normalized().y > 0.7f) {
    push.x = 0.0f;
    push.z = 0.0f;
    onGround = true;
  }

  //Friction should only apply when player is on ground
  const float cur_friction = friction;
  if (!onGround) {
    friction = 0.0f;
  }

  //Base call
  pos += push;
  if (push.MagSq() > 1e-8f * p_scale)
  {
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
  friction = cur_friction;
}

Matrix4 Player::WorldToCam() const {
  return Matrix4::RotX(-cam_rx) * Matrix4::RotY(-cam_ry) * Matrix4::Trans(-CamOffset()) * WorldToLocal();
}

Matrix4 Player::CamToWorld() const {
  return LocalToWorld() * Matrix4::Trans(CamOffset()) * Matrix4::RotY(cam_ry) * Matrix4::RotX(cam_rx);
}

Vector3 Player::CamOffset() const {
  //If bob is too small, don't even bother
  if (bob_mag < GH_BOB_MIN) {
    return Vector3::Zero();
  }

  //Convert bob to translation
  const float theta = (GH_PI/2) * std::sin(bob_phi);
  const float y = bob_mag * GH_BOB_OFFS * (1.0f - std::cos(theta));
  return Vector3(0, y, 0);
}

Matrix4 Player::LocalToWorld() const {
  return Matrix4::Trans(pos) * Matrix4::RotY(euler.y) * Matrix4::RotX(euler.x) * Matrix4::RotZ(euler.z) * Matrix4::Scale(scale * p_scale);
}

Matrix4 Player::WorldToLocal() const {
  return Matrix4::Scale(1.0f / (scale * p_scale)) * Matrix4::RotZ(-euler.z) * Matrix4::RotX(-euler.x) * Matrix4::RotY(-euler.y) * Matrix4::Trans(-pos);
}
