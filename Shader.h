#if 0
struct my_shader
{
  GLuint Program;
  GLuint MVP;
  GLuint MV;
};
#endif


class Shader {
public:
  Shader(const char* name);
  ~Shader();

  void Use();
  void SetMVP(const float* mvp, const float* mv);

private:
  static GLuint Load(const char* fname, GLenum type, std::vector<std::string>& attribs);

  GLuint progId;
  GLuint mvpId;
  GLuint mvId;
};

TYPEDEF_PTR_TYPE(Shader);
