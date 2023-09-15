Engine* GH_ENGINE = nullptr;
int GH_REC_LEVEL = 0;

Engine::Engine(int64_t Frequency)
: vObjects(std::vector<Object_Ptr>()),
  vPortals(std::vector<Portal_Ptr>()),
  player((Player_Ptr)new Player),
  TicksPerStep((int64_t)(Frequency * GH_DT)),
  input({0}),
  GH_FRAME(0)
{
  Assert(glewInit() == GLEW_OK);
  glClearColor(0.6f, 0.9f, 1.0f, 1.0f);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glCullFace(GL_BACK);
  glDepthFunc(GL_LESS);
  glDepthMask(GL_TRUE);
  glGetQueryiv(GL_SAMPLES_PASSED_ARB, GL_QUERY_COUNTER_BITS_ARB, &this->occlusionCullingSupported);
  wglSwapIntervalEXT(1);
  this->sky.load();
  GH_ENGINE = this;
}

void Engine::cleanup(void)
{
  //this->curScene->Unload();
  this->vObjects.clear();
  this->vPortals.clear();
}

void Engine::Update(void)
{
  player->Update();
  Matrix4 worldToLocal = player->WorldToLocal();

  //For each object to collide with
  for(auto& object : vObjects)
  {
    Object& obj = *object;
    if (!obj.mesh) { continue; }

    //For each hit sphere
    for (size_t s = 0; s < player->hitSpheres.size(); ++s) {
      //Brings point from collider's local coordinates to hits's local coordinates.
      const Sphere& sphere = player->hitSpheres[s];
      Matrix4 worldToUnit = sphere.LocalToUnit() * worldToLocal;
      Matrix4 localToUnit = worldToUnit * obj.LocalToWorld();
      Matrix4 unitToWorld = worldToUnit.Inverse();

      //For each collider
      for (size_t c = 0; c < obj.mesh->colliders.size(); ++c) {
        Vector3 push;
        const Collider& collider = obj.mesh->colliders[c];
        if (collider.Collide(localToUnit, push)) {
          //If push is too small, just ignore
          push = unitToWorld.MulDirection(push);
          player->OnCollide(obj, push);

          //worldToLocal = physical->WorldToLocal();
          worldToUnit = sphere.LocalToUnit() * worldToLocal;
          localToUnit = worldToUnit * obj.LocalToWorld();
          unitToWorld = worldToUnit.Inverse();
        }
      }
    }
  }

  //Portals
  for(auto& portal : vPortals)
  {
    if(player->TryPortal(*portal))
    {
      break;
    }
  }
}

void Engine::Render(const Camera& cam, GLuint curFBO, const Portal* skipPortal)
{
  if (GH_USE_SKY)
  {
    glClear(GL_DEPTH_BUFFER_BIT);
    sky.Draw(cam);
  }
  else
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  //Create queries (if applicable)
  GLuint queries[GH_MAX_PORTALS];
  GLuint drawTest[GH_MAX_PORTALS];
  Assert(vPortals.size() <= GH_MAX_PORTALS);
  if (occlusionCullingSupported) {
    glGenQueriesARB((GLsizei)vPortals.size(), queries);
  }

  //Draw scene
  for(auto& object : vObjects)
  {
    object->Draw(cam, curFBO);
  }

  //Draw portals if possible
  if (GH_REC_LEVEL > 0) {
    //Draw portals
    GH_REC_LEVEL -= 1;
    if (occlusionCullingSupported && GH_REC_LEVEL > 0) {
      glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
      glDepthMask(GL_FALSE);
      for (size_t i = 0; i < vPortals.size(); ++i) {
        if (vPortals[i].get() != skipPortal) {
          glBeginQueryARB(GL_SAMPLES_PASSED_ARB, queries[i]);
          vPortals[i]->DrawPink(cam);
          glEndQueryARB(GL_SAMPLES_PASSED_ARB);
        }
      }
      for (size_t i = 0; i < vPortals.size(); ++i) {
        if (vPortals[i].get() != skipPortal) {
          glGetQueryObjectuivARB(queries[i], GL_QUERY_RESULT_ARB, &drawTest[i]);
        }
      };
      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
      glDepthMask(GL_TRUE);
      glDeleteQueriesARB((GLsizei)vPortals.size(), queries);
    }
    for (size_t i = 0; i < vPortals.size(); ++i) {
      if (vPortals[i].get() != skipPortal) {
        if (occlusionCullingSupported && (GH_REC_LEVEL > 0) && (drawTest[i] == 0)) {
          continue;
        } else {
          vPortals[i]->Draw(cam, curFBO);
        }
      }
    }
    GH_REC_LEVEL += 1;
  }

#if 0
  //Debug draw colliders
  for (size_t i = 0; i < vObjects.size(); ++i) {
    vObjects[i]->DebugDraw(cam);
  }
#endif
}

