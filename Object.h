struct object_props
{
  Vector3 pos;
  Vector3 euler;
  Vector3 scale;
  void reset(void)
  {
    this->pos.SetZero();
    this->euler.SetZero();
    this->scale.SetOnes();
  }
  Matrix4 local_to_world(void)
  {
    Matrix4 scale = Matrix4::Scale(this->scale);
    Matrix4 rotation = Matrix4::RotY(this->euler.y) * Matrix4::RotX(this->euler.x) * Matrix4::RotZ(this->euler.z);
    Matrix4 translation = Matrix4::Trans(this->pos);
    return translation * rotation * scale;
  }
  Matrix4 world_to_local(void)
  {
    Matrix4 translation = Matrix4::Trans(-this->pos);
    Matrix4 rotation = Matrix4::RotZ(-this->euler.z) * Matrix4::RotX(-this->euler.x) * Matrix4::RotY(-this->euler.y);
    Matrix4 scale = Matrix4::Scale(1.0f / (this->scale));
    return scale * rotation * translation;
  }
};

struct geometric_props
{
  object_props Obj;
  Mesh_Ptr mesh;
  Shader_Ptr shader;
};

class Rigid
{
public:
  void Draw(const Camera& cam, uint32_t curFBO) {
    if (Geom.shader && Geom.mesh) {
      Matrix4 wtl = Geom.Obj.world_to_local();
      Matrix4 ltw = Geom.Obj.local_to_world();
      const Matrix4 mv = wtl.Transposed();
      const Matrix4 mvp = cam.Matrix() * ltw;
      Geom.shader->Use();
      if (texture) {
        texture->Use();
      }
      Geom.shader->SetMVP(mvp.m, mv.m);
      Geom.mesh->Draw();
    }
  }

  void DebugDraw(const Camera& cam) {
    if (Geom.mesh) {
      Matrix4 ltw = Geom.Obj.local_to_world();
      Geom.mesh->DebugDraw(cam, ltw);
    }
  }
  geometric_props Geom;
  Texture_Ptr texture;
};
