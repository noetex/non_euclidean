struct object_props
{
  Vector3 pos;
  Vector3 euler;
  Vector3 scale;
};

static void
object_reset(object_props* Obj)
{
  Obj->pos.SetZero();
  Obj->euler.SetZero();
  Obj->scale.SetOnes();
}

static Matrix4
object_local_to_world(object_props* Obj)
{
  Matrix4 scale = Matrix4::Scale(Obj->scale);
  Matrix4 rotation = Matrix4::RotY(Obj->euler.y) * Matrix4::RotX(Obj->euler.x) * Matrix4::RotZ(Obj->euler.z);
  Matrix4 translation = Matrix4::Trans(Obj->pos);
  return translation * rotation * scale;
}

static Matrix4
object_world_to_local(object_props* Obj)
{
  Matrix4 translation = Matrix4::Trans(-Obj->pos);
  Matrix4 rotation = Matrix4::RotZ(-Obj->euler.z) * Matrix4::RotX(-Obj->euler.x) * Matrix4::RotY(-Obj->euler.y);
  Matrix4 scale = Matrix4::Scale(1.0f / (Obj->scale));
  return scale * rotation * translation;
}

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
      Matrix4 wtl = object_world_to_local(&Geom.Obj);
      Matrix4 ltw = object_local_to_world(&Geom.Obj);
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
      Matrix4 ltw = object_local_to_world(&Geom.Obj);
      Geom.mesh->DebugDraw(cam, ltw);
    }
  }
  geometric_props Geom;
  Texture_Ptr texture;
};