void Engine::do_frame(int64_t& cur_ticks, int64_t new_ticks)
{
  //Used fixed time steps for updates
  for (int i = 0; cur_ticks < new_ticks && i < GH_MAX_STEPS; ++i) {
    this->Update();
    cur_ticks += this->TicksPerStep;
    this->GH_FRAME += 1;
    this->input.EndFrame();
  }
  cur_ticks = (cur_ticks < new_ticks ? new_ticks: cur_ticks);

  //Setup camera for rendering
  const float n = GH_CLAMP(this->NearestPortalDist() * 0.5f, GH_NEAR_MIN, GH_NEAR_MAX);
  this->main_cam.worldView = this->player->WorldToCam();
  this->main_cam.SetSize(GH_SCREEN_WIDTH, GH_SCREEN_HEIGHT, n, GH_FAR);
  this->main_cam.UseViewport();

  //Render scene
  GH_REC_LEVEL = GH_MAX_RECURSION;
  this->Render(this->main_cam, 0, nullptr);
}

float Engine::NearestPortalDist() const {
  float dist = FLT_MAX;
  for (size_t i = 0; i < vPortals.size(); ++i) {
    dist = GH_MIN(dist, vPortals[i]->DistTo(this->player->pos));
  }
  return dist;
}


void Engine::load_scene(size_t Index)
{
  vObjects.clear();
  vPortals.clear();
  player->Reset();
  //this->vObjects.push_back(player);

  switch (Index)
  {
    case 0:
    {
        Ground_Ptr ground = (Ground_Ptr)(new Ground());
        Tunnel_Ptr tunnel1 = (Tunnel_Ptr)(new Tunnel(Tunnel::NORMAL));
        Tunnel_Ptr tunnel2 = (Tunnel_Ptr)(new Tunnel(Tunnel::NORMAL));
        Portal_Ptr portal1 = (Portal_Ptr)(new Portal());
        Portal_Ptr portal2 = (Portal_Ptr)(new Portal());
        Portal_Ptr portal3 = (Portal_Ptr)(new Portal());
        Portal_Ptr portal4 = (Portal_Ptr)(new Portal());
        tunnel1->pos = Vector3(-2.4f, 0, -1.8f);
        tunnel1->scale = Vector3(1, 1, 4.8f);

        tunnel2->pos = Vector3(2.4f, 0, 0);
        tunnel2->scale = Vector3(1, 1, 0.6f);

        ground->scale *= 1.2f;

        tunnel1->SetDoor1(*portal1);

        tunnel2->SetDoor1(*portal2);

        tunnel1->SetDoor2(*portal3);

        tunnel2->SetDoor2(*portal4);

        Portal::Connect(portal1, portal2);
        Portal::Connect(portal3, portal4);
        player->SetPosition(Vector3(0, GH_PLAYER_HEIGHT, 5));

        vObjects.push_back(tunnel1);
        vObjects.push_back(tunnel2);
        vObjects.push_back(ground);

        vPortals.push_back(portal1);
        vPortals.push_back(portal2);
        vPortals.push_back(portal3);
        vPortals.push_back(portal4);
    } break;
    case 1:
    {
        House_Ptr house1 = (House_Ptr)(new House("three_room.bmp"));
        house1->pos = Vector3(0, 0, -20);

        Portal_Ptr portal1 = (Portal_Ptr)(new Portal());
        Portal_Ptr portal2 = (Portal_Ptr)(new Portal());
        house1->SetDoor3(*portal1);

        house1->SetDoor4(*portal2);
        Portal::Connect(portal1, portal2);
        player->SetPosition(Vector3(3, GH_PLAYER_HEIGHT, 3));
        vObjects.push_back(house1);

        vPortals.push_back(portal1);
        vPortals.push_back(portal2);
    } break;
    case 2:
    {
        House_Ptr house1 = (House_Ptr)(new House("three_room.bmp"));
        House_Ptr house2 = (House_Ptr)(new House("three_room2.bmp"));
        house1->pos = Vector3(0, 0, -20);

        house2->pos = Vector3(200, 0, -20);

        Portal_Ptr portal1 = (Portal_Ptr)(new Portal());
        Portal_Ptr portal2 = (Portal_Ptr)(new Portal());
        Portal_Ptr portal3 = (Portal_Ptr)(new Portal());
        house1->SetDoor4(*portal1);

        house2->SetDoor3(*portal2);

        house2->SetDoor1(*portal3);

        Portal::Connect(portal1->front, portal2->back);
        Portal::Connect(portal2->front, portal3->back);
        Portal::Connect(portal3->front, portal1->back);
        player->SetPosition(Vector3(3, GH_PLAYER_HEIGHT, 3));

        vObjects.push_back(house1);
        vObjects.push_back(house2);

        vPortals.push_back(portal1);
        vPortals.push_back(portal2);
        vPortals.push_back(portal3);
    } break;
    case 3:
    {
        //Room 1
        Pillar_Ptr pillar1 = (Pillar_Ptr)(new Pillar);
        PillarRoom_Ptr pillarRoom1 = (PillarRoom_Ptr)(new PillarRoom);
        Ground_Ptr ground1 = (Ground_Ptr)(new Ground);
        Statue_Ptr statue1 = (Statue_Ptr)(new Statue("teapot.obj"));
        Pillar_Ptr pillar2 = (Pillar_Ptr)(new Pillar);
        PillarRoom_Ptr pillarRoom2 = (PillarRoom_Ptr)(new PillarRoom);
        Ground_Ptr ground2 = (Ground_Ptr)(new Ground);
        Statue_Ptr statue2 = (Statue_Ptr)(new Statue("bunny.obj"));
        Pillar_Ptr pillar3 = (Pillar_Ptr)(new Pillar);
        PillarRoom_Ptr pillarRoom3 = (PillarRoom_Ptr)(new PillarRoom);
        Ground_Ptr ground3 = (Ground_Ptr)(new Ground);
        Statue_Ptr statue3 = (Statue_Ptr)(new Statue("suzanne.obj"));
        Portal_Ptr portal1 = (Portal_Ptr)(new Portal);
        Portal_Ptr portal2 = (Portal_Ptr)(new Portal);
        Portal_Ptr portal3 = (Portal_Ptr)(new Portal);


        ground1->scale *= 2.0f;

        statue1->pos = Vector3(0, 0.5f, 9);
        statue1->scale = Vector3(0.5f);
        statue1->euler.y = GH_PI / 2;

        //Room 2
        pillar2->pos = Vector3(200, 0, 0);

        pillarRoom2->pos = Vector3(200, 0, 0);

        ground2->pos = Vector3(200, 0, 0);
        ground2->scale *= 2.0f;

        statue2->pos = Vector3(200, -0.4f, 9);
        statue2->scale = Vector3(14.0f);
        statue2->euler.y = GH_PI;

        //Room 3
        pillar3->pos = Vector3(400, 0, 0);

        pillarRoom3->pos = Vector3(400, 0, 0);

        ground3->pos = Vector3(400, 0, 0);
        ground3->scale *= 2.0f;

        statue3->pos = Vector3(400, 0.9f, 9);
        statue3->scale = Vector3(1.2f);
        statue3->euler.y = GH_PI;

        //Portals
        pillarRoom1->SetPortal(*portal1);

        pillarRoom2->SetPortal(*portal2);

        pillarRoom3->SetPortal(*portal3);

        Portal::Connect(portal1->front, portal2->back);
        Portal::Connect(portal2->front, portal3->back);
        Portal::Connect(portal3->front, portal1->back);
        player->SetPosition(Vector3(0, GH_PLAYER_HEIGHT, 3));

        vObjects.push_back(pillar1);
        vObjects.push_back(pillarRoom1);
        vObjects.push_back(ground1);
        vObjects.push_back(statue1);
        vObjects.push_back(pillar2);
        vObjects.push_back(pillarRoom2);
        vObjects.push_back(ground2);
        vObjects.push_back(statue2);
        vObjects.push_back(pillar3);
        vObjects.push_back(pillarRoom3);
        vObjects.push_back(ground3);
        vObjects.push_back(statue3);

        vPortals.push_back(portal1);
        vPortals.push_back(portal2);
        vPortals.push_back(portal3);
    } break;
    case 4:
    {
        Ground_Ptr ground1 = (Ground_Ptr)(new Ground(true));
        Ground_Ptr ground2 = (Ground_Ptr)(new Ground(true));
        Tunnel_Ptr tunnel1 = (Tunnel_Ptr)(new Tunnel(Tunnel::SLOPE));
        Tunnel_Ptr tunnel2 = (Tunnel_Ptr)(new Tunnel(Tunnel::SLOPE));
        Portal_Ptr portal1 = (Portal_Ptr)(new Portal());
        Portal_Ptr portal2 = (Portal_Ptr)(new Portal());
        Portal_Ptr portal3 = (Portal_Ptr)(new Portal());
        Portal_Ptr portal4 = (Portal_Ptr)(new Portal());

        tunnel1->pos = Vector3(0, 0, 0);
        tunnel1->scale = Vector3(1, 1, 5);
        tunnel1->euler.y = GH_PI;

        ground1->scale *= Vector3(1, 2, 1);

        tunnel2->pos = Vector3(200, 0, 0);
        tunnel2->scale = Vector3(1, 1, 5);

        ground2->pos = Vector3(200, 0, 0);
        ground2->scale *= Vector3(1, 2, 1);
        ground2->euler.y = GH_PI;

        tunnel1->SetDoor1(*portal1);

        tunnel1->SetDoor2(*portal2);

        tunnel2->SetDoor1(*portal3);
        portal3->euler.y -= GH_PI;

        tunnel2->SetDoor2(*portal4);
        portal4->euler.y -= GH_PI;

        Portal::Connect(portal1, portal4);
        Portal::Connect(portal2, portal3);

        player->SetPosition(Vector3(0, GH_PLAYER_HEIGHT - 2, 8));

        vObjects.push_back(tunnel1);
        vObjects.push_back(ground1);
        vObjects.push_back(tunnel2);
        vObjects.push_back(ground2);

        vPortals.push_back(portal1);
        vPortals.push_back(portal2);
        vPortals.push_back(portal3);
        vPortals.push_back(portal4);
    } break;
    case 5:
    {
        Ground_Ptr ground1 = (Ground_Ptr)(new Ground());
        Ground_Ptr ground2 = (Ground_Ptr)(new Ground());
        Tunnel_Ptr tunnel1 = (Tunnel_Ptr)(new Tunnel(Tunnel::SCALE));
        Tunnel_Ptr tunnel2 = (Tunnel_Ptr)(new Tunnel(Tunnel::NORMAL));
        Tunnel_Ptr tunnel3 = (Tunnel_Ptr)(new Tunnel(Tunnel::NORMAL));
        Portal_Ptr portal1 = (Portal_Ptr)(new Portal());
        Portal_Ptr portal2 = (Portal_Ptr)(new Portal());
        Portal_Ptr portal3 = (Portal_Ptr)(new Portal());
        Portal_Ptr portal4 = (Portal_Ptr)(new Portal());

        tunnel1->pos = Vector3(-1.2f, 0, 0);
        tunnel1->scale = Vector3(1, 1, 2.4f);

        ground1->scale *= 1.2f;

        tunnel2->pos = Vector3(201.2f, 0, 0);
        tunnel2->scale = Vector3(1, 1, 2.4f);

        ground2->pos = Vector3(200, 0, 0);
        ground2->scale *= 1.2f;

        tunnel1->SetDoor1(*portal1);

        tunnel2->SetDoor1(*portal2);

        tunnel1->SetDoor2(*portal3);

        tunnel2->SetDoor2(*portal4);

        Portal::Connect(portal1, portal2);
        Portal::Connect(portal3, portal4);

        tunnel3->pos = Vector3(-1, 0, -4.2f);
        tunnel3->scale = Vector3(0.25f, 0.25f, 0.6f);
        tunnel3->euler.y = GH_PI / 2;

        player->SetPosition(Vector3(0, GH_PLAYER_HEIGHT, 5));

        vObjects.push_back(tunnel1);
        vObjects.push_back(ground1);
        vObjects.push_back(tunnel2);
        vObjects.push_back(ground2);
        vObjects.push_back(tunnel3);

        vPortals.push_back(portal1);
        vPortals.push_back(portal2);
        vPortals.push_back(portal3);
        vPortals.push_back(portal4);
    } break;
    case 6:
    {
        Floorplan_Ptr floorplan(new Floorplan);
        floorplan->AddPortals(vPortals);

        player->SetPosition(Vector3(2, GH_PLAYER_HEIGHT, 2));
        vObjects.push_back(floorplan);
    } break;
  }
}
