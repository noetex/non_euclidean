//#define USE_REAL_POINTERS
#ifdef USE_REAL_POINTERS
  #define TYPEDEF_PTR_TYPE(Type) typedef Type* Type ## _Ptr
#else
  #define TYPEDEF_PTR_TYPE(Type) typedef std::shared_ptr<Type> Type ## _Ptr
#endif

#define Assert(Expression) if(Expression); else __debugbreak()
//#define Assert(Expression) (void)(Expression)

typedef int bool_t;

//General
static const float GH_PI = 3.141592653589793f;
static const int GH_MAX_PORTALS = 16;

//Graphics
static const bool GH_START_FULLSCREEN = false;
static const bool GH_HIDE_MOUSE = true;
static const bool GH_USE_SKY = true;
static const int GH_SCREEN_WIDTH = 1280;
static const int GH_SCREEN_HEIGHT = 720;
static const int GH_SCREEN_X = 50;
static const int GH_SCREEN_Y = 50;
static const float GH_FOV = 60.0f;
static const float GH_NEAR_MIN = 1e-3f;
static const float GH_NEAR_MAX = 1e-1f;
static const float GH_FAR = 100.0f;
static const int GH_FBO_SIZE = 2048;
static const int GH_MAX_RECURSION = 4;

//Gameplay
static const float GH_MOUSE_SENSITIVITY = 0.005f;
static const float GH_MOUSE_SMOOTH = 0.5f;
static const float GH_WALK_SPEED = 2.9f;
static const float GH_WALK_ACCEL = 50.0f;
static const float GH_BOB_FREQ = 8.0f;
static const float GH_BOB_OFFS = 0.015f;
static const float GH_BOB_DAMP = 0.04f;
static const float GH_BOB_MIN = 0.1f;
static const float GH_DT = 0.002f;
static const int GH_MAX_STEPS = 30;
static const float GH_PLAYER_HEIGHT = 1.5f;
static const float GH_PLAYER_RADIUS = 0.2f;
static const float GH_GRAVITY = -9.8f;

//Global variables
//extern class Engine* GH_ENGINE;

//Functions
template<class T>
inline T GH_CLAMP(T a, T mn, T mx) {
  return a < mn ? mn : (a > mx ? mx : a);
}
template<class T>
inline T GH_MIN(T a, T b) {
  return a < b ? a : b;
}
template<class T>
inline T GH_MAX(T a, T b) {
  return a > b ? a : b;
}
