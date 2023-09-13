Engine* GH_ENGINE = nullptr;
const Input* GH_INPUT = nullptr;
int GH_REC_LEVEL = 0;

#if 0
void Engine::init_opengl(void)
{
  glewInit();
  glClearColor(0.6f, 0.9f, 1.0f, 1.0f);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glDepthMask(GL_TRUE);
  wglSwapIntervalEXT(1);

}
#endif

void Engine::cleanup(void)
{
  //this->curScene->Unload();
  this->vObjects.clear();
  this->vPortals.clear();
}

void Engine::process_input(void)
{
  if (this->input.key_press['1'])
  {
    this->LoadScene(0);
  }
  else if (this->input.key_press['2'])
  {
    this->LoadScene(1);
  }
  else if (this->input.key_press['3'])
  {
    this->LoadScene(2);
  }
  else if (this->input.key_press['4'])
  {
    this->LoadScene(3);
  }
  else if (this->input.key_press['5'])
  {
    this->LoadScene(4);
  }
  else if (this->input.key_press['6'])
  {
    this->LoadScene(5);
  }
  else if (this->input.key_press['7'])
  {
    this->LoadScene(6);
  }
}

void Engine::LoadScene(int ix)
{
  //TODO: Deallocate memory
  vObjects.clear();
  vPortals.clear();
  player->Reset();

  Scene_Ptr CurrentScene = this->vScenes[this->CurrentSceneIndex];
  CurrentScene->Load(this->vObjects, this->vPortals, *this->player);
  this->vObjects.push_back(player);
}

void Engine::Update(void)
{
  for (auto& Object : vObjects)
  {
    Assert(Object.get());
    Object->Update();
  }

  //Collisions
  //For each physics object
  for (size_t i = 0; i < vObjects.size(); ++i) {
    Physical* physical = vObjects[i]->AsPhysical();
    if (!physical) { continue; }
    Matrix4 worldToLocal = physical->WorldToLocal();

    //For each object to collide with
    for (size_t j = 0; j < vObjects.size(); ++j) {
      if (i == j) { continue; }
      Object& obj = *vObjects[j];
      if (!obj.mesh) { continue; }

      //For each hit sphere
      for (size_t s = 0; s < physical->hitSpheres.size(); ++s) {
        //Brings point from collider's local coordinates to hits's local coordinates.
        const Sphere& sphere = physical->hitSpheres[s];
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
            vObjects[j]->OnHit(*physical, push);
            physical->OnCollide(*vObjects[j], push);

            worldToLocal = physical->WorldToLocal();
            worldToUnit = sphere.LocalToUnit() * worldToLocal;
            localToUnit = worldToUnit * obj.LocalToWorld();
            unitToWorld = worldToUnit.Inverse();
          }
        }
      }
    }
  }

  //Portals
  for (size_t i = 0; i < vObjects.size(); ++i) {
    Physical* physical = vObjects[i]->AsPhysical();
    if (physical) {
      for (size_t j = 0; j < vPortals.size(); ++j) {
        if (physical->TryPortal(*vPortals[j])) {
          break;
        }
      }
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
  for (size_t i = 0; i < vObjects.size(); ++i) {
    vObjects[i]->Draw(cam, curFBO);
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
  this->process_input();

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
