void Level4::Load(PObjectVec& objs, PPortalVec& portals, Player& player) {
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
  objs.push_back(tunnel1);

  ground1->scale *= Vector3(1, 2, 1);
  objs.push_back(ground1);

  tunnel2->pos = Vector3(200, 0, 0);
  tunnel2->scale = Vector3(1, 1, 5);
  objs.push_back(tunnel2);

  ground2->pos = Vector3(200, 0, 0);
  ground2->scale *= Vector3(1, 2, 1);
  ground2->euler.y = GH_PI;
  objs.push_back(ground2);

  tunnel1->SetDoor1(*portal1);
  portals.push_back(portal1);

  tunnel1->SetDoor2(*portal2);
  portals.push_back(portal2);

  tunnel2->SetDoor1(*portal3);
  portal3->euler.y -= GH_PI;
  portals.push_back(portal3);

  tunnel2->SetDoor2(*portal4);
  portal4->euler.y -= GH_PI;
  portals.push_back(portal4);

  Portal::Connect(portal1, portal4);
  Portal::Connect(portal2, portal3);

  player.SetPosition(Vector3(0, GH_PLAYER_HEIGHT - 2, 8));
}
