class Object {
public:
  Object();
  virtual void Reset();
  virtual void Draw(const Camera& cam, uint32_t curFBO);

  void DebugDraw(const Camera& cam);

  Matrix4 LocalToWorld() const;
  Matrix4 WorldToLocal() const;
  Vector3 Forward() const;

  Vector3 pos;
  Vector3 euler;
  Vector3 scale;
  float p_scale;  // Physical scale, only updated by portal scale changes

  Mesh_Ptr mesh;
  Texture_Ptr texture;
  Shader_Ptr shader;
};
