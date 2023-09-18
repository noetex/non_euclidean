#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include<windows.h>

#include"euclid_windows.h"

#include<cmath>
#include<vector>
#include<string>
#include<cstdint>
#include<fstream>
#include<sstream>
#include<iostream>
#include<unordered_map>

#pragma warning(disable : 4099) // Missing PDB file

#define GLEW_STATIC
#include"glew-2.2.0/include/GL/glew.h"
#include"glew-2.2.0/include/GL/wglew.h"

#include"gameheader.h"
#include"Input.h"
#include"vector.h"
#include"Shader.h"
#include"Texture.h"
#include"Camera.h"
#include"FrameBuffer.h"
#include"Collider.h"
#include"Mesh.h"
#include"Resources.h"
#include"Sky.h"
#include"Sphere.h"

#include"Object.h"
#include"Statue.h"
#include"House.h"
#include"Ground.h"
#include"Pillar.h"
#include"PillarRoom.h"
#include"Portal.h"
#include"Floorplan.h"
#include"Tunnel.h"
#include"Player.h"
#include"Engine.h"

#include"framebuffer.cpp"
#include"texture.cpp"
#include"mesh.cpp"
#include"shader.cpp"
#include"resources.cpp"
#include"camera.cpp"

#include"collider.cpp"
#include"object.cpp"
#include"portal.cpp"
#include"player.cpp"

#include"engine.cpp"
#include"windows_main.cpp"
