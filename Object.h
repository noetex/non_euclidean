class Object {
public:
  virtual void Reset();

  Matrix4 LocalToWorld() const;
  Matrix4 WorldToLocal() const;

  Vector3 pos;
  Vector3 euler;
  Vector3 scale;
};

class Geometric : public Object
{
public:
  Mesh_Ptr mesh;
  Shader_Ptr shader;
};

class Rigid : public Geometric
{
public:
  void Draw(const Camera& cam, uint32_t curFBO) {
    if (shader && mesh) {
      const Matrix4 mv = WorldToLocal().Transposed();
      const Matrix4 mvp = cam.Matrix() * LocalToWorld();
      shader->Use();
      if (texture) {
        texture->Use();
      }
      shader->SetMVP(mvp.m, mv.m);
      mesh->Draw();
    }
  }

  void DebugDraw(const Camera& cam) {
    if (mesh) {
      mesh->DebugDraw(cam, LocalToWorld());
    }
  }

  Texture_Ptr texture;
};
