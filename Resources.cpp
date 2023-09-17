Mesh_Ptr AquireMesh(const char* name)
{
  static std::unordered_map<std::string, std::weak_ptr<Mesh>> map;
  std::weak_ptr<Mesh>& mesh = map[std::string(name)];
  if (mesh.expired())
  {
    Mesh_Ptr newMesh = (Mesh_Ptr)(new Mesh(name));
    mesh = newMesh;
    return newMesh;
  }
  return mesh.lock();
}

Shader_Ptr AquireShader(const char* name)
{
  static std::unordered_map<std::string, std::weak_ptr<Shader>> map;
  std::weak_ptr<Shader>& shader = map[std::string(name)];
  if (shader.expired())
  {
    Shader_Ptr newShader = (Shader_Ptr)(new Shader(name));
    shader = newShader;
    return newShader;
  }
  return shader.lock();
}

Texture_Ptr AquireTexture(const char* name, int rows, int cols)
{
  static std::unordered_map<std::string, std::weak_ptr<Texture>> map;
  std::weak_ptr<Texture>& tex = map[std::string(name)];
  if (tex.expired())
  {
    Texture_Ptr newTex = (Texture_Ptr)(new Texture(name, rows, cols));
    tex = newTex;
    return newTex;
  }
  return tex.lock();
}

#if 0
template<typename resource_type>
static resource_type*
aquire_shared_resource(const char* name)
{
  static std::unordered_map<std::string, std::weak_ptr<resource_type>> map;
  std::weak_ptr<resource_type>& Resource = map[std::string(name)];
  if(Resource.expired())
  {
    resource_type* NewResource = new
  }

}
#endif
