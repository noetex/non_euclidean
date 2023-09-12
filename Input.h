class Input {
public:
  void EndFrame(void)
  {
    memset(key_press, 0, sizeof(key_press));
    memset(mouse_button_press, 0, sizeof(mouse_button_press));
    mouse_dx = mouse_dx * GH_MOUSE_SMOOTH + mouse_ddx * (1.0f - GH_MOUSE_SMOOTH);
    mouse_dy = mouse_dy * GH_MOUSE_SMOOTH + mouse_ddy * (1.0f - GH_MOUSE_SMOOTH);
    mouse_ddx = 0.0f;
    mouse_ddy = 0.0f;
  }

  //Keyboard
  bool key[256];
  bool key_press[256];

  //Mouse
  bool mouse_button[3];
  bool mouse_button_press[3];
  float mouse_dx;
  float mouse_dy;
  float mouse_ddx;
  float mouse_ddy;

  //Joystick
  //TODO:

  //Bindings
  //TODO:

  //Calibration
  //TODO:
};
