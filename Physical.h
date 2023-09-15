class Physical : public Object {
public:
  Physical();

  virtual void Reset() override;
  virtual void Update();
  virtual void OnCollide(Object& other, const Vector3& push);
  bool is_physical(void) final override { return true; }
  void SetPosition(const Vector3& _pos) {
    pos = _pos;
    prev_pos = _pos;
  }

  Vector3 gravity;
  Vector3 velocity;
  float bounce;
  float friction;
  float high_friction;
  float drag;
  //float p_scale;  // Physical scale, only updated by portal scale changes
  
  Vector3 prev_pos;

  std::vector<Sphere> hitSpheres;
};
