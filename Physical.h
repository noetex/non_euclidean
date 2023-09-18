class Physical : public Object {
public:
  void OnCollide(const Vector3& push);

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
