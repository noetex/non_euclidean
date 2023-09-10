#ifndef DPI_ENUMS_DECLARED
  typedef enum
  {
    PROCESS_DPI_UNAWARE = 0,
    PROCESS_SYSTEM_DPI_AWARE = 1,
    PROCESS_PER_MONITOR_DPI_AWARE = 2
  } PROCESS_DPI_AWARENESS;
  typedef enum
  {
    MDT_EFFECTIVE_DPI = 0,
    MDT_ANGULAR_DPI = 1,
    MDT_RAW_DPI = 2,
    MDT_DEFAULT = MDT_EFFECTIVE_DPI
  } MONITOR_DPI_TYPE;
#endif

#ifndef _DPI_AWARENESS_CONTEXTS_
  DECLARE_HANDLE(DPI_AWARENESS_CONTEXT);
  #define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE (DPI_AWARENESS_CONTEXT)-3
#endif

#ifndef DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE
  #define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE ((DPI_AWARENESS_CONTEXT)-3)
#endif

#ifndef DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
  #define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((DPI_AWARENESS_CONTEXT)-4)
#endif

// https://learn.microsoft.com/en-us/windows-hardware/drivers/hid/hid-usages
enum raw_input_stuff
{
  HID_USAGE_PAGE_GENERIC = 0x01,
  HID_USAGE_PAGE_GAME = 0x05,
  HID_USAGE_PAGE_LED = 0x08,
  HID_USAGE_PAGE_BUTTON = 0x09,
  HID_USAGE_GENERIC_POINTER = 0x01,
  HID_USAGE_GENERIC_MOUSE = 0x02,
  HID_USAGE_GENERIC_JOYSTICK = 0x04,
  HID_USAGE_GENERIC_GAMEPAD = 0x05,
  HID_USAGE_GENERIC_KEYBOARD = 0x06,
  HID_USAGE_GENERIC_KEYPAD = 0x07,
  HID_USAGE_GENERIC_MULTI_AXIS_CONTROLLER = 0x08,
};

//Windows
static const char GH_TITLE[] = "NonEuclideanDemo";
static const char GH_CLASS[] = "NED";

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
class Engine;
class Input;
class Player;
extern Engine* GH_ENGINE;
extern Player* GH_PLAYER;
extern const Input* GH_INPUT;
extern int GH_REC_LEVEL;
extern int64_t GH_FRAME;

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
