struct scene_level1
{
  Ground ground;
  Tunnel tunnel1;
  Tunnel tunnel2;
  Portal portal1;
  Portal portal2;
  Portal portal3;
  Portal portal4;
};

static void
load_level1()

void Level1::Load(PObjectVec& objs, PPortalVec& portals, Player& player) {
  Ground_Ptr ground = (Ground_Ptr)(new Ground());
  Tunnel_Ptr tunnel1 = (Tunnel_Ptr)(new Tunnel(Tunnel::NORMAL));
  Tunnel_Ptr tunnel2 = (Tunnel_Ptr)(new Tunnel(Tunnel::NORMAL));
  Portal_Ptr portal1 = (Portal_Ptr)(new Portal());
  Portal_Ptr portal2 = (Portal_Ptr)(new Portal());
  Portal_Ptr portal3 = (Portal_Ptr)(new Portal());
  Portal_Ptr portal4 = (Portal_Ptr)(new Portal());
  tunnel1->pos = Vector3(-2.4f, 0, -1.8f);
  tunnel1->scale = Vector3(1, 1, 4.8f);
  objs.push_back(tunnel1);

  tunnel2->pos = Vector3(2.4f, 0, 0);
  tunnel2->scale = Vector3(1, 1, 0.6f);
  objs.push_back(tunnel2);

  ground->scale *= 1.2f;
  objs.push_back(ground);

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

  player.SetPosition(Vector3(0, GH_PLAYER_HEIGHT, 5));
}
