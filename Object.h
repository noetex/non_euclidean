class Object {
public:
  Object();
  virtual void Reset();
  virtual void Draw(const Camera& cam, GLuint curFBO);

  void DebugDraw(const Camera& cam);

  Matrix4 LocalToWorld() const;
  Matrix4 WorldToLocal() const;
  Vector3 Forward() const;

  Vector3 pos;
  Vector3 euler;
  Vector3 scale;

  Mesh_Ptr mesh;
  Texture_Ptr texture;
  Shader_Ptr shader;
};
