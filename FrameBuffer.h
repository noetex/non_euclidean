class FrameBuffer {
public:
  FrameBuffer();

  void Render(const Camera& cam, GLuint curFBO, const class Portal* skipPortal);
  void Use();

private:
  GLuint texId;
  GLuint fbo;
  GLuint renderBuf;
};
