Engine* GH_ENGINE = nullptr;

Engine::Engine(int64_t Frequency)
: TicksPerStep((int64_t)(Frequency * GH_DT)),
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
  player.init();
  this->sky.load();
  GH_ENGINE = this;
}

void Engine::cleanup(void)
{
  this->vObjects.clear();
  this->vPortals.clear();
}

void Engine::Update(void)
{
  player.update_bob_and_stuff();
  player.Look(input.mouse_dx, input.mouse_dy);
  player.process_input(this->input);
  Matrix4 worldToLocal = player.WorldToLocal();

  for(auto& sphere : player.hitSpheres)
  {
    Matrix4 worldToUnit = sphere.LocalToUnit() * worldToLocal;
    Matrix4 unitToWorld = worldToUnit.Inverse();
    for(auto& object : vObjects)
    {
      Rigid& obj = *object;
      if (!obj.Geom.mesh)
      {
        continue;
      }
      Matrix4 localToUnit = worldToUnit * object_local_to_world(&obj.Geom.Obj);
      for(auto& collider : obj.Geom.mesh->colliders)
      {
        Vector3 push;
        if (collider.Collide(localToUnit, push))
        {
          //If push is too small, just ignore
          push = unitToWorld.MulDirection(push);
          player.OnCollide(push);
        }
      }
    }
  }

  for(auto& portal : vPortals)
  {
    const Vector3 bump = portal->GetBump(player.prev_pos) * (2 * GH_NEAR_MIN * player.p_scale);
    const Portal::Warp* warp = portal->Intersects(player.prev_pos, player.Obj.pos, bump);
    if(!warp)
    {
      continue;
    }
    //Teleport object
    player.Obj.pos = warp->deltaInv.MulPoint(player.Obj.pos - bump * 2);
    player.velocity = warp->deltaInv.MulDirection(player.velocity);
    player.prev_pos = player.Obj.pos;

    //Update camera direction
    const Vector3 forward(-std::sin(player.Obj.euler.y), 0, -std::cos(player.Obj.euler.y));
    const Vector3 newDir = warp->deltaInv.MulDirection(forward);
    player.Obj.euler.y = -std::atan2(newDir.x, -newDir.z);

    //Update object scale
    player.p_scale *= warp->deltaInv.XAxis().Mag();
    break;
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

  //Draw scene
  for(auto& object : vObjects)
  {
    object->Draw(cam, curFBO);
  }

  GLuint drawTest[GH_MAX_PORTALS];
  Assert(vPortals.size() <= GH_MAX_PORTALS);

  //Portal recursive rendering
  if (GH_REC_LEVEL > 0)
  {
    GLuint Query;
    if (occlusionCullingSupported)
    {
      glGenQueriesARB(1, &Query);
    }
    GH_REC_LEVEL -= 1;
    if (occlusionCullingSupported && GH_REC_LEVEL > 0)
    {
      glDepthMask(GL_FALSE);
      glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
      for (size_t i = 0; i < vPortals.size(); ++i)
      {
        if (vPortals[i] == skipPortal)
        {
          continue;
        }
        glBeginQueryARB(GL_SAMPLES_PASSED_ARB, Query);
        vPortals[i]->DrawPink(cam);
        glEndQueryARB(GL_SAMPLES_PASSED_ARB);
        glGetQueryObjectuivARB(Query, GL_QUERY_RESULT_ARB, &drawTest[i]);
      }
      glDeleteQueriesARB(1, &Query);
      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
      glDepthMask(GL_TRUE);
    }
    for (size_t i = 0; i < vPortals.size(); ++i) {
      if (vPortals[i] == skipPortal)
      {
        continue;
      }
      if (occlusionCullingSupported && (GH_REC_LEVEL > 0) && (drawTest[i] == 0)) {
        continue;
      }
      vPortals[i]->Draw(cam, curFBO);
    }
    GH_REC_LEVEL += 1;
  }

#if 0
  for(auto& object : vObjects)
  {
    object->DebugDraw(cam);
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
  this->main_cam.worldView = this->player.WorldToCam();
  this->main_cam.SetSize(GH_SCREEN_WIDTH, GH_SCREEN_HEIGHT, n, GH_FAR);
  this->main_cam.UseViewport();

  //Render scene
  GH_REC_LEVEL = GH_MAX_RECURSION;
  this->Render(this->main_cam, 0, nullptr);
}

float Engine::NearestPortalDist() const {
  float dist = FLT_MAX;
  for(auto& portal : vPortals)
  {
    dist = GH_MIN(dist, portal->DistTo(this->player.Obj.pos));
  }
  return dist;
}


void Engine::load_scene(size_t Index)
{
  vObjects.clear();
  vPortals.clear();
  player.Reset();

  switch (Index)
  {
    case 0:
    {
        Ground* ground = new Ground();
        Tunnel* tunnel1 = new Tunnel(Tunnel::NORMAL);
        Tunnel* tunnel2 = new Tunnel(Tunnel::NORMAL);
        Portal* portal1 = new Portal();
        Portal* portal2 = new Portal();
        Portal* portal3 = new Portal();
        Portal* portal4 = new Portal();
        tunnel1->Geom.Obj.pos = Vector3(-2.4f, 0, -1.8f);
        tunnel1->Geom.Obj.scale = Vector3(1, 1, 4.8f);

        tunnel2->Geom.Obj.pos = Vector3(2.4f, 0, 0);
        tunnel2->Geom.Obj.scale = Vector3(1, 1, 0.6f);

        ground->Geom.Obj.scale *= 1.2f;

        tunnel1->SetDoor1(*portal1);

        tunnel2->SetDoor1(*portal2);

        tunnel1->SetDoor2(*portal3);

        tunnel2->SetDoor2(*portal4);

        Portal::Connect(portal1, portal2);
        Portal::Connect(portal3, portal4);
        player.SetPosition(Vector3(0, GH_PLAYER_HEIGHT, 5));

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
        House* house1 = new House("three_room.bmp");
        house1->Geom.Obj.pos = Vector3(0, 0, -20);

        Portal* portal1 = new Portal();
        Portal* portal2 = new Portal();
        house1->SetDoor3(*portal1);

        house1->SetDoor4(*portal2);
        Portal::Connect(portal1, portal2);
        player.SetPosition(Vector3(3, GH_PLAYER_HEIGHT, 3));
        vObjects.push_back(house1);

        vPortals.push_back(portal1);
        vPortals.push_back(portal2);
    } break;
    case 2:
    {
        House* house1 = new House("three_room.bmp");
        House* house2 = new House("three_room2.bmp");
        house1->Geom.Obj.pos = Vector3(0, 0, -20);

        house2->Geom.Obj.pos = Vector3(200, 0, -20);

        Portal* portal1 = new Portal();
        Portal* portal2 = new Portal();
        Portal* portal3 = new Portal();
        house1->SetDoor4(*portal1);

        house2->SetDoor3(*portal2);

        house2->SetDoor1(*portal3);

        Portal::Connect(portal1->front, portal2->back);
        Portal::Connect(portal2->front, portal3->back);
        Portal::Connect(portal3->front, portal1->back);
        player.SetPosition(Vector3(3, GH_PLAYER_HEIGHT, 3));

        vObjects.push_back(house1);
        vObjects.push_back(house2);

        vPortals.push_back(portal1);
        vPortals.push_back(portal2);
        vPortals.push_back(portal3);
    } break;
    case 3:
    {
        //Room 1
        Pillar* pillar1 = new Pillar;
        Pillar* pillar2 = new Pillar;
        Pillar* pillar3 = new Pillar;
        Ground* ground1 = new Ground;
        Ground* ground2 = new Ground;
        Statue* statue1 = new Statue("teapot.obj");
        Statue* statue2 = new Statue("bunny.obj");
        PillarRoom* pillarRoom1 = new PillarRoom;
        PillarRoom* pillarRoom2 = new PillarRoom;
        PillarRoom* pillarRoom3 = new PillarRoom;
        Ground* ground3 = new Ground;
        Statue* statue3 = new Statue("suzanne.obj");
        Portal* portal1 = new Portal();
        Portal* portal2 = new Portal();
        Portal* portal3 = new Portal();


        ground1->Geom.Obj.scale *= 2.0f;

        statue1->Geom.Obj.pos = Vector3(0, 0.5f, 9);
        statue1->Geom.Obj.scale = Vector3(0.5f);
        statue1->Geom.Obj.euler.y = GH_PI / 2;

        //Room 2
        pillar2->Geom.Obj.pos = Vector3(200, 0, 0);

        pillarRoom2->Geom.Obj.pos = Vector3(200, 0, 0);

        ground2->Geom.Obj.pos = Vector3(200, 0, 0);
        ground2->Geom.Obj.scale *= 2.0f;

        statue2->Geom.Obj.pos = Vector3(200, -0.4f, 9);
        statue2->Geom.Obj.scale = Vector3(14.0f);
        statue2->Geom.Obj.euler.y = GH_PI;

        //Room 3
        pillar3->Geom.Obj.pos = Vector3(400, 0, 0);

        pillarRoom3->Geom.Obj.pos = Vector3(400, 0, 0);

        ground3->Geom.Obj.pos = Vector3(400, 0, 0);
        ground3->Geom.Obj.scale *= 2.0f;

        statue3->Geom.Obj.pos = Vector3(400, 0.9f, 9);
        statue3->Geom.Obj.scale = Vector3(1.2f);
        statue3->Geom.Obj.euler.y = GH_PI;

        //Portals
        pillarRoom1->SetPortal(*portal1);

        pillarRoom2->SetPortal(*portal2);

        pillarRoom3->SetPortal(*portal3);

        Portal::Connect(portal1->front, portal2->back);
        Portal::Connect(portal2->front, portal3->back);
        Portal::Connect(portal3->front, portal1->back);
        player.SetPosition(Vector3(0, GH_PLAYER_HEIGHT, 3));

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
        Ground* ground1 = new Ground(true);
        Ground* ground2 = new Ground(true);
        Tunnel* tunnel1 = new Tunnel(Tunnel::SLOPE);
        Tunnel* tunnel2 = new Tunnel(Tunnel::SLOPE);
        Portal* portal1 = new Portal();
        Portal* portal2 = new Portal();
        Portal* portal3 = new Portal();
        Portal* portal4 = new Portal();

        tunnel1->Geom.Obj.pos = Vector3(0, 0, 0);
        tunnel1->Geom.Obj.scale = Vector3(1, 1, 5);
        tunnel1->Geom.Obj.euler.y = GH_PI;

        ground1->Geom.Obj.scale *= Vector3(1, 2, 1);

        tunnel2->Geom.Obj.pos = Vector3(200, 0, 0);
        tunnel2->Geom.Obj.scale = Vector3(1, 1, 5);

        ground2->Geom.Obj.pos = Vector3(200, 0, 0);
        ground2->Geom.Obj.scale *= Vector3(1, 2, 1);
        ground2->Geom.Obj.euler.y = GH_PI;

        tunnel1->SetDoor1(*portal1);

        tunnel1->SetDoor2(*portal2);

        tunnel2->SetDoor1(*portal3);
        portal3->Geom.Obj.euler.y -= GH_PI;

        tunnel2->SetDoor2(*portal4);
        portal4->Geom.Obj.euler.y -= GH_PI;

        Portal::Connect(portal1, portal4);
        Portal::Connect(portal2, portal3);

        player.SetPosition(Vector3(0, GH_PLAYER_HEIGHT - 2, 8));

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
        Ground* ground1 = new Ground();
        Ground* ground2 = new Ground();
        Tunnel* tunnel1 = new Tunnel(Tunnel::SCALE);
        Tunnel* tunnel2 = new Tunnel(Tunnel::NORMAL);
        Tunnel* tunnel3 = new Tunnel(Tunnel::NORMAL);
        Portal* portal1 = new Portal();
        Portal* portal2 = new Portal();
        Portal* portal3 = new Portal();
        Portal* portal4 = new Portal();

        tunnel1->Geom.Obj.pos = Vector3(-1.2f, 0, 0);
        tunnel1->Geom.Obj.scale = Vector3(1, 1, 2.4f);

        ground1->Geom.Obj.scale *= 1.2f;

        tunnel2->Geom.Obj.pos = Vector3(201.2f, 0, 0);
        tunnel2->Geom.Obj.scale = Vector3(1, 1, 2.4f);

        ground2->Geom.Obj.pos = Vector3(200, 0, 0);
        ground2->Geom.Obj.scale *= 1.2f;

        tunnel1->SetDoor1(*portal1);

        tunnel2->SetDoor1(*portal2);

        tunnel1->SetDoor2(*portal3);

        tunnel2->SetDoor2(*portal4);

        Portal::Connect(portal1, portal2);
        Portal::Connect(portal3, portal4);

        tunnel3->Geom.Obj.pos = Vector3(-1, 0, -4.2f);
        tunnel3->Geom.Obj.scale = Vector3(0.25f, 0.25f, 0.6f);
        tunnel3->Geom.Obj.euler.y = GH_PI / 2;

        player.SetPosition(Vector3(0, GH_PLAYER_HEIGHT, 5));

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
      Floorplan* floorplan = new Floorplan;
      //Floorplan_Ptr floorplan(new Floorplan);
      Portal* p1 = new Portal();
      Portal* p2 = new Portal();
      Portal* p3 = new Portal();
      Portal* p4 = new Portal();
      Portal* p5 = new Portal();
      Portal* p6 = new Portal();

      p1->Geom.Obj.pos = Vector3(33, 10, 25.5f) * floorplan->Geom.Obj.scale;
      p1->Geom.Obj.scale = Vector3(4, 10, 1) * floorplan->Geom.Obj.scale;

      p2->Geom.Obj.pos = Vector3(74, 10, 25.5f) * floorplan->Geom.Obj.scale;
      p2->Geom.Obj.scale = Vector3(4, 10, 1) * floorplan->Geom.Obj.scale;

      p3->Geom.Obj.pos = Vector3(33, 10, 66.5f) * floorplan->Geom.Obj.scale;
      p3->Geom.Obj.scale = Vector3(4, 10, 1) * floorplan->Geom.Obj.scale;

      p4->Geom.Obj.pos = Vector3(63.5f, 10, 48) * floorplan->Geom.Obj.scale;
      p4->Geom.Obj.scale = Vector3(4, 10, 1) * floorplan->Geom.Obj.scale;
      p4->Geom.Obj.euler.y = GH_PI/2;

      p5->Geom.Obj.pos = Vector3(63.5f, 10, 7) * floorplan->Geom.Obj.scale;
      p5->Geom.Obj.scale = Vector3(4, 10, 1) * floorplan->Geom.Obj.scale;
      p5->Geom.Obj.euler.y = GH_PI / 2;

      p6->Geom.Obj.pos = Vector3(22.5f, 10, 48) * floorplan->Geom.Obj.scale;
      p6->Geom.Obj.scale = Vector3(4, 10, 1) * floorplan->Geom.Obj.scale;
      p6->Geom.Obj.euler.y = GH_PI / 2;

      Portal::Connect(p1->front, p3->back);
      Portal::Connect(p1->back, p2->front);
      Portal::Connect(p3->front, p2->back);

      Portal::Connect(p4->front, p6->back);
      Portal::Connect(p4->back, p5->front);
      Portal::Connect(p6->front, p5->back);

      vPortals.push_back(p1);
      vPortals.push_back(p2);
      vPortals.push_back(p3);
      vPortals.push_back(p4);
      vPortals.push_back(p5);
      vPortals.push_back(p6);

      player.SetPosition(Vector3(2, GH_PLAYER_HEIGHT, 2));
      vObjects.push_back(floorplan);
    } break;
  }
}
