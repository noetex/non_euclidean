class Physical : public Object {
public:
  Physical();

  void Reset() override;
  void Update();
  void OnCollide(Object& other, const Vector3& push);
  Matrix4 LocalToWorld() const;
  Matrix4 WorldToLocal() const;

  Vector3 gravity;
  Vector3 velocity;
  float bounce;
  float friction;
  float high_friction;
  float drag;
  float p_scale;  // Physical scale, only updated by portal scale changes
  
  Vector3 prev_pos;

  std::vector<Sphere> hitSpheres;
};
