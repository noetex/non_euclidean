void Level2::Load(PObjectVec& objs, PPortalVec& portals, Player& player) {
  House_Ptr house1 = (House_Ptr)(new House("three_room.bmp"));
  House_Ptr house2;
  house1->pos = Vector3(0, 0, -20);
  objs.push_back(house1);

  if (num_rooms > 4) {
    house2.reset(new House("three_room2.bmp"));
    house2->pos = Vector3(200, 0, -20);
    objs.push_back(house2);
  }

  if (num_rooms == 1) {
    Portal_Ptr portal1 = (Portal_Ptr)(new Portal());
    Portal_Ptr portal2 = (Portal_Ptr)(new Portal());
    house1->SetDoor1(*portal1);
    portals.push_back(portal1);

    house1->SetDoor4(*portal2);
    portals.push_back(portal2);

    Portal::Connect(portal1, portal2);
  } else if (num_rooms == 2) {
    Portal_Ptr portal1 = (Portal_Ptr)(new Portal());
    Portal_Ptr portal2 = (Portal_Ptr)(new Portal());
    house1->SetDoor2(*portal1);
    portals.push_back(portal1);

    house1->SetDoor4(*portal2);
    portals.push_back(portal2);

    Portal::Connect(portal1, portal2);
  } else if (num_rooms == 3) {
    Portal_Ptr portal1 = (Portal_Ptr)(new Portal());
    Portal_Ptr portal2 = (Portal_Ptr)(new Portal());
    house1->SetDoor3(*portal1);
    portals.push_back(portal1);

    house1->SetDoor4(*portal2);
    portals.push_back(portal2);

    Portal::Connect(portal1, portal2);
  } else if (num_rooms == 4) {
  } else if (num_rooms == 5) {
    Portal_Ptr portal1 = (Portal_Ptr)(new Portal());
    Portal_Ptr portal2 = (Portal_Ptr)(new Portal());
    Portal_Ptr portal3 = (Portal_Ptr)(new Portal());
    house1->SetDoor4(*portal1);
    portals.push_back(portal1);

    house2->SetDoor2(*portal2);
    portals.push_back(portal2);

    house2->SetDoor1(*portal3);
    portals.push_back(portal3);

    Portal::Connect(portal1->front, portal2->back);
    Portal::Connect(portal2->front, portal3->back);
    Portal::Connect(portal3->front, portal1->back);
  } else if (num_rooms == 6) {
    Portal_Ptr portal1 = (Portal_Ptr)(new Portal());
    Portal_Ptr portal2 = (Portal_Ptr)(new Portal());
    Portal_Ptr portal3 = (Portal_Ptr)(new Portal());
    house1->SetDoor4(*portal1);
    portals.push_back(portal1);

    house2->SetDoor3(*portal2);
    portals.push_back(portal2);

    house2->SetDoor1(*portal3);
    portals.push_back(portal3);

    Portal::Connect(portal1->front, portal2->back);
    Portal::Connect(portal2->front, portal3->back);
    Portal::Connect(portal3->front, portal1->back);
  }

  player.SetPosition(Vector3(3, GH_PLAYER_HEIGHT, 3));
}
