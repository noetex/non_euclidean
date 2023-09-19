class Player {
public:
  Player();

  void Reset();
  void OnCollide(Vector3 push);
  Matrix4 LocalToWorld() const;
  Matrix4 WorldToLocal() const;
  void update_bob_and_stuff(void);
  void process_input(Input&);
  void Look(float mouseDx, float mouseDy);
  void Move(float moveF, float moveL);
  void jump(void);
  void SetPosition(const Vector3& _pos) {
    Obj.pos = _pos;
    prev_pos = _pos;
  }

  Matrix4 WorldToCam() const;
  Matrix4 CamToWorld() const;
  Vector3 CamOffset() const;

  object_props Obj;
  Vector3 gravity;
  Vector3 velocity;
  float bounce;
  float friction;
  float high_friction;
  float drag;
  float p_scale;

  Vector3 prev_pos;

  std::vector<Sphere> hitSpheres;

private:
  float cam_rx;
  float cam_ry;

  float bob_mag;
  float bob_phi;

  bool onGround;
};
