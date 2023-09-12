class Sky {
public:
  Sky() {
    mesh = AquireMesh("quad.obj");
    shader = AquireShader("sky");
  }

  void Draw(const Camera& cam) {
    glDepthMask(GL_FALSE);
    const Matrix4 mvp = cam.projection.Inverse();
    const Matrix4 mv = cam.worldView.Inverse();
    shader->Use();
    shader->SetMVP(mvp.m, mv.m);
    mesh->Draw();
    glDepthMask(GL_TRUE);
  }

private:
  Mesh_Ptr mesh;
  Shader_Ptr shader;
};
