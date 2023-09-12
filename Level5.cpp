void Level5::Load(PObjectVec& objs, PPortalVec& portals, Player& player) {
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
  objs.push_back(tunnel1);

  ground1->scale *= 1.2f;
  objs.push_back(ground1);

  tunnel2->pos = Vector3(201.2f, 0, 0);
  tunnel2->scale = Vector3(1, 1, 2.4f);
  objs.push_back(tunnel2);

  ground2->pos = Vector3(200, 0, 0);
  ground2->scale *= 1.2f;
  objs.push_back(ground2);

  tunnel1->SetDoor1(*portal1);
  portals.push_back(portal1);

  tunnel2->SetDoor1(*portal2);
  portals.push_back(portal2);

  tunnel1->SetDoor2(*portal3);
  portals.push_back(portal3);

  tunnel2->SetDoor2(*portal4);
  portals.push_back(portal4);

  Portal::Connect(portal1, portal2);
  Portal::Connect(portal3, portal4);

  tunnel3->pos = Vector3(-1, 0, -4.2f);
  tunnel3->scale = Vector3(0.25f, 0.25f, 0.6f);
  tunnel3->euler.y = GH_PI/2;
  objs.push_back(tunnel3);

  player.SetPosition(Vector3(0, GH_PLAYER_HEIGHT, 5));
}
