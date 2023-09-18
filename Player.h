class Player : public Physical {
public:
  Player();

  virtual void Reset() override;
  virtual void Update();
  virtual void OnCollide(Vector3 push);
  Matrix4 LocalToWorld() const;
  Matrix4 WorldToLocal() const;

  void Look(float mouseDx, float mouseDy);
  void Move(float moveF, float moveL);
  void SetPosition(const Vector3& _pos) {
    pos = _pos;
    prev_pos = _pos;
  }

  Matrix4 WorldToCam() const;
  Matrix4 CamToWorld() const;
  Vector3 CamOffset() const;

private:
  float cam_rx;
  float cam_ry;

  float bob_mag;
  float bob_phi;
  //float p_scale;  // Physical scale, only updated by portal scale changes

  bool onGround;
};
