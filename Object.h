//Forward declarations
class Physical;
class Mesh;
class Texture;
class Shader;

class Object {
public:
  Object();
  virtual ~Object() {}

  virtual void Reset();
  virtual void Draw(const Camera& cam, uint32_t curFBO);
  virtual void Update() {};
  virtual void OnHit(Object& other, Vector3& push) {};

  //Casts
#if 0
  virtual Physical* AsPhysical() { return nullptr; }
  const Physical* AsPhysical() const { return const_cast<Object*>(this)->AsPhysical(); }
#endif
  virtual bool is_physical(void) { return false; }

  void DebugDraw(const Camera& cam);

  Matrix4 LocalToWorld() const;
  Matrix4 WorldToLocal() const;
  Vector3 Forward() const;

  Vector3 pos;
  Vector3 euler;
  Vector3 scale;

  // Physical scale, only updated by portal scale changes
  float p_scale;

  Mesh_Ptr mesh;
  Texture_Ptr texture;
  Shader_Ptr shader;
};

TYPEDEF_PTR_TYPE(Object);
typedef std::vector<Object_Ptr> PObjectVec;
