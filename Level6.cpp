void Level6::Load(PObjectVec& objs, PPortalVec& portals, Player& player) {
  Floorplan_Ptr floorplan(new Floorplan);
  objs.push_back(floorplan);
  floorplan->AddPortals(portals);

  player.SetPosition(Vector3(2, GH_PLAYER_HEIGHT, 2));
}
