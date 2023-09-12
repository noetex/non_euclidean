void Level3::Load(PObjectVec& objs, PPortalVec& portals, Player& player) {
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
  objs.push_back(pillar1);

  objs.push_back(pillarRoom1);

  ground1->scale *= 2.0f;
  objs.push_back(ground1);

  statue1->pos = Vector3(0, 0.5f, 9);
  statue1->scale = Vector3(0.5f);
  statue1->euler.y = GH_PI / 2;
  objs.push_back(statue1);

  //Room 2
  pillar2->pos = Vector3(200, 0, 0);
  objs.push_back(pillar2);

  pillarRoom2->pos = Vector3(200, 0, 0);
  objs.push_back(pillarRoom2);

  ground2->pos = Vector3(200, 0, 0);
  ground2->scale *= 2.0f;
  objs.push_back(ground2);

  statue2->pos = Vector3(200, -0.4f, 9);
  statue2->scale = Vector3(14.0f);
  statue2->euler.y = GH_PI;
  objs.push_back(statue2);

  //Room 3
  pillar3->pos = Vector3(400, 0, 0);
  objs.push_back(pillar3);

  pillarRoom3->pos = Vector3(400, 0, 0);
  objs.push_back(pillarRoom3);

  ground3->pos = Vector3(400, 0, 0);
  ground3->scale *= 2.0f;
  objs.push_back(ground3);

  statue3->pos = Vector3(400, 0.9f, 9);
  statue3->scale = Vector3(1.2f);
  statue3->euler.y = GH_PI;
  objs.push_back(statue3);

  //Portals
  pillarRoom1->SetPortal(*portal1);
  portals.push_back(portal1);

  pillarRoom2->SetPortal(*portal2);
  portals.push_back(portal2);

  pillarRoom3->SetPortal(*portal3);
  portals.push_back(portal3);

  Portal::Connect(portal1->front, portal2->back);
  Portal::Connect(portal2->front, portal3->back);
  Portal::Connect(portal3->front, portal1->back);

  player.SetPosition(Vector3(0, GH_PLAYER_HEIGHT, 3));
}
